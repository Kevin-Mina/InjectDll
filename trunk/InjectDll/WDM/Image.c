#include "Image.h"
#include "Inject.h"
#include "apc.h"
#include "Process.h"


UNICODE_STRING g_Ntkernel32Path = {0};
UNICODE_STRING g_DosKernel32Path = {0};

UNICODE_STRING g_NtkernelWow64Path = {0};
UNICODE_STRING g_DosKernelWow64Path = {0};

SIZE_T LoadLibraryExWFn;//ĳ�������е�L"\\SystemRoot\\System32\\kernel32.dll"���LoadLibraryExW��ַ��
#ifdef _WIN64
SIZE_T LoadLibraryExWWow64Fn;//ĳ��WOW64�����е�L"\\SystemRoot\\SysWOW64\\kernel32.dll"���LoadLibraryExW��ַ��
#endif


SIZE_T LoadLibraryWFn;//ĳ�������е�L"\\SystemRoot\\System32\\kernel32.dll"���LoadLibraryW��ַ��
#ifdef _WIN64
SIZE_T LoadLibraryWWow64Fn;//ĳ��WOW64�����е�L"\\SystemRoot\\SysWOW64\\kernel32.dll"���LoadLibraryW��ַ��
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////


void GetKernel32FullPath()
{
    UNICODE_STRING g_kernel32 = RTL_CONSTANT_STRING(L"\\SystemRoot\\System32\\kernel32.dll");
    GetSystemRootName(&g_kernel32, &g_Ntkernel32Path, &g_DosKernel32Path);

#ifdef _WIN64
    UNICODE_STRING g_kernelWow64 = RTL_CONSTANT_STRING(L"\\SystemRoot\\SysWOW64\\kernel32.dll");
    GetSystemRootName(&g_kernelWow64, &g_NtkernelWow64Path, &g_DosKernelWow64Path);
#endif
}


BOOL IsLoadKernel32(HANDLE UniqueProcess)
/*
���ܣ��ж�һ�����̣�����WOW64)�Ƿ����kernel32.dll��

��׼�İ취����һ�������������ڽ��̻ص���IMAGE�ص�����ͳ�ơ�

������һ���򵥵İ취��������ȡLoadLibraryEx�ĵ�һ��ָ��ĵ�һ���ֽڣ�����������쳣��
*/
{
    PPS_APC_ROUTINE UserRoutine = (PPS_APC_ROUTINE)LoadLibraryExWFn;
    BOOL ret = FALSE;
    NTSTATUS Status;
    PEPROCESS  Process = NULL;
    HANDLE  KernelHandle = NULL;
    KAPC_STATE   ApcState;

    if (IsWow64Process(UniqueProcess)) {
        UserRoutine = (PPS_APC_ROUTINE)LoadLibraryExWWow64Fn;
    }

    __try {
        Status = PsLookupProcessByProcessId(UniqueProcess, &Process);
        if (!NT_SUCCESS(Status)) {
            Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "0x%#x", Status);
            __leave;
        }

        Status = ObOpenObjectByPointer(Process,
                                       OBJ_KERNEL_HANDLE,
                                       NULL,
                                       GENERIC_READ,
                                       *PsProcessType,
                                       KernelMode,
                                       &KernelHandle);
        if (!NT_SUCCESS(Status)) {
            Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "0x%#x", Status);
            __leave;
        }

        KeStackAttachProcess(Process, &ApcState);

        ret = TRUE;

        __try {
            PCHAR Temp = (PCHAR)*(PCHAR)UserRoutine;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            ret = FALSE;
        }

        KeUnstackDetachProcess(&ApcState);
    } __finally {
        if (KernelHandle) {
            ZwClose(KernelHandle);
        }

        if (Process) {
            ObDereferenceObject(Process);
        }
    }

    return ret;
}


PVOID GetLoadLibraryExWAddressByPid(HANDLE UniqueProcess)
/*
���ܣ���ȡһ�����̵�LoadLibraryExW�����ĵ�ַ��

����������ڵ��ļ��ǣ�
L"\\SystemRoot\\System32\\kernel32.dll"
L"\\SystemRoot\\SysWOW64\\kernel32.dll"
*/
{
    PVOID UserRoutine = NULL;

    //����һ�Ĳ��ԡ�
    //UserRoutine = GetUserFunctionAddress(UniqueProcess, g_Ntkernel32Path.Buffer, "LoadLibraryExW");
    //if (!UserRoutine) {
    //#ifdef _WIN64 //Ϊ�����API��ʱ��Ϊ�˼ӿ��ٶȣ�����ж��ǲ���WOW64����.
    //    UserRoutine = GetUserFunctionAddress(UniqueProcess, g_NtkernelWow64Path.Buffer, "LoadLibraryExW");
    //    if (UserRoutine) {
    //        LoadLibraryExWWow64Fn = (SIZE_T)UserRoutine;
    //    }
    //#endif
    //} else {
    //    LoadLibraryExWFn = (SIZE_T)UserRoutine;
    //}

    if (IsWow64Process(UniqueProcess)) {
        LoadLibraryWWow64Fn = (SIZE_T)GetUserFunctionAddress(UniqueProcess, g_NtkernelWow64Path.Buffer, "LoadLibraryW");
        LoadLibraryExWWow64Fn = (SIZE_T)GetUserFunctionAddress(UniqueProcess, g_NtkernelWow64Path.Buffer, "LoadLibraryExW");
    } else {
        LoadLibraryWFn = (SIZE_T)GetUserFunctionAddress(UniqueProcess, g_Ntkernel32Path.Buffer, "LoadLibraryW");
        LoadLibraryExWFn = (SIZE_T)GetUserFunctionAddress(UniqueProcess, g_Ntkernel32Path.Buffer, "LoadLibraryExW");
    }

    //�������Ĳ��ԣ���֧��WOW64��
    //UserRoutine = GetUserFunctionAddressByPeb(UniqueProcess, g_DosKernel32Path.Buffer, "LoadLibraryExW");
    //if (!UserRoutine) {
    //#ifdef _WIN64 //Ϊ�����API��ʱ��Ϊ�˼ӿ��ٶȣ�����ж��ǲ���WOW64����.
    //    UserRoutine = GetUserFunctionAddressByPeb(UniqueProcess, g_DosKernelWow64Path.Buffer, "LoadLibraryExW");
    //#endif
    //}

    return UserRoutine;
}


NTSTATUS WINAPI GetLoadLibraryExWAddressCallBack(_In_ HANDLE UniqueProcessId, _In_opt_ PVOID Context)
{
    PEPROCESS Process = NULL;
    NTSTATUS  status = STATUS_UNSUCCESSFUL;

    UNREFERENCED_PARAMETER(Context);

    status = PsLookupProcessByProcessId(UniqueProcessId, &Process);
    if (!NT_SUCCESS(status)) {

        return status;
    }

    __try {
        status = STATUS_UNSUCCESSFUL;

        if (0 == UniqueProcessId) {

            __leave;
        }

        if (PsGetProcessId(PsInitialSystemProcess) == UniqueProcessId) {

            __leave;
        }

        //if (PsIsSystemProcess(Process)) {//���̫�ࡣ
        //    Print(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "SystemProcess:%d, %s",
        //          HandleToUlong(UniqueProcessId), PsGetProcessImageFileName(Process));
        //    __leave;
        //}

        if (PsIsProtectedProcess(Process)) {
            Print(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "ProtectedProcess:%d, %s",
                  HandleToUlong(UniqueProcessId), PsGetProcessImageFileName(Process));
            __leave;
        }

        //IsSecureProcess

        PVOID UserRoutine = GetLoadLibraryExWAddressByPid(UniqueProcessId);
        if (LoadLibraryExWFn && LoadLibraryExWWow64Fn && LoadLibraryWFn && LoadLibraryWWow64Fn) {
            status = STATUS_SUCCESS;//ֹͣ������
        }
    } __finally {
        ObDereferenceObject(Process);
    }

    return status;
}


NTSTATUS GetLoadLibraryExWAddressByEnum()
{
    return EnumProcess(GetLoadLibraryExWAddressCallBack, NULL);
}


//////////////////////////////////////////////////////////////////////////////////////////////////


VOID ImageNotifyRoutine(_In_opt_ PUNICODE_STRING FullImageName,
                        _In_ HANDLE ProcessId,
                        _In_ PIMAGE_INFO ImageInfo
)
/*
��ʱ�ܼ�ص���ע���DLL��

���ʱ���ж�ע��ɹ������������ĵ�IsCanInject�����ж�RtlCreateUserThread��׼ȷ�����ţ����ǷѾ�����ʱ��
*/
{
    if (ImageInfo->SystemModeImage) {
        return;
    }

    if (0 == LoadLibraryWFn || 0 == LoadLibraryWWow64Fn) {
        GetLoadLibraryExWAddressByPid(ProcessId);
    }

//    UNICODE_STRING LoadImageFullName = {0};
//    BOOL IsKernel32 = FALSE;
//
//    RtlGetLoadImageFullName(&LoadImageFullName, FullImageName, ProcessId, ImageInfo);
//
//#ifdef _WIN64
//    if (RtlCompareUnicodeString(&LoadImageFullName, &g_DosKernel32Path, TRUE) == 0 ||
//        RtlCompareUnicodeString(&LoadImageFullName, &g_DosKernelWow64Path, TRUE) == 0) {
//        IsKernel32 = TRUE;
//    }
//#else
//    if (RtlCompareUnicodeString(&LoadImageFullName, &g_DosKernel32Path, TRUE) == 0) {
//        IsKernel32 = TRUE;
//    }
//#endif

    PPROCESS_CONTEXT Context = GetProcessContext(ProcessId);
    if (Context) {
        if (!Context->IsInjected) {//Context->IsCanInject && 
            InjectOneProcess(ProcessId, NULL);//�ܻ�ɹ��İɣ���ѡ��ʱ���ˣ�ʧ����Ҳ����ν��
        }
    }

    //if (IsKernel32) {
    //    PROCESS_CONTEXT Temp = {0};
    //    Temp.Pid = ProcessId;
    //    Temp.IsCanInject = TRUE;
    //    UpdateProcessContext(&Temp);
    //}    

    //FreeUnicodeString(&LoadImageFullName);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
