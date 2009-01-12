/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：资源字符串头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OResStr.h,v 1.14 2006/06/07 00:51:09 zjy Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <windows.h>
#include "resource.h"

// 字符串常量
extern wchar_t* sSunAllwaysUp;
extern wchar_t* sSunAllwaysDown;
extern wchar_t* sSunError;
extern wchar_t* sUnknown;

extern wchar_t* sHigh;
extern wchar_t* sLow;
extern wchar_t* sCritical;
extern wchar_t* sCharging;
extern wchar_t* sNone;

extern wchar_t* sAlkaline;
extern wchar_t* sNiCd;
extern wchar_t* sNiMH;
extern wchar_t* sLiIon;
extern wchar_t* sLiPB;

extern wchar_t* sCalenderInfo;
extern wchar_t* sSunInfo;
extern wchar_t* sHolidayInfo;
extern wchar_t* sPowerInfo;
extern wchar_t* sPowerInfoBk;
extern wchar_t* sMemInfo;
extern wchar_t* sStorageInfo;
extern wchar_t* sAboutInfo;

extern wchar_t* sAction;
extern wchar_t* sFormatTime;

extern wchar_t* sFmtHoliDate;
extern wchar_t* sFmtHoliBirth;
extern wchar_t* sFmtHoliBirthL;
extern wchar_t* sFmtHoliLBirth;
extern wchar_t* sFmtHoliLDay;
extern wchar_t* sFmtHoliLDayL;
extern wchar_t* sFmtHoliLDate;
extern wchar_t* sFmtCalToday;
extern wchar_t* sFmtCalInfo;
extern wchar_t* sFmtCalDayCount;
extern wchar_t* sFmtMonthHoliDay;

extern wchar_t* sLtHoliday;
extern wchar_t* sLtBirthDay;
extern wchar_t* sLtRemDay;
extern wchar_t* sLfSolarDate;
extern wchar_t* sLfSolarShort;
extern wchar_t* sLfLunarDate;
extern wchar_t* sLfLunarShort;
extern wchar_t* sLfWeekDate;

extern wchar_t* sMsgExecConfirm;

void InitStringTable(HINSTANCE hInst);
void FreeStringTable();
