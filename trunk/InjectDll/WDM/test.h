/*
���ܣ�����������APCע��һ�δ��뵽ĳ�����������С�

���������Ǻܼ򵥾�һ���������磺NtQueueApcThread�������������û�е����������Զ�̬��ȡ�����һ��е�����KeInsertQueueApc֮��ĺ���ʵ�����ƵĹ��ܡ�

���̣�Ҫ�ų�IDLE��SYSTEM��smss.exe���Ҳ�ų�������ֻ��NTDLL.DLL�ĺ��������Ǽ�������DLLҲ���Եġ�

����X64��X64�е�WOW64��Ҫ�����Ľ�/�޸ġ�
��Ȼ��ע��DLL����һ��ע��ɹ��ˣ��Ժ���ע��ĺ������֪���ġ�

�����ˣ���ʱ���л�δ֪�������Ҫ���룬������Ҫ�����ӣ�������ʮ���ӣ�����ļ���Сʱ���߼��켸ҹ��������Զ�������У��磺�����������̵߳�״̬�����ϡ�

���Ƶİ취���У�
1.IMAGE�ص���
2.KernelCallbackTable = apfnDispatch
3.�Լ��ֶ������̣߳�Ҫ����X86��X64/WOW64�ȡ�

APCӦ����һ������ȡ���Ĺ��ܣ���Ϊ��
1.���ò��У�QueueUserAPC��
2.�ں˵�DPC�������ǹ����ˣ��磺KeInitializeDpc/KeInsertQueueDpc/KeRemoveQueueDpc��

�����ǲ��Դ��룬���ܾ����淶��ȥд�����ǻ���һЩBUG����ж�صȡ�

�ο���
1.WRK
2.http://www.microsoft.com/msj/0799/nerd/nerd0799.aspx
3.http://www.rohitab.com/discuss/topic/40737-inject-dll-from-kernel-mode/
����һ�ݶ���ģ�д��Ҳ����

made by correy
made at 2015.12.25
http://correy.webs.com
*/

#include "pch.h"

typedef
int (NTAPI * MessageBoxT)(
    HWND hWnd,
    LPCTSTR lpText,
    LPCTSTR lpCaption,
    UINT uType
    );

#pragma warning(disable:4700)//ʹ����δ��ʼ���ľֲ�������UserRoutine��


/*
ժ�ԣ�http://msdn.microsoft.com/en-us/library/windows/desktop/aa813708(v=vs.85).aspx
*/
typedef struct _LDR_DATA_TABLE_ENTRY {
    PVOID Reserved1[2];
    LIST_ENTRY InMemoryOrderLinks;
    PVOID Reserved2[2];
    PVOID DllBase;
    PVOID EntryPoint;
    PVOID Reserved3;
    UNICODE_STRING FullDllName;
    BYTE Reserved4[8];
    PVOID Reserved5[3];
    union {
        ULONG CheckSum;
        PVOID Reserved6;
    };
    ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;


/*
ժ�ԣ�\wrk\WindowsResearchKernel-WRK\WRK-v1.2\base\ntos\inc\ps.h
�˺�����XP 32�Ͼ��Ѿ�������Ӧ�ÿ��Է���ʹ�á�
����ZwQueryInformationProcess �� ProcessBasicInformation.
*/
NTKERNELAPI
PPEB
PsGetProcessPeb(
    __in PEPROCESS Process
);



//ժ�ԣ�http://msdn.microsoft.com/en-us/library/windows/desktop/aa813708(v=vs.85).aspx
typedef struct _PEB_LDR_DATA {
    BYTE       Reserved1[8];
    PVOID      Reserved2[3];
    LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;


//http://msdn.microsoft.com/en-us/library/windows/desktop/aa813741(v=vs.85).aspx
typedef struct _RTL_USER_PROCESS_PARAMETERS {
    BYTE           Reserved1[16];
    PVOID          Reserved2[10];
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;


//ժ�ԣ�Winternl.h��
typedef
VOID
(NTAPI * PPS_POST_PROCESS_INIT_ROUTINE) (
    VOID
    );


#ifdef _X86_
typedef struct _PEB {
    BYTE                          Reserved1[2];
    BYTE                          BeingDebugged;
    BYTE                          Reserved2[1];
    PVOID                         Reserved3[2];
    PPEB_LDR_DATA                 Ldr;
    PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
    BYTE                          Reserved4[104];
    PVOID                         Reserved5[52];
    PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
    BYTE                          Reserved6[128];
    PVOID                         Reserved7[1];
    ULONG                         SessionId;
} PEB, * PPEB;
#endif
//���µĽṹ���壬ժ�ԣ�http://msdn.microsoft.com/en-us/library/windows/desktop/aa813706(v=vs.85).aspx
#if defined(_WIN64)
typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[21];
    PPEB_LDR_DATA Ldr;//LoaderData;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    BYTE Reserved3[520];
    PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
    BYTE Reserved4[136];
    ULONG SessionId;
} PEB;
#endif 


//////////////////////////////////////////////////////////////////////////////////////////////////


typedef
HMODULE(WINAPI * LoadLibraryT)(
    __in          LPCTSTR lpFileName
    );

typedef NTSTATUS(WINAPI * ZwFreeVirtualMemoryT)(
    __in HANDLE  ProcessHandle,
    __inout PVOID * BaseAddress,
    __inout PSIZE_T  RegionSize,
    __in ULONG  FreeType
    );

typedef struct _PassToUser {
    PVOID ntdll;
    PVOID kernel32;
    PVOID KernelBase;

    LoadLibraryT LoadLibraryW;
    PVOID GetProcAddress;
    PVOID LdrLoadDll;
    ZwFreeVirtualMemoryT NtFreeVirtualMemory;

    wchar_t FullDllPathName[MAX_PATH];
    wchar_t OtherData[MAX_PATH];

    SIZE_T RegionSize;

    BOOLEAN done;
} PassToUser, * PPassToUser;
