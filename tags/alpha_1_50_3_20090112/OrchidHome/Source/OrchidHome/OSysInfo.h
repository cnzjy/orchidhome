/*******************************************************************************
* ������ƣ�Orchid Mobile Software Home Plugin
* �ļ����ƣ�ϵͳ��Ϣͷ�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OSysInfo.h,v 1.14 2006/06/05 13:45:04 zjy Exp $
* ��    ע��
*******************************************************************************/

#pragma once

#include "OHomeData.h"
#include "OHomeDlg.h"

#define STRNCAT(buff, s) wcsncat(buff, s, sizeof(buff) / sizeof(wchar_t) - wcslen(buff) - 1)

/************************************************************************/
/* ��Ϣ��ʾ��                                                           */
/************************************************************************/

class CSysInfo
{

public:
  CSysInfo();

  void GetDlgTitle(wchar_t* title, int slen, uint resid);

  void ShowMain();
  void ShowCalendar();
  void ShowAlarms();
  void ShowUserHoliday();
  void ShowTools();

  void ShowCalendarInfo();
  void ShowHolidayInfo();
  void ShowPowerInfo();
  void ShowMemoryInfo();

  void ShowOption();
  void ShowAbout();

private:
  void SecondToTimeStr(int sec, wchar_t* text);
  wchar_t* GetBatteryFlag(BYTE bFlag);
  wchar_t* GetBatteryChemistry(DWORD dwChemistry);
};
