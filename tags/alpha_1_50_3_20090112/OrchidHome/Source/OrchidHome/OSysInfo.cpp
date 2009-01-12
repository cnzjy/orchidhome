/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：系统信息单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OSysInfo.cpp,v 1.39 2006/06/05 14:21:36 zjy Exp $
* 备    注：该单元的部分内容参考了 CeleTask 1.7 的代码
*******************************************************************************/

#include <aygshell.h>
#include <tpcshell.h>
#include "OSysInfo.h"

/************************************************************************/
/* 信息显示类                                                           */
/************************************************************************/

CSysInfo::CSysInfo()
{

}

void CSysInfo::SecondToTimeStr(int sec, wchar_t* text)
{
  if (sec > 0)
  {
    int d, h, m;
    d = sec / (3600 * 24);
    h = sec / 3600 % 24;
    m = sec / 60 % 60;
    wsprintf(text, sFormatTime, d, h, m);
  }
  else
    wcscpy(text, sUnknown);
}

// 获取电池标记
wchar_t* CSysInfo::GetBatteryFlag(BYTE bFlag)
{
  if (bFlag & BATTERY_FLAG_NO_BATTERY)
    return sNone;
  else if (bFlag & BATTERY_FLAG_CHARGING)
    return sCharging;
  else if (bFlag & BATTERY_FLAG_HIGH)
    return sHigh;
  else if (bFlag & BATTERY_FLAG_LOW)
    return sLow;
  else if (bFlag & BATTERY_FLAG_CRITICAL)
    return sCritical;
  else
    return sUnknown;
}

// 获取电池性质
wchar_t* CSysInfo::GetBatteryChemistry(DWORD dwChemistry)
{
  switch (dwChemistry)
  {
    case BATTERY_CHEMISTRY_ALKALINE: 
      return sAlkaline;
    case BATTERY_CHEMISTRY_NICD: 
      return sNiCd;
    case BATTERY_CHEMISTRY_NIMH: 
      return sNiMH;
    case BATTERY_CHEMISTRY_LION: 
      return sLiIon;
    case BATTERY_CHEMISTRY_LIPOLY: 
      return sLiPB;
    default: 
      return sUnknown;
  }
}

void CSysInfo::GetDlgTitle(wchar_t* title, int slen, uint resid)
{
  if (resid)
  {
    wchar_t buff[TEXT_LEN];
    LoadString(hRes, resid, buff, TEXT_LEN - 1);
    wsprintf(title, L"%s - %s", APPNAME1, buff);
  }
  else
  {
    wcsncpy(title, APPNAME, slen - 1);
  }
}

void CSysInfo::ShowAbout()
{
  wchar_t buff[1024], title[TITLE_LEN];

  wsprintf(buff, sAboutInfo, APPNAME, VERSION, COPYRIGHT, WEBSITE, EMAIL, BUILDSTAMP);

  GetDlgTitle(title, TITLE_LEN, IDS_CAP_ABOUT);
  MessageBox(NULL, buff, title, MB_OK);
}

void CSysInfo::ShowCalendarInfo()
{
  wchar_t buff[1024], title[TITLE_LEN], *p;
  OCALENDAR_INFO info;;

  HomeData->CheckData(true);
  info = HomeData->m_calinfo;

  // 日历信息
  p = buff + wsprintf(buff, sCalenderInfo,
    info.date.year, info.date.month, info.date.day, info.sWeek, info.sXingZuo,
    info.sShengXiao, info.sMonth, info.sDay, info.sJieQi,
    info.sgzYear, info.sgzMonth, info.sgzDay, info.sgzHour
    );

  // 日出日落信息
  if (HomeData->geo.latitude != 0 && HomeData->geo.longitude != 0)
  {
    SYSTEMTIME stime;
    SUNTYPE suntype;
    SUNTIME suntime;
    wchar_t slight[NAME_LEN], srise[NAME_LEN], stransit[NAME_LEN], sset[NAME_LEN], sdark[NAME_LEN];

    GetLocalTime(&stime);
    suntype = GetSunRiseSetTime(stime.wYear, stime.wMonth, stime.wDay, 
      HomeData->geo.longitude, HomeData->geo.latitude, HomeData->geo.zonetime, &suntime);
    HomeData->GetSunTimeStr(suntype, &suntime.lighttime, slight);
    HomeData->GetSunTimeStr(suntype, &suntime.risetime, srise);
    HomeData->GetSunTimeStr(suntype, &suntime.transittime, stransit);
    HomeData->GetSunTimeStr(suntype, &suntime.settime, sset);
    HomeData->GetSunTimeStr(suntype, &suntime.darktime, sdark);

    wsprintf(p, sSunInfo,
      HomeData->geo.longitude, HomeData->geo.latitude, 
      srise, sset, slight, sdark, stransit
      );
  }

  GetDlgTitle(title, TITLE_LEN, IDS_CAP_CALENDER);
  MessageBox(NULL, buff, title, MB_OK);
}

void CSysInfo::ShowHolidayInfo()
{
  wchar_t buff[1024], tmp[256], title[TITLE_LEN];
  wchar_t stoday[512], stomorrow[512], snear[512];
  CHolidayItems* items = HomeData->holidayitems;
  int days = HomeData->alldays;

  // 日历信息
  stoday[0] = 0;
  snear[0] = 0;
  stomorrow[0] = 0;
  for (int i = 0; i < items->GetCount(); i++)
  {
    OHOLIDAY_ITEM* item = (OHOLIDAY_ITEM*)items->Get(i);
    if (item->days == days)
    {
      HomeData->GetHolidayStr(item, false, true, tmp, 256);
      STRNCAT(stoday, tmp);
      STRNCAT(stoday, L"\n");
    }
    else if (item->days == days + 1)
    {
      HomeData->GetHolidayStr(item, false, true, tmp, 256);
      STRNCAT(stomorrow, tmp);
      STRNCAT(stomorrow, L"\n");
    }
    else if (item->days - days < 30)
    {
      HomeData->GetHolidayStr(item, false, true, tmp, 256);
      STRNCAT(snear, tmp);
      STRNCAT(snear, L"\n");
    }
  }

  if (!stoday[0]) wsprintf(stoday, L"%s\n", sNone);
  if (!stomorrow[0]) wsprintf(stomorrow, L"%s\n", sNone);
  if (!snear[0]) wsprintf(snear, L"%s\n", sNone);

  wsprintf(buff, sHolidayInfo, stoday, stomorrow, snear);

  GetDlgTitle(title, TITLE_LEN, IDS_CAP_HOLIDAY);
  MessageBox(NULL, buff, title, MB_OK);
}

void CSysInfo::ShowPowerInfo()
{
  wchar_t buff[1024], title[TITLE_LEN], *p;
  wchar_t s1[64], s2[64];
  SYSTEM_POWER_STATUS_EX2 info;

  HomeData->CheckData(true);
  info = HomeData->m_power;

  SecondToTimeStr(info.BatteryLifeTime, s1);
  SecondToTimeStr(info.BatteryFullLifeTime, s2);

  // 主电池
  p = buff + wsprintf(buff, sPowerInfo,
    GetBatteryFlag(info.BatteryFlag),
    info.BatteryLifePercent, 
    info.BatteryTemperature,
    info.BatteryVoltage,
    info.BatteryCurrent, 
    info.BatteryAverageCurrent,
    s1, s2,
    info.BatterymAHourConsumed,
    GetBatteryChemistry(info.BatteryChemistry)
    );

  // 备用电池
  if (info.BackupBatteryFlag && (info.BackupBatteryFlag & BATTERY_FLAG_NO_BATTERY) == 0)
  {
    SecondToTimeStr(info.BackupBatteryLifeTime, s1);
    SecondToTimeStr(info.BackupBatteryFullLifeTime, s2);

    wsprintf(p, sPowerInfoBk,
      GetBatteryFlag(info.BackupBatteryFlag),
      info.BackupBatteryLifePercent,
      info.BackupBatteryVoltage,
      s1, s2
      );
  }

  GetDlgTitle(title, TITLE_LEN, IDS_CAP_POWER);
  MessageBox(NULL, buff, title, MB_OK);
}

void CSysInfo::ShowMemoryInfo()
{
  wchar_t buff[1024], title[TITLE_LEN], *p;
  WIN32_FIND_DATA fdFind;
  HANDLE hFind;

  HomeData->CheckData(true);

  // 内存信息
  p = buff + wsprintf(buff, sMemInfo,
    (double)HomeData->m_memstat.dwTotalPhys / (1024 * 1024),
    (double)HomeData->m_memstat.dwAvailPhys / (1024 * 1024),
    100 - HomeData->m_memstat.dwMemoryLoad,
    (double)HomeData->m_store.dwStoreSize / (1024 * 1024),
    (double)HomeData->m_store.dwFreeSize / (1024 * 1024),
    100 - HomeData->m_storeload
    );

  // 闪存卡
  hFind = FindFirstFlashCard(&fdFind);
  if (hFind != INVALID_HANDLE_VALUE)
  {
    do
    {
      if (fdFind.cFileName[0])
      {
        wchar_t tzPath[MAX_PATH];
        STORAGE_INFO info;
        wsprintf(tzPath, TEXT("\\%s"), fdFind.cFileName);
        if (GetStorageInfo(tzPath, &info))
        {
          p += wsprintf(p, sStorageInfo,
            fdFind.cFileName,
            (double) info.total / 1024,
            (double) info.free / 1024,
            100 - info.load
            );
        }
      }
    }
    while (FindNextFlashCard(hFind, &fdFind));

    FindClose(hFind);
  }

  GetDlgTitle(title, TITLE_LEN, IDS_CAP_MEM);
  MessageBox(NULL, buff, title, MB_OK);
}

void CSysInfo::ShowMain()
{
  ShowMainDlg();
}

void CSysInfo::ShowCalendar()
{
  ShowMainDlg(PAGE_CALENDAR);
}

void CSysInfo::ShowAlarms()
{
  ShowMainDlg(PAGE_ALARM);
}

void CSysInfo::ShowUserHoliday()
{
  ShowMainDlg(PAGE_USERDAY);
}

void CSysInfo::ShowTools()
{
  ShowMainDlg(PAGE_TOOLS);
}

void CSysInfo::ShowOption()
{
  DialogBox(hRes, MAKEINTRESOURCE(IDD_OPTIONDLG), NULL, Option_DlgProc);
}
