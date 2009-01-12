/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：调试日志文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: ODebug.cpp,v 1.2 2006/09/22 15:00:54 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "ODebug.h"

#ifdef DEBUG

static TCHAR g_szBuffer[2000];
void DebugOut(const TCHAR* szFormat, ...)
{
  va_list vaMarker;
  FILE* f;

  va_start(vaMarker, szFormat);
  wvsprintf(g_szBuffer, szFormat, vaMarker);
  va_end(vaMarker);

  f = fopen("\\Storage Card\\log.txt", "a+");
  fwprintf(f, L"%d %x %x %s", GetTickCount(), GetCurrentProcessId(), GetCurrentThreadId(), g_szBuffer);
  fwprintf(f, L"\n");
  fclose(f);

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
