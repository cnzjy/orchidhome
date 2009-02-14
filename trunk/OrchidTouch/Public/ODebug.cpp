/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ�������־�ļ�
* �ļ����ߣ�OrchidSoft (master@orchid-soft.com)
* ��Ԫ��ʶ��$Id: ODebug.cpp,v 1.2 2008/02/29 06:22:36 Administrator Exp $
* ��    ע��
*******************************************************************************/

#include "ODebug.h"

#ifdef DEBUG

void DebugOut(const TCHAR* szFormat, ...)
{
    static TCHAR g_szBuffer[2000];
    static FILE* f = NULL;

    va_list vaMarker;

    va_start(vaMarker, szFormat);
    wvsprintf(g_szBuffer, szFormat, vaMarker);
    va_end(vaMarker);

    if (!f)
        f = fopen("\\Storage Card\\log.txt", "a+");
    fwprintf(f, L"[%d] %s\n", GetTickCount(), g_szBuffer);
#if 1
    fclose(f);
    f = NULL;
#endif

    OutputDebugString(g_szBuffer);
}

static int _tick;

void StartTime()
{
    _tick = GetTickCount();
}

void LogTime(const TCHAR* a)
{
    DebugOut(L"%s [%d]ms", a, GetTickCount() - _tick);
    _tick = GetTickCount();
}

#endif
