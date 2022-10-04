#pragma once

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct _PROCESS_CONTEXT {
    LIST_ENTRY  ListEntry;

    HANDLE Pid;//�����е�Ψһ��ʶ����KEY��

    ULONG Tids;//�̵߳ĸ�����

    HANDLE MainThreadId;
    BOOLEAN IsCanInject;//��ע���𣿼���Ӧ��kernel32.dll��������
    BOOLEAN IsInjected;//�Ѿ��ɹ�ע������
    HANDLE InjectThreadId;//ע����̵߳�ID�������߳��˳�ʱ���ͷ�Ӧ�ò���ڴ档

} PROCESS_CONTEXT, * PPROCESS_CONTEXT;


//////////////////////////////////////////////////////////////////////////////////////////////////


VOID ProcessNotifyRoutine(_In_ HANDLE ParentId, _In_ HANDLE ProcessId, _In_ BOOLEAN Create);

void InitProcessContextList();
VOID RemoveProcessContextList();
PPROCESS_CONTEXT GetProcessContext(HANDLE Pid);
VOID UpdateProcessContext(PPROCESS_CONTEXT ProcessContext);
