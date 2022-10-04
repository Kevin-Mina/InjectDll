#include "Thread.h"
#include "Process.h"
#include "Resource.h"


VOID FreeUserMemory(_In_ PPROCESS_CONTEXT Context)
{
    PEPROCESS Process = NULL;
    HANDLE  Handle = 0;
    PVOID DllFullPath = NULL;//�����ƶ�Ϊ0�����򷵻ز������� 

    __try {
        if (!Context) {
            __leave;
        }

        NTSTATUS status = PsLookupProcessByProcessId(Context->UniqueProcess, &Process);
        if (!NT_SUCCESS(status)) {
            Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "status:%#x, pid:%d", status, HandleToULong(Context->UniqueProcess));
            __leave;
        }

        status = ObOpenObjectByPointer(Process, OBJ_KERNEL_HANDLE, NULL, GENERIC_ALL, *PsProcessType, KernelMode, &Handle);
        if (!NT_SUCCESS(status)) {
            Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "status:%#x, pid:%d", status, HandleToULong(Context->UniqueProcess));
            __leave;
        }

        BOOL IsProcess64 = IsProcessPe64(Context->UniqueProcess);
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
        status = ZwFreeVirtualMemory(Handle, &Context->UserAddress, &size, MEM_DECOMMIT);
        if (!NT_SUCCESS(status)) {
            Print(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, "status:%#x, pid:%d, size:%lld",
                  status, HandleToULong(Context->UniqueProcess), size);
            __leave;
        }
    } __finally {
        if (Process) {
            ObDereferenceObject(Process);
        }

        if (Handle) {
            ZwClose(Handle);
        }
    }
}


VOID ThreadNotifyRoutine(_In_ HANDLE ProcessId, _In_ HANDLE ThreadId, _In_ BOOLEAN Create)
/*
Ŀ�ģ�ע����߳̽���ʱ���ͷ������Ӧ�ò���ڴ档
*/
{
    PPROCESS_CONTEXT Context = GetProcessContext(ProcessId);
    if (!Context) {
        
        return;
    }

    if (Context->InjectThreadId != ThreadId) {
        return;
    }

    if (Create) {
        PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Info: ע����߳�%p����", ThreadId);

        PROCESS_CONTEXT Temp = {0};
        Temp.Pid = ProcessId;
        Temp.IsInjected = TRUE;
        UpdateProcessContext(&Temp);
    } else {
        PrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "Info: ע����߳�%p����", ThreadId);

        FreeUserMemory(Context);

        PROCESS_CONTEXT Temp = {0};
        Temp.Pid = ProcessId;
        Temp.UserAddress = NULL;
        UpdateProcessContext(&Temp);
    }
}
