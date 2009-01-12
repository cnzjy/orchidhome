/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ�������־�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: ODebug.cpp,v 1.2 2006/09/22 15:00:54 Administrator Exp $
* ��    ע��
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
