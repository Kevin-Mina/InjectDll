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
    HANDLE UniqueProcess;

    PVOID UserAddress;

} PROCESS_CONTEXT, * PPROCESS_CONTEXT;


//////////////////////////////////////////////////////////////////////////////////////////////////


extern PAGED_LOOKASIDE_LIST g_ProcessContextLookasideList;


VOID ProcessNotifyRoutine(_In_ HANDLE ParentId, _In_ HANDLE ProcessId, _In_ BOOLEAN Create);

void InitProcessContextList();
VOID RemoveProcessContextList();
PPROCESS_CONTEXT GetProcessContext(HANDLE Pid);
BOOL UpdateProcessContext(PPROCESS_CONTEXT ProcessContext);
void InsertProcessContext(PPROCESS_CONTEXT Context);
