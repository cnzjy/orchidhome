/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ���������
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OCalInfoW.c,v 1.11 2006/06/02 10:02:47 zjy Exp $
* ��    ע��
*******************************************************************************/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include "OCalInfoW.h"
#include "resource.h"

/* ��� */
wchar_t sGan[10][2];

/* ��֧ */
wchar_t sZhi[12][2];

/* ��Ф */
wchar_t sShengXiao[12][2];

wchar_t sXingZuo[12][3];
wchar_t sZuo[STR_LEN];

/* ���� */
wchar_t sJieQi[24][3];

/* ����ַ��� */
wchar_t sGuangXu[STR_LEN];
wchar_t sXuanTong[STR_LEN];
wchar_t sMinGuo[STR_LEN];
wchar_t sGongYuan[STR_LEN];
wchar_t sYuan[STR_LEN];
wchar_t sNian[STR_LEN];

/* �·��ַ��� */
wchar_t sLeapMonth[STR_LEN];
wchar_t sMonthName[12][3];
wchar_t sMonthStr[STR_LEN];
wchar_t sBigMonth[STR_LEN];
wchar_t sLittleMonth[STR_LEN];

/* �����ַ��� */
wchar_t nStr1[11][2];
wchar_t nStr2[4][2];

/* �����ַ��� */
wchar_t sWeekName[7][2];
wchar_t sWeekStr[STR_LEN];

#define LOADSTR(s, id) LoadString(hInst, id, s, sizeof(s) - 1);

/* ��ʼ���ַ����� */
void CalInitStrings(HINSTANCE hInst)
{
  int i;

  for (i = 0; i < 10; i++)
    LOADSTR(sGan[i], IDS_GAN01 + i);

  for (i = 0; i < 12; i++)
  {
    LOADSTR(sZhi[i], IDS_ZHI01 + i);
    LOADSTR(sShengXiao[i], IDS_SHENGXIAO01 + i);
    LOADSTR(sXingZuo[i], IDS_XINGZUO01 + i);
    LOADSTR(sMonthName[i], IDS_NUM01 + i);
  }

  for (i = 0; i < 24; i++)
    LOADSTR(sJieQi[i], IDS_JIEQI01 + i);

  for (i = 0; i < 11; i++)
    LOADSTR(nStr1[i], IDS_NUM00 + i);

  for (i = 0; i < 4; i++)
    LOADSTR(nStr2[i], IDS_DAY0 + i);

  for (i = 0; i < 7; i++)
    LOADSTR(sWeekName[i], IDS_NUM00 + i);

  LOADSTR(sZuo, IDS_ZUO);
  LOADSTR(sGuangXu, IDS_GUANGXU);
  LOADSTR(sXuanTong, IDS_XUANTONG);
  LOADSTR(sMinGuo, IDS_MINGUO);
  LOADSTR(sGongYuan, IDS_GONGYUAN);
  LOADSTR(sYuan, IDS_YUAN);
  LOADSTR(sNian, IDS_NIAN);
  LOADSTR(sLeapMonth, IDS_LEAPMONTH);
  LOADSTR(sMonthStr, IDS_MONTH);
  LOADSTR(sBigMonth, IDS_BIGMONTH);
  LOADSTR(sLittleMonth, IDS_LITTLEMONTH);
  LOADSTR(sWeekStr, IDS_WEEK);
}

/* ���� offset ���ظ�֧, 0=���� */
static void cyclical(int num, wchar_t* sz)
{
  wcscpy(sz, sGan[num % 10]);
  wcscat(sz, sZhi[num % 12]);
}

/* ����ũ������ȡ�ַ��� */
void GetLunarDayStr(int day, wchar_t* sDay)
{
  if (day == 10)
    wsprintf(sDay, L"%s%s", nStr2[0], nStr1[10]);
  else if (day == 20)
    wsprintf(sDay, L"%s%s", nStr2[2], nStr1[10]);
  else if (day == 30)
    wsprintf(sDay, L"%s%s", nStr1[3], nStr1[10]);
  else
    wsprintf(sDay, L"%s%s", nStr2[day / 10], nStr1[day % 10]);
}

/* ����������Ϣ */
int GetCalendarInfo(ODATE *date, OTIME *time, OCALENDAR_INFO *info)
{
  memset(info, 0, sizeof(OCALENDAR_INFO));
  if (!GetDateIsValid(date->year, date->month, date->day))
    return FALSE;

  /* ��ʼ������ */
  info->date = *date;
  info->time = *time;
  info->LeapYear = GetIsLeapYear(date->year);
  info->Week = GetWeek(date->year, date->month, date->day);

  /* ������� */
  info->JieQi = GetJieQiFromDay(date->year, date->month, date->day);

  /* �������� */
  info->XingZuo = GetXingZuoFromMonthDay(date->month, date->day);

  /* ������Ф */
  info->ShengXiao = GetShengXiaoFromYear(date->year);

  /* �����֧ */
  info->gzYear = GetGanZhiFromYear(date->year, date->month, date->day);
  info->gzMonth = GetGanZhiFromMonth(date->year, date->month, date->day);
  info->gzDay = GetGanZhiFromDay(date->year, date->month, date->day);
  info->gzHour = GetGanZhiFromHour(date->year, date->month, date->day, time->hour);

  /* ����ũ�� */
  if (GetLunarInfo(date, &info->lunar))
  {
    /* ������Ф */
    info->ShengXiao = GetShengXiaoFromYear(info->lunar.year);

    /* ũ����� */
    if (info->lunar.year == 1875)
      wsprintf(info->sYear, L"%s%s", sGuangXu, sYuan);
    else if (info->lunar.year > 1875 && info->lunar.year < 1909)
      wsprintf(info->sYear, L"%s%d", sGuangXu, info->lunar.year - 1874);
    else if (info->lunar.year == 1909)
      wsprintf(info->sYear, L"%s%s", sXuanTong, sYuan);
    else if (info->lunar.year > 1909 && info->lunar.year < 1912)
      wsprintf(info->sYear, L"%s%d", sXuanTong, info->lunar.year - 1908);
    else if (info->lunar.year == 1912)
      wsprintf(info->sYear, L"%s%s", sMinGuo, sYuan);
    else if (info->lunar.year > 1912 && info->lunar.year < 1949)
      wsprintf(info->sYear, L"%s%d", sMinGuo, info->lunar.year - 1911);
    else if (info->lunar.year == 0)
      wsprintf(info->sYear, L"%s%s", sGongYuan, sYuan);
    else
      wsprintf(info->sYear, L"%s%d", sGongYuan, info->lunar.year);
    wcscat(info->sYear, sNian);

    /* ũ���·� */
    if (info->lunar.leap)
      wcscpy(info->sLeapMonth, sLeapMonth);
    else
      info->sLeapMonth[0] = 0;
    wcscpy(info->sShortMonth, sMonthName[info->lunar.month - 1]);
    wcscpy(info->sMonthLen, info->lunar.little ? sLittleMonth : sBigMonth);
    wsprintf(info->sMonth, L"%s%s%s%s", info->sLeapMonth, info->sShortMonth, sMonthStr, info->sMonthLen);

    /* ũ������ */
    GetLunarDayStr(info->lunar.day, info->sDay);
  }

  /* �����ַ��� */
  wcscpy(info->sWeek, sWeekName[info->Week]);

  /* �����ַ��� */
  if (info->JieQi >= 0)
    wcscpy(info->sJieQi, sJieQi[info->JieQi - 1]);

  /* ���� */
  wcscpy(info->sXingZuo, sXingZuo[info->XingZuo]);

  /* ��Ф */
  wcscpy(info->sShengXiao, sShengXiao[info->ShengXiao]);

  /* ��֧�ַ��� */
  cyclical(info->gzYear, info->sgzYear);
  cyclical(info->gzMonth, info->sgzMonth);
  cyclical(info->gzDay, info->sgzDay);
  cyclical(info->gzHour, info->sgzHour);

  return TRUE;
}
