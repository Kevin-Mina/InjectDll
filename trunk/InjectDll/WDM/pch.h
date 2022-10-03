#pragma once

#include <ntifs.h>
#include <windef.h>
#include <ntddk.h>
#include <ntstrsafe.h>
#include <Aux_klib.h> 
#include "..\libdrv\inc\lib.h"

#define TAG  'tset' //test


#define RT_RCDATA 10


 //////////////////////////////////////////////////////////////////////////////////////////////////


/*
һ�¶���ժ�ԣ�
C:\Program Files (x86)\Windows Kits\8.0\Include\um\winternl.h����
C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include\winternl.h
�������Ϣ���ɿ���
http://undocumented.ntinternals.net/UserMode/Undocumented%20Functions/System%20Information/SYSTEM_INFORMATION_CLASS.html#SystemProcessInformation
http://doxygen.reactos.org/d2/d5c/ntddk__ex_8h_source.html
*/
typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemPerformanceInformation = 2,
    SystemTimeOfDayInformation = 3,
    SystemProcessInformation = 5,
    SystemProcessorPerformanceInformation = 8,
    SystemInterruptInformation = 23,
    SystemExceptionInformation = 33,
    SystemRegistryQuotaInformation = 37,
    SystemLookasideInformation = 45
} SYSTEM_INFORMATION_CLASS;


//ժ�ԣ�http://msdn.microsoft.com/en-us/library/gg750724.aspx ���WRKҲ�еġ�
typedef struct {
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    LONG Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    ULONG ThreadState;
    ULONG WaitReason;
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;


//ժ�ԣ�http://doxygen.reactos.org/de/d22/ndk_2extypes_8h_source.html�����޸ġ�
//�����https://chromium.googlesource.com/chromium/chromium/+/1a9d8d9f3355e8b9f35591c8a678940bd264f412/third_party/psutil/psutil/arch/mswindows/ntextapi.h
typedef struct _SYSTEM_PROCESS_INFORMATION
{
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize; //VISTA
    ULONG HardFaultCount; //WIN7
    ULONG NumberOfThreadsHighWatermark; //WIN7
    ULONGLONG CycleTime; //WIN7
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;//������ֺ��񲻳���15-16���ַ���
    KPRIORITY BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR PageDirectoryBase;

    //
    // This part corresponds to VM_COUNTERS_EX.
    // NOTE: *NOT* THE SAME AS VM_COUNTERS!
    //
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;

    //
    // This part corresponds to IO_COUNTERS
    //
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    SYSTEM_THREAD_INFORMATION TH[1];//���������ע�͵��ġ�
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;


//2008�汾��MSDN����WDK��
//http://msdn.microsoft.com/en-us/library/windows/desktop/ms687420(v=vs.85).aspx
//�����һЩ��ע�ڵͰ汾�ϵ�WDK����
NTSTATUS /* WINAPI */ ZwQueryInformationProcess(
    __in          HANDLE ProcessHandle,
    __in          PROCESSINFOCLASS ProcessInformationClass,
    __out         PVOID ProcessInformation,
    __in          ULONG ProcessInformationLength,
    __out_opt     PULONG ReturnLength
);


/*
ժ�ԣ�http://msdn.microsoft.com/en-us/library/windows/desktop/ms724509(v=vs.85).aspx��
���޸ġ�
*/
NTSTATUS /* WINAPI NtQuerySystemInformation */ ZwQuerySystemInformation(
    _In_       SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _Inout_    PVOID SystemInformation,
    _In_       ULONG SystemInformationLength,
    _Out_opt_  PULONG ReturnLength
);


typedef
NTSTATUS(NTAPI *
         RtlCreateUserThreadFn)(
             IN HANDLE Process,
             IN PSECURITY_DESCRIPTOR ThreadSecurityDescriptor OPTIONAL,
             IN BOOLEAN CreateSuspended,
             IN ULONG ZeroBits OPTIONAL,
             IN SIZE_T MaximumStackSize OPTIONAL,
             IN SIZE_T CommittedStackSize OPTIONAL,
             IN PUSER_THREAD_START_ROUTINE StartAddress,
             IN PVOID Parameter OPTIONAL,
             OUT PHANDLE Thread OPTIONAL,
             OUT PCLIENT_ID ClientId OPTIONAL
             );


//////////////////////////////////////////////////////////////////////////////////////////////////


#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAMEW__ (wcsrchr(_CRT_WIDE(__FILE__), L'\\') ? wcsrchr(_CRT_WIDE(__FILE__), L'\\') + 1 : _CRT_WIDE(__FILE__))

/*
��֧�ֵ��ַ�Ҳ֧�ֿ��ַ���
ע�⣺
1.�����������ǵ��ַ�������Ϊ�գ�����ҪΪNULL��������ʡ�ԡ�
2.������DPC�ϲ�Ҫ��ӡ���ַ���
3.
*/

//���֧��3����������
#define Print(ComponentId, Level, Format, ...) \
{DbgPrintEx(ComponentId, Level, "FILE:%s, LINE:%d, "##Format".\r\n", __FILENAME__, __LINE__, __VA_ARGS__);}

//�������4��������
#define PrintEx(ComponentId, Level, Format, ...) \
{KdPrintEx((ComponentId, Level, "FILE:%s, LINE:%d, "##Format".\r\n", __FILENAME__, __LINE__, __VA_ARGS__));}


//////////////////////////////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////////////////////////////
