/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ���������
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OCalInfo.c,v 1.4 2006/06/02 10:02:28 zjy Exp $
* ��    ע��
*******************************************************************************/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "OCalInfo.h"

/* ��� */
static const char* sGan[10] = {"��","��","��","��","��","��","��","��","��","��"};

/* ��֧ */
static const char* sZhi[12] = {"��","��","��","î","��","��","��","δ","��","��","��","��"};

/* ��Ф */
static const char* sShengXiao[12] = {"��","ţ","��","��","��","��","��","��","��","��","��","��"};

static const char* sXingZuo[12] = {"����","��ţ","˫��","��з","ʨ��","��Ů","���","��Ы","����","Ħ��","��ƿ","˫��"};
static const char* sZuo = "��";

/* ���� */
static const char* sJieQi[24] = {"С��","��","����","��ˮ","����","����","����","����",
                                 "����","С��","â��","����","С��","����","����","��¶",
                                 "���","����","��¶","˪��","����","Сѩ","��ѩ","����"};

/* ����ַ��� */
static const char* sGuangXu = "����";
static const char* sXuanTong = "��ͳ";
static const char* sMinGuo = "���";
static const char* sGongYuan = "��Ԫ";
static const char* sYuan = "Ԫ";
static const char* sNian = "��";

/* �·��ַ��� */
static const char* sLeapMonth = "��";
static const char* sMonthName[12] = {"һ","��","��","��","��","��","��","��","��","ʮ","ʮһ","ʮ��"};
static const char* sMonth = "��";
static const char* sBigMonth = "��";
static const char* sLittleMonth = "С";

/* �����ַ��� */
static const char* nStr1[11] = {"��","һ","��","��","��","��","��","��","��","��","ʮ"};
static const char* nStr2[4] = {"��","ʮ","إ","ئ"};

/* �����ַ��� */
static const char* sWeek = "����";
static const char* sWeekName[7] = {"��","һ","��","��","��","��","��"};

/* ���� offset ���ظ�֧, 0=���� */
static void cyclical(int num, char* sz)
{
  strcpy(sz, sGan[num % 10]);
  strcat(sz, sZhi[num % 12]);
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
      sprintf(info->sYear, "%s%s", sGuangXu, sYuan);
    else if (info->lunar.year > 1875 && info->lunar.year < 1909)
      sprintf(info->sYear, "%s%d", sGuangXu, info->lunar.year - 1874);
    else if (info->lunar.year == 1909)
      sprintf(info->sYear, "%s%s", sXuanTong, sYuan);
    else if (info->lunar.year > 1909 && info->lunar.year < 1912)
      sprintf(info->sYear, "%s%d", sXuanTong, info->lunar.year - 1908);
    else if (info->lunar.year == 1912)
      sprintf(info->sYear, "%s%s", sMinGuo, sYuan);
    else if (info->lunar.year > 1912 && info->lunar.year < 1949)
      sprintf(info->sYear, "%s%d", sMinGuo, info->lunar.year - 1911);
    else if (info->lunar.year == 0)
      sprintf(info->sYear, "%s%s", sGongYuan, sYuan);
    else
      sprintf(info->sYear, "%s%d", sGongYuan, info->lunar.year);
    strcat(info->sYear, sNian);

    /* ũ���·� */
    if (info->lunar.leap)
      sprintf(info->sMonth, "%s%s", sLeapMonth, sMonthName[info->lunar.month - 1]);
    else
      strcpy(info->sMonth, sMonthName[info->lunar.month - 1]);
    strcat(info->sMonth, sMonth);
    strcat(info->sMonth, info->lunar.little ? sLittleMonth : sBigMonth);

    /* ũ������ */
    if (info->lunar.day == 10)
      sprintf(info->sDay, "%s%s", nStr2[0], nStr1[10]);
    else if (info->lunar.day == 20)
      sprintf(info->sDay, "%s%s", nStr2[2], nStr1[10]);
    else if (info->lunar.day == 30)
      sprintf(info->sDay, "%s%s", nStr1[3], nStr1[10]);
    else
      sprintf(info->sDay, "%s%s", nStr2[info->lunar.day / 10], nStr1[info->lunar.day % 10]);
  }

  /* �����ַ��� */
  sprintf(info->sWeek, "%s%s", sWeek, sWeekName[info->Week]);

  /* �����ַ��� */
  if (info->JieQi > 0)
    strcpy(info->sJieQi, sJieQi[info->JieQi - 1]);

  /* ���� */
  sprintf(info->sXingZuo, "%s%s", sXingZuo[info->XingZuo], sZuo);

  /* ��Ф */
  strcpy(info->sShengXiao, sShengXiao[info->ShengXiao]);

  /* ��֧�ַ��� */
  cyclical(info->gzYear, info->sgzYear);
  cyclical(info->gzMonth, info->sgzMonth);
  cyclical(info->gzDay, info->sgzDay);
  cyclical(info->gzHour, info->sgzHour);

  return TRUE;
}
