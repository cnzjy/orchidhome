/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：调试日志文件
* 文件作者：OrchidSoft (master@orchid-soft.com)
* 单元标识：$Id: ODebug.cpp,v 1.2 2008/02/29 06:22:36 Administrator Exp $
* 备    注：
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
