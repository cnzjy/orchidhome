/*******************************************************************************
* ������ƣ�Orchid Mobile Software Home Plugin
* �ļ����ƣ�������־�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: ODebug.h,v 1.3 2008/01/18 03:13:19 Administrator Exp $
* ��    ע��
*******************************************************************************/

#pragma once

#include <windows.h>

//#define DEBUG
//#define DEBUG_SHORT

#ifdef DEBUG

#define DEBUGLOG(a)          DebugOut(a)
#define DEBUGLOG1(a,b)       DebugOut(a,b)
#define DEBUGLOG2(a,b,c)     DebugOut(a,b,c)
#define DEBUGLOG3(a,b,c,d)   DebugOut(a,b,c,d)

#define STARTTIME()          StartTime()
#define LOGTIME(a)           LogTime(a)

void DebugOut(const TCHAR* szFormat, ...);
void StartTime();
void LogTime(const TCHAR* a);

#else

#define DEBUGLOG(a)
#define DEBUGLOG1(a,b)
#define DEBUGLOG2(a,b,c)
#define DEBUGLOG3(a,b,c,d)

#define STARTTIME()
#define LOGTIME(a)

#endif

#ifndef DEBUG_SHORT
#define DLOG(a)
#define DLOG(a)         
#define DLOG1(a,b)      
#define DLOG2(a,b,c)    
#define DLOG3(a,b,c,d)  
#else
#define DLOG(a)          DEBUGLOG(a)
#define DLOG1(a,b)       DEBUGLOG1(a,b)
#define DLOG2(a,b,c)     DEBUGLOG2(a,b,c)
#define DLOG3(a,b,c,d)   DEBUGLOG3(a,b,c,d)
#endif