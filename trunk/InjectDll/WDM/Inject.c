#include "Inject.h"
#include "apc.h"
#include "Image.h"
#include "Process.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


NTSTATUS WINAPI InjectOneThread(_In_ PCLIENT_ID Cid, _In_opt_ PVOID Context)
{
    UNREFERENCED_PARAMETER(Context);

    //   ֻע��һ�Ρ�  ���۲죺5�����̵߳ĵȴ�״̬������Ķ���û���ҵ���THREAD_ALERT == 4
    //   ע�⣺ApcState��Alerted��Alertable�⼸����Ա�Ĳ�ͬ��
    //if (5 == psti->ThreadState && (g_b == 1)) 
    {
        //ע�⣬��������ˣ���ʱ���л�δ֪�������Ҫ���룬������Ҫ�����ӣ�������ʮ���ӣ�
        //����ļ���Сʱ���߼��켸ҹ��������Զ�������У��磺�����������̵߳�״̬�����ϡ�
        QueueApcThread(Cid);
    }

    return STATUS_UNSUCCESSFUL;//����������
}


NTSTATUS InjectAllThread(__in HANDLE UniqueProcessId)
/*
ע��ǰ��ע�����
1.�ܲ��ܴ򿪽��̡�
2.������û���û��ռ䡣
3.��X64��Ҫ�������ǲ���WOW64���̡�
4.����.net��java�ȳ���Ҫ��Ҫע�롣
5.���ﲻ����WSL�µ�linux���̡�
*/
{
    EnumThread(UniqueProcessId, InjectOneThread, NULL);

    return STATUS_SUCCESS;
}


NTSTATUS InjectDllByRtlCreateUserThread(HANDLE Process)
/*

ע�⣺WOW64�Ĵ���
"\\SystemRoot\\System32\\kernel32.dll"
"\\SystemRoot\\SysWOW64\\kernel32.dll"

��̾��
��ô���ɺϡ�
PUSER_THREAD_START_ROUTINE��LoadLibraryW��ԭ�;�Ȼһ�¡�
���ԣ���ʡȥ����Ӧ�ò������ִ���ڴ�Ĳ�����
��Ȼ������Ǹ��ƴ��루���Բ���shellcode����ȻҪ֧��WOW64����Ӧ�ò�Ĳ�����
������˵shellcode�ˡ�

ע�⣺WOW64�Ĳ����Ĵ�С���磺ָ���size_t�ȡ�

DllPullPath���ڵ��ڴ���Ӧ�ò�ġ�

ֻ��ע�룬���ܶ�����¡�
*/
{
    NTSTATUS Status = STATUS_SUCCESS;    

    LPCWSTR DllPullPath = SetDllFullPath(Process);
    if (!DllPullPath) {
        Print(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "pid:%d", HandleToUlong(Process));
        return STATUS_UNSUCCESSFUL;
    }

    PUSER_THREAD_START_ROUTINE LoadLibraryW = NULL;//LoadLibraryW�ĵ�ַ��
    if (IsWow64Process(Process)) {
        LoadLibraryW = (PUSER_THREAD_START_ROUTINE)LoadLibraryWWow64Fn;
    } else {
        LoadLibraryW = (PUSER_THREAD_START_ROUTINE)LoadLibraryWFn;
    }

    if (!LoadLibraryW) {
        //Print(DPFLTR_DEFAULT_ID, DPFLTR_WARNING_LEVEL, "pid:%d", HandleToUlong(ClientId->UniqueProcess));
        return STATUS_UNSUCCESSFUL;
    }

    if (!IsLoadKernel32(Process)) {
        return STATUS_UNSUCCESSFUL;
    }

    Status = CreateUserThread(Process, LoadLibraryW, (PVOID)DllPullPath);
    if (NT_SUCCESS(Status)) {
        
    }

    return Status;
}


NTSTATUS WINAPI InjectOneProcess(_In_ HANDLE UniqueProcessId, _In_opt_ PVOID Context)
{
    PEPROCESS Process = NULL;
    NTSTATUS  status = STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(Context);

    status = PsLookupProcessByProcessId(UniqueProcessId, &Process);
    if (!NT_SUCCESS(status)) {

        return status;
    }

    __try {
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

        //InjectAllThread(UniqueProcessId);
        status = InjectDllByRtlCreateUserThread(UniqueProcessId);
        if (NT_SUCCESS(status)) {
            PROCESS_CONTEXT Temp = {0};
            Temp.Pid = UniqueProcessId;
            Temp.IsInjected = TRUE;
            UpdateProcessContext(&Temp);
        }
    } __finally {
        ObDereferenceObject(Process);
    }

    return STATUS_UNSUCCESSFUL;//����������
}


NTSTATUS InjectAllProcess(VOID)
{
    return EnumProcess(InjectOneProcess, NULL);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
