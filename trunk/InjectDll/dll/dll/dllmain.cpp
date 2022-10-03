// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"


void DebugPrintA(char * format, ...)
//OutputDebugStringA �֧�� 65534��MAXUINT16 - 1�� ���ַ������(������β�� L'\0').
{
    size_t len = MAXUINT16;

    char * out = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, len);
    _ASSERTE(NULL != out);

    va_list marker;

    va_start(marker, format);
    StringCbVPrintfA(out, len, format, marker);//STRSAFE_MAX_CCH
    va_end(marker);

    OutputDebugStringA(out);
    HeapFree(GetProcessHeap(), 0, out);
}


#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)


#define DbgPrintA(Format, ...) \
{DebugPrintA("FILE:%s, LINE:%d, "##Format".\r\n", __FILENAME__, __LINE__, __VA_ARGS__);}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        //DebugBreak();
        DbgPrintA("��Ϣ����dll��ע�뵽pid:%d", GetCurrentProcessId());
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}
