#include "apc.h"
#include "test.h"
#include "Resource.h"
#include "Image.h"


SIZE_T ApcStateOffset;

ZwTestAlertT ZwTestAlert;
ZwQueueApcThreadT g_ZwQueueApcThread;
ZwQueryVirtualMemory_PFN ZwQueryVirtualMemoryFn;



//#define CopyCode       //���ƴ���ķ�ʽ��
#define UseUserFun   //ֱ�����û��ӵ�API��ʽ��LoadLibraryExW����


//////////////////////////////////////////////////////////////////////////////////////////////////


BOOL IsExcludeProcess(PCLIENT_ID ClientId)
/*
�ų�����Ҫע��Ľ��̡�
*/
{
    if (ClientId->UniqueProcess == 0 || PsGetProcessId(PsInitialSystemProcess) == ClientId->UniqueProcess) {
        return TRUE;
    }

    /*
    ��ȡ�ں˾����
    */
    PEPROCESS  Process = 0;
    NTSTATUS status = PsLookupProcessByProcessId(ClientId->UniqueProcess, &Process);
    if (!NT_SUCCESS(status)) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "0x%#x", status);
        return TRUE;
    }
    ObDereferenceObject(Process); //΢������ϡ�
    HANDLE  KernelProcessHandle = NULL;
    status = ObOpenObjectByPointer(Process,
                                   OBJ_KERNEL_HANDLE,
                                   NULL,
                                   GENERIC_READ,
                                   *PsProcessType,
                                   KernelMode,
                                   &KernelProcessHandle);
    if (!NT_SUCCESS(status)) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "0x%#x", status);
        return TRUE;
    }

    BOOLEAN SecureProcess = FALSE;
    NTSTATUS Status = IsSecureProcess(KernelProcessHandle, &SecureProcess);
    if (SecureProcess) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "SecureProcess:%p", ClientId->UniqueProcess);
        ZwClose(KernelProcessHandle);
        return TRUE;// STATUS_NOT_SUPPORTED;
    }

#ifdef _WIN64
    //if (PsIsProtectedProcess(Process)) {
    //    Print(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "ProtectedProcess:%p", ClientId->UniqueProcess);
    //    ZwClose(KernelProcessHandle);
    //    return TRUE;
    //}
#endif

    BOOLEAN ProtectedProcess = FALSE;
    Status = IsProtectedProcess(KernelProcessHandle, &ProtectedProcess);
    if (SecureProcess) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "ProtectedProcess:%p", ClientId->UniqueProcess);
        ZwClose(KernelProcessHandle);
        return TRUE;
    }

    ZwClose(KernelProcessHandle);

    return FALSE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef CopyCode


void ApcCallback(PVOID NormalContext, PVOID SystemArgument1, PVOID SystemArgument2)
/*
Ҫ���Ƹ��û�̬�Ĵ��롣

���������ͷ�������������û�̬�ڴ档

�������������֧��/guard (Enable Control Flow Guard)��
�����������û���⡣
���������֧��/guard��һ��Ҫuse /guard:cf-.
�������������ĺ������û��ɣ�IDA�鿴����call    cs:__guard_dispatch_icall_fptr
��������Ը�����һ����Ч���ڴ档
����رպ󣬻��Ϊ��IDA�鿴����call    qword ptr [rax+XXh]
����Ƿ�������ġ�

ע�⣺�������Ҫ֧��X86��X64��WOW64.
*/
{
    PPassToUser pa2 = (PPassToUser)SystemArgument2;
    SIZE_T  RegionSize = pa2->RegionSize;

    //__debugbreak();

    pa2->LoadLibraryW((LPCTSTR)pa2->FullDllPathName);
    pa2->NtFreeVirtualMemory(NtCurrentProcess(), &SystemArgument2, &RegionSize, MEM_RELEASE);
}
void ApcCallbackEnd()
{

}


VOID InitialUserRoutine(PCLIENT_ID ClientId, PSIZE_T UserApcCallbackAddr)
/*
�����û�̬���ڴ棬���Ѵ��븴�ƹ�ȥ��

��ʱ���룬�û�����ڴ�Ҳ�������룬
��һƬӦ�ò㲻�õ��ҿ�д���ڴ棬ֱ��д��ȥ������ס����ڴ棬�ú󵱲������ݹ�ȥ���ɡ�
*/
{
    PEPROCESS    Process;
    NTSTATUS status = PsLookupProcessByProcessId(ClientId->UniqueProcess, &Process);
    ASSERT(NT_SUCCESS(status));

    HANDLE  Handle = 0;
    status = ObOpenObjectByPointer(Process,
                                   OBJ_KERNEL_HANDLE,
                                   NULL,
                                   GENERIC_ALL,
                                   *PsProcessType,
                                   KernelMode,
                                   &Handle);
    ASSERT(NT_SUCCESS(status));

    SIZE_T size = 0;

    if (((SIZE_T)ApcCallbackEnd - (SIZE_T)ApcCallback) > 0) {
        size = (SIZE_T)ApcCallbackEnd - (SIZE_T)ApcCallback;
    } else {
        size = (SIZE_T)ApcCallback - (SIZE_T)ApcCallbackEnd;
    }

    SIZE_T CodeSize = size;

    PVOID BaseAddress = 0;//�����ƶ�Ϊ0�����򷵻ز������� 
    status = ZwAllocateVirtualMemory(Handle  /*NtCurrentProcess()*/,
                                     &BaseAddress,
                                     0,
                                     &size,
                                     MEM_COMMIT,
                                     PAGE_EXECUTE_READWRITE);
    if (!NT_SUCCESS(status)) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "status:%#x", status);
        ObDereferenceObject(Process);
        ZwClose(Handle);
        return;
    }

    KAPC_STATE   ApcState;
    KeStackAttachProcess(Process, &ApcState);
    __try {
        RtlZeroMemory(BaseAddress, CodeSize);
        RtlCopyMemory(BaseAddress, ApcCallback, CodeSize);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        KdBreakPoint();
    }
    KeUnstackDetachProcess(&ApcState);

    *UserApcCallbackAddr = (SIZE_T)BaseAddress;

    ObDereferenceObject(Process);
    ZwClose(Handle);
}


VOID InitialUserArgument(HANDLE UniqueProcess, PSIZE_T UserArgument)
/*

ע�ͣ�
smss.exeֻ��ntll.dll������û��kernel32.dll�����Ǹ�native����
*/
{
    PEPROCESS    Process;
    NTSTATUS status = PsLookupProcessByProcessId(UniqueProcess, &Process);
    ASSERT(NT_SUCCESS(status));

    HANDLE  Handle = 0;
    status = ObOpenObjectByPointer(Process,
                                   OBJ_KERNEL_HANDLE,
                                   NULL,
                                   GENERIC_ALL,
                                   *PsProcessType,
                                   KernelMode,
                                   &Handle);
    ASSERT(NT_SUCCESS(status));

    SIZE_T size = sizeof(PassToUser);
    SIZE_T CodeSize = size;

    PVOID BaseAddress = 0;//�����ƶ�Ϊ0�����򷵻ز�������  
    status = ZwAllocateVirtualMemory(Handle, &BaseAddress, 0, &size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!NT_SUCCESS(status)) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "status:%#x", status);
        ObDereferenceObject(Process);
        ZwClose(Handle);
        return;
    }

    KAPC_STATE   ApcState;
    KeStackAttachProcess(Process, &ApcState); //���ӵ�ǰ�̵߳�Ŀ����̿ռ���   

    __try {//ע�⣺��������У����̿��ܻ��˳���
        RtlZeroMemory(BaseAddress, CodeSize);
        PPassToUser pUserData = (PPassToUser)BaseAddress;
        pUserData->RegionSize = size;

        PPEB ppeb = PsGetProcessPeb(Process);//ע�⣺IDLE��system������Ӧ�û�ȡ������
        if (ppeb && ppeb->Ldr) {//��������ʱ��LdrΪ�ա�
            PLDR_DATA_TABLE_ENTRY pldte;
            UNICODE_STRING ntdll = RTL_CONSTANT_STRING(L"ntdll.dll");
            UNICODE_STRING kernel32 = RTL_CONSTANT_STRING(L"kernel32.dll");
            UNICODE_STRING KernelBase = RTL_CONSTANT_STRING(L"KernelBase.dll");

            PLIST_ENTRY le1 = ppeb->Ldr->InMemoryOrderModuleList.Flink;
            PLIST_ENTRY le2 = le1;

            do {
                pldte = (PLDR_DATA_TABLE_ENTRY)CONTAINING_RECORD(le1, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
                if (pldte->FullDllName.Length) //���˵����һ��������ġ�
                {
                    //KdPrint(("FullDllName:%wZ \n", &pldte->FullDllName));
                    //"C:\WINDOWS\system32\USER32.dll"����Ϊ����������·������һ��˼·�ǻ�ȡϵͳ·������ϡ�

                    PUNICODE_STRING pus = (PUNICODE_STRING)&pldte->Reserved4;
                    //if (RtlCompareUnicodeString(&pldte->FullDllName, &user32, TRUE) == 0)

                    if (RtlCompareUnicodeString(pus, &ntdll, TRUE) == 0) {
                        ANSI_STRING NtFreeVirtualMemory = RTL_CONSTANT_STRING("NtFreeVirtualMemory");

                        pUserData->ntdll = pldte->DllBase;

                        pUserData->NtFreeVirtualMemory = MiFindExportedRoutineByName(pldte->DllBase, &NtFreeVirtualMemory);
                        ASSERT(pUserData->NtFreeVirtualMemory);
                    }

                    if (RtlCompareUnicodeString(pus, &kernel32, TRUE) == 0) {
                        ANSI_STRING LoadLibraryW = RTL_CONSTANT_STRING("LoadLibraryW");
                        ANSI_STRING GetProcAddress = RTL_CONSTANT_STRING("GetProcAddress");

                        pUserData->kernel32 = pldte->DllBase;

                        pUserData->LoadLibraryW = MiFindExportedRoutineByName(pldte->DllBase, &LoadLibraryW);
                        ASSERT(pUserData->LoadLibraryW);

                        pUserData->GetProcAddress = MiFindExportedRoutineByName(pldte->DllBase, &GetProcAddress);
                        ASSERT(pUserData->GetProcAddress);
                    }
                }

                le1 = le1->Flink;
            } while (le1 != le2);
        }

        if (IsProcessPe64(UniqueProcess)) {
            RtlCopyMemory(pUserData->FullDllPathName, g_us_FullDllPathName.Buffer, g_us_FullDllPathName.Length);
        } else {

        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "ExceptionCode:%#X", GetExceptionCode());
    }

    KeUnstackDetachProcess(&ApcState);//�������

    *UserArgument = (SIZE_T)BaseAddress;

    ObDereferenceObject(Process);
    ZwClose(Handle);
}


NTSTATUS QueueApcThread(PCLIENT_ID ClientId)
/*
64λ��Ҫ����WOW64���̡�

��win8��Ҫ�ر�SMEP��
*/
{
    if (IsExcludeProcess(ClientId)) {
        return STATUS_UNSUCCESSFUL;
    }

    SIZE_T Argument = 0;
    InitialUserArgument(ClientId->UniqueProcess, &Argument);
    if (!Argument) {
        return STATUS_UNSUCCESSFUL;
    }

    SIZE_T UserRoutine;
    InitialUserRoutine(ClientId, &UserRoutine);
    ASSERT(UserRoutine != 0);

    NTSTATUS Status = ZwTestAlert();
    ASSERT(NT_SUCCESS(Status));

    Status = NtQueueApcThreadEx(ClientId->UniqueThread,
                                (PPS_APC_ROUTINE)UserRoutine,
                                NULL,
                                NULL,
                                (PVOID)Argument);
    if (!NT_SUCCESS(Status)) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "Status:%#x, pid:%p", Status, ClientId->UniqueProcess);
    }

    return Status;
}


#endif


//////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef UseUserFun


PVOID GetLoadLibraryExWAddress(HANDLE UniqueProcess)
/*
���ܣ���ȡLoadLibraryExW�ĵ�ַ��

ע�⣺֧��X64��X86���Լ�WOW64(\Windows\SysWOW64\kernel32.dll).

�㷢��û��APC���û�̬�ص�������LoadLibraryEx�Ĳ�������һ��������������
���԰�APC���û�̬�ص�����ֱ������ΪLoadLibraryExA/W���Ǹ�����
������Ҳ����shellcode,�������Լ������������ڴ��ٸ��ƴ��루����shellcode���ˡ�
ע�⺯���Ĳ����ĵ��÷�ʽ��
������������һ��ȱ�㣬�޷����б�Ĳ������磺�ͷ��ڴ档
*/
{
    PVOID LoadLibraryExWAddress = NULL;
    PEPROCESS    Process;
    BOOL IsProcess64 = IsProcessPe64(UniqueProcess);
    NTSTATUS status = PsLookupProcessByProcessId(UniqueProcess, &Process);
    ASSERT(NT_SUCCESS(status));

    KAPC_STATE   ApcState;
    KeStackAttachProcess(Process, &ApcState);

    __try {//ע�⣺��������У����̿��ܻ��˳���
        PPEB ppeb = PsGetProcessPeb(Process);//ע�⣺IDLE��system������Ӧ�û�ȡ������
        if (ppeb && ppeb->Ldr) {//��������ʱ��LdrΪ�ա�
            PLDR_DATA_TABLE_ENTRY pldte;
            UNICODE_STRING kernel32 = RTL_CONSTANT_STRING(L"kernel32.dll");

            PLIST_ENTRY le1 = ppeb->Ldr->InMemoryOrderModuleList.Flink;
            PLIST_ENTRY le2 = le1;

            do {
                pldte = (PLDR_DATA_TABLE_ENTRY)CONTAINING_RECORD(le1, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
                if (pldte->FullDllName.Length) //���˵����һ��������ġ�
                {
                    //KdPrint(("FullDllName:%wZ \n", &pldte->FullDllName));
                    //"C:\WINDOWS\system32\USER32.dll"����Ϊ����������·������һ��˼·�ǻ�ȡϵͳ·������ϡ�

                    PUNICODE_STRING pus = (PUNICODE_STRING)&pldte->Reserved4;
                    //if (RtlCompareUnicodeString(&pldte->FullDllName, &user32, TRUE) == 0)

                    ANSI_STRING LoadLibraryExW = RTL_CONSTANT_STRING("LoadLibraryExW");

#ifdef _WIN64
                    if (IsProcessPe64) {
                        if (RtlCompareUnicodeString(pus, &kernel32, TRUE) == 0) {
                            LoadLibraryExWAddress = MiFindExportedRoutineByName(pldte->DllBase, &LoadLibraryExW);
                            break;
                        }
                    } else {//WOW64.
                        if (RtlCompareUnicodeString(pus, &kernel32, TRUE) == 0) {
                            LoadLibraryExWAddress = MiFindExportedRoutineByName(pldte->DllBase, &LoadLibraryExW);
                            break;
                        }
                    }
#else
                    if (RtlCompareUnicodeString(pus, &kernel32, TRUE) == 0) {
                        LoadLibraryExWAddress = MiFindExportedRoutineByName(pldte->DllBase, &LoadLibraryExW);
                        break;
                    }
#endif
                }

                le1 = le1->Flink;
            } while (le1 != le2);
        }

    #if defined(_WIN64)
        //�����WOW64������Ҫִ������Ĵ��룬����Ҫ����ж�WOW64�Ĵ��롣
        //ZwQueryInformationProcess +��ProcessWow64Information
        //PWOW64_PROCESS pwp = (PWOW64_PROCESS)PsGetProcessWow64Process(Process);
        //if (NULL != pwp) {
        //    EnumWow64Module(pwp, CallBack, Context);
        //}
    #endif
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "ExceptionCode:%#X", GetExceptionCode());
    }

    KeUnstackDetachProcess(&ApcState);

    ObDereferenceObject(Process);

    return LoadLibraryExWAddress;
}


PVOID SetDllFullPath(HANDLE UniqueProcess)
/*
���ܣ���Ŀ�����������һ���ڴ��û��洢DLL��·����

ע�⣺֧��X64��X86���Լ�WOW64.
*/
{
    BOOL IsProcess64 = IsProcessPe64(UniqueProcess);
    PEPROCESS Process = NULL;
    HANDLE  Handle = 0;
    PVOID DllFullPath = NULL;//�����ƶ�Ϊ0�����򷵻ز������� 

    __try {
        NTSTATUS status = PsLookupProcessByProcessId(UniqueProcess, &Process);
        if (!NT_SUCCESS(status)) {
            Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "status:%#x, pid:%d", status, HandleToULong(UniqueProcess));
            __leave;
        }

        status = ObOpenObjectByPointer(Process, OBJ_KERNEL_HANDLE, NULL, GENERIC_ALL, *PsProcessType, KernelMode, &Handle);
        if (!NT_SUCCESS(status)) {
            Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "status:%#x, pid:%d", status, HandleToULong(UniqueProcess));
            __leave;
        }

        SIZE_T size = 0;
    #ifdef _WIN64
        if (IsProcess64) {
            size = g_us_FullDllPathName.Length;
        } else {//WOW64.
            size = g_us_FullDllPathNameWow64.Length;
        }
    #else
        size = g_us_FullDllPathName.Length;
    #endif
         
        status = ZwAllocateVirtualMemory(Handle, &DllFullPath, 0, &size, MEM_COMMIT, PAGE_READWRITE);
        if (!NT_SUCCESS(status)) {//�����PAGE_EXECUTE_READWRITE�����STATUS_DYNAMIC_CODE_BLOCKED.
            Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "status:%#x, pid:%d, size:%lld",
                  status, HandleToULong(UniqueProcess), size);
            __leave;
        }

        KAPC_STATE   ApcState;
        KeStackAttachProcess(Process, &ApcState);

        __try {
        #ifdef _WIN64
            if (IsProcess64) {
                RtlCopyMemory(DllFullPath, g_us_FullDllPathName.Buffer, g_us_FullDllPathName.Length);
            } else {//WOW64.
                RtlCopyMemory(DllFullPath, g_us_FullDllPathNameWow64.Buffer, g_us_FullDllPathNameWow64.Length);
            }
        #else
            RtlCopyMemory(DllFullPath, g_us_FullDllPathName.Buffer, g_us_FullDllPathName.Length);
        #endif
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "ExceptionCode:%#X", GetExceptionCode());
        }

        KeUnstackDetachProcess(&ApcState);//�������
    } __finally {
        if (Process) {
            ObDereferenceObject(Process);
        }

        if (Handle) {
            ZwClose(Handle);
        }
    }

    return DllFullPath;
}


NTSTATUS NTAPI ZwQueueApcThreadEx(__in HANDLE ThreadHandle,
                                  __in PPS_APC_ROUTINE ApcRoutine,
                                  __in_opt PVOID ApcArgument1,
                                  __in_opt PVOID ApcArgument2,
                                  __in_opt PVOID ApcArgument3
)
/*
��NtQueueApcThread�ķ�װ��

��һ�����������û�̬�ľ������ʵ��tid.
*/
{
    PETHREAD Thread;
    NTSTATUS Status = PsLookupThreadByThreadId(ThreadHandle, &Thread);
    if (NT_SUCCESS(Status)) {
        if (PsIsSystemThread(Thread)) {
            Status = STATUS_INVALID_HANDLE;
        } else {
            HANDLE KernelHandle;
            Status = ObOpenObjectByPointer(Thread,
                                           OBJ_KERNEL_HANDLE,
                                           NULL,
                                           THREAD_ALERT,
                                           *PsThreadType,
                                           KernelMode,
                                           &KernelHandle);
            if (NT_SUCCESS(Status)) {
                Status = g_ZwQueueApcThread(KernelHandle, ApcRoutine, ApcArgument1, ApcArgument2, ApcArgument3);
                if (!NT_SUCCESS(Status)) {
                    PrintEx(DPFLTR_FLTMGR_ID, DPFLTR_ERROR_LEVEL, "Status:%#x", Status);
                }

                ZwClose(KernelHandle);
            }
        }

        ObDereferenceObject(Thread);
    }

    return Status;
}


NTSTATUS QueueApcThread(PCLIENT_ID ClientId)
/*
64λ��Ҫ����WOW64���̡�

��win8��Ҫ�ر�SMEP��
*/
{
    //if (IsExcludeProcess(ClientId)) {
    //    return STATUS_UNSUCCESSFUL;
    //}

    PVOID Argument = SetDllFullPath(ClientId->UniqueProcess);
    if (!Argument) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "pid:%d", HandleToUlong(ClientId->UniqueProcess));
        return STATUS_UNSUCCESSFUL;
    }

    //PPS_APC_ROUTINE UserRoutine = GetLoadLibraryExWAddress(ClientId->UniqueProcess);

    PPS_APC_ROUTINE UserRoutine = NULL;
    if (IsWow64Process(ClientId->UniqueProcess)) {
        UserRoutine = (PPS_APC_ROUTINE)LoadLibraryExWWow64Fn;
    } else {
        UserRoutine = (PPS_APC_ROUTINE)LoadLibraryExWFn;
    }  

    if (!UserRoutine) {
        //Print(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "pid:%d", HandleToUlong(ClientId->UniqueProcess));
        return STATUS_UNSUCCESSFUL;
    }

    if (!IsLoadKernel32(ClientId->UniqueProcess)) {
        return STATUS_UNSUCCESSFUL;
    }

    NTSTATUS Status = ZwTestAlert();
    if (!NT_SUCCESS(Status)) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "Status:%#x, pid:%d, tid:%d",
              Status, HandleToULong(ClientId->UniqueProcess), HandleToULong(ClientId->UniqueThread));
    }

    if (g_ZwQueueApcThread) {//����ʹ��ϵͳ�ġ�
        Status = ZwQueueApcThreadEx(ClientId->UniqueThread, UserRoutine, Argument, NULL, NULL);
    } else {
        Status = NtQueueApcThreadEx(ClientId->UniqueThread, UserRoutine, Argument, NULL, NULL);        
    }

    if (!NT_SUCCESS(Status)) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "Status:%#x, pid:%d, tid:%d",
              Status, HandleToULong(ClientId->UniqueProcess), HandleToULong(ClientId->UniqueThread));
    }

    return Status;
}


#endif // UseUserFun


//////////////////////////////////////////////////////////////////////////////////////////////////
