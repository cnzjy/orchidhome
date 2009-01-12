/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：资源字符串单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OResStr.cpp,v 1.16 2006/06/07 00:51:09 zjy Exp $
* 备    注：
*******************************************************************************/

#include "OResStr.h"
#include "OCalInfoW.h"
#include "OUtils.h"

wchar_t* sSunAllwaysUp;
wchar_t* sSunAllwaysDown;
wchar_t* sSunError;
wchar_t* sUnknown;

wchar_t* sHigh;
wchar_t* sLow;
wchar_t* sCritical;
wchar_t* sCharging;
wchar_t* sNone;

wchar_t* sAlkaline;
wchar_t* sNiCd;
wchar_t* sNiMH;
wchar_t* sLiIon;
wchar_t* sLiPB;

wchar_t* sCalenderInfo;
wchar_t* sSunInfo;
wchar_t* sHolidayInfo;
wchar_t* sPowerInfo;
wchar_t* sPowerInfoBk;
wchar_t* sMemInfo;
wchar_t* sStorageInfo;
wchar_t* sAboutInfo;

wchar_t* sAction;
wchar_t* sFormatTime;

wchar_t* sFmtHoliDate;
wchar_t* sFmtHoliBirth;
wchar_t* sFmtHoliBirthL;
wchar_t* sFmtHoliLBirth;
wchar_t* sFmtHoliLDay;
wchar_t* sFmtHoliLDayL;
wchar_t* sFmtHoliLDate;
wchar_t* sFmtCalToday;
wchar_t* sFmtCalInfo;
wchar_t* sFmtCalDayCount;
wchar_t* sFmtMonthHoliDay;

wchar_t* sLtHoliday;
wchar_t* sLtBirthDay;
wchar_t* sLtRemDay;
wchar_t* sLfSolarDate;
wchar_t* sLfSolarShort;
wchar_t* sLfLunarDate;
wchar_t* sLfLunarShort;
wchar_t* sLfWeekDate;

wchar_t* sMsgExecConfirm;

struct TResStrInfo 
{
  wchar_t** pStr;
  uint id;
};

static TResStrInfo ResStrTable[] =
{
  {&sSunAllwaysUp, IDS_SUNALLWAYSUP},
  {&sSunAllwaysDown, IDS_SUNALLWAYSDOWN},
  {&sSunError, IDS_SUNERROR},
  {&sUnknown, IDS_UNKNOWN},

  {&sHigh, IDS_HIGH},
  {&sLow, IDS_LOW},
  {&sCritical, IDS_CRITICAL},
  {&sCharging, IDS_CHARGING},
  {&sNone, IDS_NONE},

  {&sAlkaline, IDS_ALKALINE},
  {&sNiCd, IDS_NICD},
  {&sNiMH, IDS_NIMH},
  {&sLiIon, IDS_LIION},
  {&sLiPB, IDS_LIPB},

  {&sCalenderInfo, IDS_CALENDER_INFO},
  {&sSunInfo, IDS_SUN_INFO},
  {&sHolidayInfo, IDS_HOLIDAY_INFO},
  {&sPowerInfo, IDS_POWER_INFO},
  {&sPowerInfoBk, IDS_POWER_INFO_BK},
  {&sMemInfo, IDS_MEM_INFO},
  {&sStorageInfo, IDS_STORAGE_INFO},
  {&sAboutInfo, IDS_ABOUT_INFO},

  {&sAction, IDS_ACTION},
  {&sFormatTime, IDS_FORMAT_TIME},

  {&sFmtHoliDate, IDS_FMTHOLI_DATE},
  {&sFmtHoliBirth, IDS_FMTHOLI_BIRTH},
  {&sFmtHoliBirthL, IDS_FMTHOLI_BIRTHL},
  {&sFmtHoliLBirth, IDS_FMTHOLI_LBIRTH},
  {&sFmtHoliLDay, IDS_FMTHOLI_DAY},
  {&sFmtHoliLDayL, IDS_FMTHOLI_DAYL},
  {&sFmtHoliLDate, IDS_FMTHOLI_LDATE},
  {&sFmtCalToday, IDS_FMTCAL_TODAY},
  {&sFmtCalInfo, IDS_FMTCAL_INFO},
  {&sFmtCalDayCount, IDS_FMTCAL_DAYCOUNT},
  {&sFmtMonthHoliDay, IDS_FMTCAL_MONTHHOLIDAY},

  {&sLtHoliday, IDS_LT_HOLIDAY},
  {&sLtBirthDay, IDS_LT_BIRTHDAY},
  {&sLtRemDay, IDS_LT_REMDAY},
  {&sLfSolarDate, IDS_LF_SOLARDATE},
  {&sLfSolarShort, IDS_LF_SOLARSHORT},
  {&sLfLunarDate, IDS_LF_LUNARDATE},
  {&sLfLunarShort, IDS_LF_LUNARSHORT},
  {&sLfWeekDate, IDS_LF_WEEKDATE},

  {&sMsgExecConfirm, IDS_MSG_EXECCONFIRM}
};

void InitStringTable(HINSTANCE hInst)
{
  CalInitStrings(hInst);

  for (int i = 0; i < LENOF(ResStrTable); i++)
  {
    wchar_t buff[1024];
    LoadString(hInst, ResStrTable[i].id, buff, sizeof(buff));
    CloneString(*ResStrTable[i].pStr, buff);
  }
}

void FreeStringTable()
{
  for (int i = 0; i < LENOF(ResStrTable); i++)
  {
    FreeString(*ResStrTable[i].pStr);
  }
}