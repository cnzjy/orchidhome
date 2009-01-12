/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ���������
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OCalendar.c,v 1.13 2006/06/02 08:40:41 zjy Exp $
* ��    ע��
*******************************************************************************/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "OCalendar.h"

/* ũ����Ϣ 1900-2050 �� */
static const unsigned long lunarInfo[] = {
  0x04bd8,0x04ae0,0x0a570,0x054d5,0x0d260,0x0d950,0x16554,0x056a0,0x09ad0,0x055d2,
  0x04ae0,0x0a5b6,0x0a4d0,0x0d250,0x1d255,0x0b540,0x0d6a0,0x0ada2,0x095b0,0x14977,
  0x04970,0x0a4b0,0x0b4b5,0x06a50,0x06d40,0x1ab54,0x02b60,0x09570,0x052f2,0x04970,
  0x06566,0x0d4a0,0x0ea50,0x06e95,0x05ad0,0x02b60,0x186e3,0x092e0,0x1c8d7,0x0c950,
  0x0d4a0,0x1d8a6,0x0b550,0x056a0,0x1a5b4,0x025d0,0x092d0,0x0d2b2,0x0a950,0x0b557,
  0x06ca0,0x0b550,0x15355,0x04da0,0x0a5b0,0x14573,0x052b0,0x0a9a8,0x0e950,0x06aa0,
  0x0aea6,0x0ab50,0x04b60,0x0aae4,0x0a570,0x05260,0x0f263,0x0d950,0x05b57,0x056a0,
  0x096d0,0x04dd5,0x04ad0,0x0a4d0,0x0d4d4,0x0d250,0x0d558,0x0b540,0x0b6a0,0x195a6,
  0x095b0,0x049b0,0x0a974,0x0a4b0,0x0b27a,0x06a50,0x06d40,0x0af46,0x0ab60,0x09570,
  0x04af5,0x04970,0x064b0,0x074a3,0x0ea50,0x06b58,0x055c0,0x0ab60,0x096d5,0x092e0,
  0x0c960,0x0d954,0x0d4a0,0x0da50,0x07552,0x056a0,0x0abb7,0x025d0,0x092d0,0x0cab5,
  0x0a950,0x0b4a0,0x0baa4,0x0ad50,0x055d9,0x04ba0,0x0a5b0,0x15176,0x052b0,0x0a930,
  0x07954,0x06aa0,0x0ad50,0x05b52,0x04b60,0x0a6e6,0x0a4e0,0x0d260,0x0ea65,0x0d530,
  0x05aa0,0x076a3,0x096d0,0x04bd7,0x04ad0,0x0a4d0,0x1d0b6,0x0d250,0x0d520,0x0dd45,
  0x0b5a0,0x056d0,0x055b2,0x049b0,0x0a577,0x0a4b0,0x0aa50,0x1b255,0x06d20,0x0ada0,
  0x14b63};

/* ÿ���µ���������ƽ������꣩ */
static const int MonthDays[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

/* 1/1/0001 �� 12/31/1899 ������ */
#define DateDelta 693594


/************************************************************************/
/* ��������                                                             */
/************************************************************************/

/* ����ɵ�֧��ϳɸ�֧��0-9 0-11 ת���� 0-59 */
int CombineGanZhi(int Gan, int Zhi)
{
  int i;

  if ((Gan >= 0) && (Gan <= 9) && (Zhi >= 0) && (Zhi <= 11))
  {
    for (i = 0; i < 6; i++)
    {
      if ((i * 10 + Gan) % 12 == Zhi)
      {
        return i * 10 + Gan;
      }
    }
  }

  return -1;
}

/* ����֧��ֳ���ɵ�֧��0-59 ת���� 0-9 0-11 */
int ExtractGanZhi(int GanZhi, int *Gan, int *Zhi)
{
  if ((GanZhi >= 0) && (GanZhi <= 59))
  {
    *Gan = GanZhi % 10;
    *Zhi = GanZhi % 12;
    return TRUE;
  }
  else
  {
    *Gan = -1;
    *Zhi = -1;
    return FALSE;
  }
}

/* ���ݹ��������жϵ�ʱ���� */
CALENDER_TYPE GetCalendarType(int AYear, int AMonth, int ADay)
{
  if (AYear < 1582)
    return ctJulian;
  else if ((AYear == 1582) && (AMonth < 10))
    return ctJulian;
  else if ((AYear == 1582) && (AMonth == 10) && (ADay <= 4))
    return ctJulian;
  else if ((AYear == 1582) && (AMonth == 10) && (ADay >= 5) && (ADay <= 14))
    return ctInvalid;
  else
    return ctGregorian;
}

/* ����ĳ�����Ƿ����� */
int GetIsLeapYear(int AYear)
{
  if (GetCalendarType(AYear, 1, 1) == ctGregorian)
    return (AYear % 4 == 0) && ((AYear % 100 != 0) || (AYear % 400 == 0)) ? TRUE : FALSE;
  else if (AYear >= 0)
    return (AYear % 4 == 0) ? TRUE : FALSE;
  else /* ��Ҫ�����жϹ�Ԫǰ��ԭ����û�й�Ԫ 0 �� */
    return (AYear - 3) % 4 == 0 ? TRUE : FALSE;
}

/* ȡ���������������� 1582 �� 10 �µ�������� */
int GetMonthDays(int AYear, int AMonth)
{
  return MonthDays[GetIsLeapYear(AYear)][AMonth - 1];
}

/* ���ع��������Ƿ�Ϸ� */
int GetDateIsValid(int AYear, int AMonth, int ADay)
{
  int ret;
  ret = (AYear != 0) && (AMonth >= 1) && (AMonth <= 12) &&
    (ADay >= 1) && (ADay <= GetMonthDays(AYear, AMonth));
  if (ret && (AYear == 1582) && (AMonth == 10))
    ret = (ADay < 5) || (ADay > 14);
  return ret ? TRUE : FALSE;
}

/* ����ʱ���Ƿ�Ϸ� */
int GetTimeIsValid(int AHour, int AMinute, int ASecond)
{
  return (AHour >= 0) && (AHour <= 23) && (AMinute >= 0) && (AMinute <= 59) && 
    (ASecond >= 0) && (ASecond <= 59) ? TRUE : FALSE;
}

/* ȡĳ���ڵ������������������ 1582 �� 10 �µ�������� */
int GetDayFromYear(int AYear, int AMonth, int ADay)
{
  int i, ret;
  ret = 0;
  for (i = 1; i < AMonth; i++)
    ret += GetMonthDays(AYear, i);
  ret += ADay;

  return ret;
}

/* �Ӿ��������������º���������������ж��Ƿ������꣬���� False ��ʾ���Ϸ����� */
int ExtractMonthDay(int Days, int AYear, int *AMonth, int *ADay)
{
  int i, day;
  if ((Days <= 0) || (Days > 366) || (Days > 365) && (!GetIsLeapYear(AYear)))
  {
    *AMonth = -1;
    *ADay = -1;
    return FALSE;
  }

  for (i = 1; i <= 12; i++)
  {
    day = GetMonthDays(AYear, i);
    if (Days > day)
      Days = Days - day;
    else
    {
      *ADay = Days;
      *AMonth = i;
      return TRUE;
    }
  }

  return FALSE;
}

/* ��þ๫Ԫԭ������� */
int GetAllDays(int AYear, int AMonth, int ADay)
{
  int i, BaseDays, LeapDays;
  if (AYear > 0)
    BaseDays = (AYear - 1) * 365;
  else
    BaseDays = AYear * 365;

  for (i = 1; i < AMonth; i++)
    BaseDays += GetMonthDays(AYear, i);
  BaseDays += ADay;

  if (AYear >= 0) /* ��Ԫ�� */
  {
    if (GetCalendarType(AYear, AMonth, ADay) != ctGregorian)
    {
      LeapDays = 0;
    }
    else
    {
      /* 1582.10.5/15 ǰ�� Julian ��ֻ������һ���������պ����Ϊ Gregorian �� */
      LeapDays = 10; /* �� Gregory ɾȥ�� 10 �� */

      if (AYear > 1700) /* �����㷨�򻯰棬�� 1701 ��� 11 �� */
      {
        /* ÿһ�����ۼ�һ */
        LeapDays += 1 + ((AYear - 1701) / 100);
        /* �� 400 ���������Ͳ��� */
        LeapDays -= (AYear - 1601) / 400;
      }
    }
    LeapDays = ((AYear - 1) / 4) - LeapDays; /* 4 ��һ���� */
  }
  else /* ��Ԫǰ */
  {
    LeapDays = - ((- AYear + 3) / 4);
  }

  return BaseDays + LeapDays;
}

/* ��ȡ��Ч��׼�������˸���ϵ��ֲ�������ƺ��Ǿ� Gregorian ��Ԫ��Ԫ�������� */
int GetEquStandardDays(int AYear, int AMonth, int ADay)
{
  if (GetCalendarType(AYear, AMonth, ADay) == ctJulian)
  {
    return (AYear - 1) * 365 + ((AYear - 1) / 4) + GetDayFromYear(AYear, AMonth, ADay) - 2;
    /* Ϊɶ�� 2���²⹫Ԫ 1 �굽 1582 �꣬Julian ���� Gregorian �������� 12 �죬
      (100, 200, 300, 500, 600, 700, 900, 1000, 1100, 1300, 1400, 1500)
      �� Gregory ֻɾȥ 10 �죬���������� 2 ��Ĳ�ֵ��
      ��˵������ Gregorian ���� 1582.10.4 ��ǰ���Ƶõ� Gregorian ��Ԫ��Ԫ��
      ��ʵ�ʹ�ԪԪ��Ԫ������ͬһ�졣 */
  }
  else if (GetCalendarType(AYear, AMonth, ADay) == ctGregorian)
  {
    return (AYear - 1) * 365 + ((AYear - 1) / 4) -((AYear - 1) / 100) + 
      ((AYear - 1) / 400) + GetDayFromYear(AYear, AMonth, ADay);
  }
  else
    return -1;
}

/* ���ĳ���������ڼ���0-6 */
int GetWeek(int AYear, int AMonth, int ADay)
{
  int week;
  /* -2 Դ�ڹ�Ԫ 1 �� 1 �� 2 �ղ��������� */
  week = (GetAllDays(AYear, AMonth, ADay) - 2) % 7;
  if (week < 0)
    week += 7;
  return week;
}

/* ��þ�1899-12-31�յ����� */
int EncodeDate(int AYear, int AMonth, int ADay, int* ADate)
{
  int i, leap;

  leap = GetIsLeapYear(AYear) ? 1 : 0;

  if ((AYear >= 1) && (AYear <= 9999) && (AMonth >= 1) && (AMonth <= 12) &&
    (ADay >= 1) && (ADay <= MonthDays[leap][AMonth - 1]))
  {
    /* ������������һ���еĵڼ��� */
    for (i = 1; i < AMonth; i++)
      ADay += MonthDays[leap][i - 1];

    /* �������ڼ��ϴ��㵽��һ��Ԫ�������� */
    i = AYear - 1;
    *ADate = i * 365 + i / 4 - i / 100 + i / 400 + ADay - DateDelta;

    return TRUE;
  } else {
    *ADate = 0;
    return FALSE;
  }
}

/* ���ݾ�1899-12-31�յ��������������� */
int DecodeDate(int ADate, int* AYear, int* AMonth, int* ADay)
{
  int d1 = 365; /* һ������� */
  int d4 = d1 * 4 + 1; /* ���������������һ�� */
  int d100 = d4 * 25 - 1; /* һ��������������겻�� */
  int d400 = d100 * 4 + 1; /* �İ�����������İ������� */
  int y, m, d, i;
  int leap;

  d = ADate + DateDelta;
  if (d <= 0)
  {
    *AYear = 0;
    *AMonth = 0;
    *ADay = 0;
    return FALSE;
  } else {
    /* �����ȼ�һ�����⽫ 12/31 ����� 01/00 */
    d--;

    /* ������� */
    y = d / d400 * 400;
    d = d % d400;

    y += d / d100 * 100;
    d = d % d100;

    y += d / d4 * 4;
    d = d % d4;

    y += d / d1 + 1;
    d = d % d1;

    /* ���ڼ�һ��Ϊ�ڸ�������� */
    d++;

    leap = GetIsLeapYear(y);

    /* �����·ݺ����� */;
    m = 1;
    while (1) {
      i = MonthDays[leap][m - 1];
      if (d <= i) break;
      d -= i;
      m++;
    }

    *AYear = y;
    *AMonth = m;
    *ADay = d;
    return TRUE;
  }
}

/* ����ƫ�� */
int OffsetDate(ODATE* date, int offset)
{
  int days;
  if (EncodeDate(date->year, date->month, date->day, &days))
  {
    days += offset;
    return DecodeDate(days, &date->year, &date->month, &date->day);
  }
  return FALSE;
}

/* ��ü��µڼ������ڼ������ڣ���� AWeek >= 5 ��ʾ���һ�����ڼ� */
int GetDateFromMonthWeek(int AYear, int AMonth, int AWeek, int AWeekDay, int* ADay)
{
  if (GetDateIsValid(AYear, AMonth, 1))
  {
    if (AWeek < 5)
    {
      /* �� AWeek �ܵ����� AWeekDay */
      int week = GetWeek(AYear, AMonth, 1);
      *ADay = (week > AWeekDay ? 7 : 0) + 7 * (AWeek - 1) + (AWeekDay - week) + 1;
      return TRUE;
    }
    else
    {
      /* ���һ������ AWeekDay */
      int lastday = GetMonthDays(AYear, AMonth);
      int week = GetWeek(AYear, AMonth, lastday);
      *ADay = lastday - week + AWeekDay - (week < AWeekDay ? 7 : 0);
      return TRUE;
    }
  }
  return FALSE;
}

/* ���ĳ�������ڵĵ� N �������������������1-24����ӦС�������� */
double GetJieQiDayTimeFromYear(int AYear, int N)
{
  double juD, tht, yrD, shuoD;

  /* ���ڽ����¶�����ɵ�����Ӱ�죬̫������ͨ�������������ʱ�䲢����һ��ȷ�ع���
    ����û���� 365.2422 Ϊ������ֱ�Ӽ������������ʱ�̡����������ʽ����ֲ������
    ���ص�������С���������������ʱ�֡�*/

  if (AYear <= 0) /* ��û�й�Ԫ 0 ��ĵ��� */
    AYear++;

  juD = AYear * (365.2423112 - 6.4e-14 * (AYear-100) * (AYear - 100)
    - 3.047e-8 * (AYear-100)) + 15.218427 * N + 1721050.71301;
  tht = 3e-4 * AYear - 0.372781384 - 0.2617913325 * N;
  yrD = (1.945 * sin(tht) - 0.01206 * sin(2*tht)) * (1.048994 - 2.583e-5 * AYear);
  shuoD = -18e-4 * sin(2.313908653 * AYear- 0.439822951 - 3.0443 * N);
  return juD + yrD + shuoD - GetEquStandardDays(AYear, 1, 0) - 1721425; /* ���� */
  /* (juD - GetEquStandardDays(AYear, 1, 0) - 1721425); ƽ�� */
}

/* ���ĳ������ĵ� N �������Ľ�������ʱ�֣�1-24����ӦС�������� */
int GetJieQiInAYear(int AYear, int N, int *AMonth, int *ADay, int *AHour, int *AMinute)
{
  double Days;
  int i, Day;
  if ((N >= 1) && (N <= 24))
  {
    Days = GetJieQiDayTimeFromYear(AYear, N);
    for (i = 1; i <= 12; i++)
    {
      Day = GetMonthDays(AYear, i);
      if (Days > Day)
        Days = Days - Day;
      else
      {
        *AMonth = i;
        break;
      }
    }
    *ADay = (int)floor(Days);

    Days = Days - (double)*ADay;
    *AHour = (int)floor(Days * 24);

    Days = Days * 24 - (double)*AHour;
    *AMinute = (int)(Days * 60 + 0.5);
    return TRUE;
  }
  else
    return FALSE;
}

/* ��ù����������Ǳ����ʲô������1-24����ӦС�������������򷵻� -1 */
int GetJieQiFromDay(int AYear, int AMonth, int ADay)
{
  /* ���������Ӧ�ö�Ӧ�Ľ����� */
  int N = (AMonth - 1) * 2 + ((ADay < 15) ? 1 : 2);
  int Month, Day, DummyHour, DummyMinute;
  GetJieQiInAYear(AYear, N, &Month, &Day, &DummyHour, &DummyMinute);
  if (Day == ADay)
    return N;
  else
    return -1;
}

/* ��ù������µ�����������Ӧ�����ںͽ�����š�1-24����ӦС�������� */
void GetJieQiFromMonth(int AYear, int AMonth, int *Day1, int *JieQi1, int *Day2, int *JieQi2)
{
  int Month, DummyHour, DummyMinute;
  *JieQi1 = (AMonth - 1) * 2 + 1;
  *JieQi2 = (AMonth - 1) * 2 + 2;
  GetJieQiInAYear(AYear, *JieQi1, &Month, Day1, &DummyHour, &DummyMinute);
  GetJieQiInAYear(AYear, *JieQi2, &Month, Day2, &DummyHour, &DummyMinute);
}

/* ���ݹ��������ջ��ĳ���������ɵ�֧��������Ϊ��ֽ磬0-59 ��Ӧ ���ӵ��ﺥ */
int GetGanZhiFromYear(int AYear, int AMonth, int ADay)
{
  int ret;

  /* ����������ǰ������ǰһ�� */
  if (GetDayFromYear(AYear, AMonth, ADay) < floor(GetJieQiDayTimeFromYear(AYear, 3)))
    AYear--;

  if (AYear > 0)
    ret = (AYear - 4) % 60;
  else /* ��Ҫ�����жϹ�Ԫǰ��ԭ����û�й�Ԫ 0 �� */
    ret = (AYear - 3) % 60;

  if (ret < 0)
    ret += 60;

  return ret;
}

/* ���ĳ�����µ���ɵ�֧��0-59 ��Ӧ ���ӵ��ﺥ */
int GetGanZhiFromMonth(int AYear, int AMonth, int ADay)
{
  int Gan, DummyZhi, i, Days, ret;

  /* ��������¼�¼����Ϊ���µ���ɵ�֧�������������͸�������Ϊ�ֽ�� */
  Days = GetDayFromYear(AYear, AMonth, ADay);
  /* �籾����������ǰ����������ǰһ�� */
  if (Days < floor(GetJieQiDayTimeFromYear(AYear, 3)))
  {
    /* �����Ϊǰһ�� */
    AYear--;
    if (Days < floor(GetJieQiDayTimeFromYear(AYear, 1))) /* ���С��С������ 11 �� */
      AMonth = 11;
    else /* С������������ 12 �� */
      AMonth = 12;
  }
  else
  {
    /* ���㱾��Ľ������������������������� */
    for (i = 1; i <= 12; i++) /* i ���Խ���Ϊ�ֽ���·��� */
    {
      /* ��� i ���׽����ľ���ͷ������С�ڴ��� */
      if (Days >= floor(GetJieQiDayTimeFromYear(AYear, 2 * i + 1)))
        AMonth = i;
      else
        break;
    }
  }

  ret = -1;
  ExtractGanZhi(GetGanZhiFromYear(AYear, AMonth, ADay), &Gan, &DummyZhi);
  switch(Gan) /* ���ݿھ��ӱ���������㱾�����µĸ��� */
  {
  case 0: /* �׼� ������ */
  case 5:
    ret = 2;
    break;
  case 1: /* �Ҹ� ��Ϊͷ */
  case 6:
    ret = 4;
    break;
  case 2: /* ���� Ѱ���� */
  case 7:
    ret = 6;
    break;
  case 3: /* ���� ��λ�� */
  case 8:
    ret = 8;
    break;
  case 4: /* ��� �׺��� */
  case 9:
    ret = 0;
    break;
  }
  ret += (AMonth - 1) % 10; /* ���㱾�¸��� */
  if (ret >= 10)
    ret = ret % 10;

  return CombineGanZhi(ret, (AMonth - 1 + 2) % 12); /* ���֧��������Ϊ�� */
}

/* ���ĳ�����յ���ɵ�֧��0-59 ��Ӧ ���ӵ��ﺥ */
int GetGanZhiFromDay(int AYear, int AMonth, int ADay)
{
  int ret;
  ret = (GetAllDays(AYear, AMonth, ADay) + 12) % 60;
  if (ret < 0)
    ret += 60;
  return ret;
}

/* ���ĳ����ʱ����ɵ�֧��0-59 ��Ӧ ���ӵ��ﺥ */
int GetGanZhiFromHour(int AYear, int AMonth, int ADay, int AHour)
{
  int Gan, Zhi, DummyZhi;

  AHour = AHour % 24;
  ExtractGanZhi(GetGanZhiFromDay(AYear, AMonth, ADay), &Gan, &DummyZhi);

  /* Zhi��ʱ����(0-11)Ҳ����֧�� */
  if (AHour == 23)
  {
    /* ������ʱ */
    Gan = (Gan + 1) % 10;
    Zhi = 0;
  }
  else
  {
    AHour++;
    Zhi = AHour / 2;
  }

  /* Gan ��ʱ�Ǳ��ո��������ݹ�����ɱ�����ʱ������
     Ӧ��Ҳ�пھ������ɲ���Ҫ���ɼ����� */
  if (Gan >= 5)
    Gan -= 5;
  Gan = 2 * Gan;
  
  /* �����ʱ������ */
  Gan = (Gan + Zhi) % 10;

  return CombineGanZhi(Gan, Zhi);
}

/* ���ĳ��/ũ�������ФҲ���ǵ�֧��0-11 ��Ӧ ���� */
int GetShengXiaoFromYear(int AYear)
{
  int ret;

  if (AYear > 0)
    ret = (AYear - 4) % 12;
  else /* ��Ҫ�����жϹ�Ԫǰ��ԭ����û�й�Ԫ 0 �� */
    ret = (AYear - 3) % 12;

  if (ret < 0)
    ret += 12;

  return ret;
}

/* ���ĳ�������յ�������0-11 ��Ӧ ����˫�� */
int GetXingZuoFromMonthDay(int AMonth, int ADay)
{
  /* ÿ����������ʼ���� */
  const int XingZuoDays[12] = {120, 219, 321, 421, 521, 622, 723, 823, 923, 1023, 1123, 1222};
  int i, Days;

  Days = AMonth * 100 + ADay;
  for (i = 0; i <= 11; i++)
  {
    /* �����ڵ�һ�������Ǳ�ƿ���ԵüӸ�ƫ�� */
    if (Days < XingZuoDays[i])
    {
      return (i + 9) % 12;
    }
    else if ((Days >= XingZuoDays[i]) && (Days < XingZuoDays[i + 1]))
    {
      return (i + 10) % 12;
    }
  }

  return -1;
}

/* ��ù������������������еĵڼ��ŵĵڼ��գ�1~9,1~9��Ӧһ�ŵ��žţ�False Ϊ������������ */
int GetShu9Day(int AYear, int AMonth, int ADay, int *JiuSeq, int *JiuDay)
{
  int DongZhi, Days;

  DongZhi = (int)floor(GetJieQiDayTimeFromYear(AYear, 24));
  Days = GetDayFromYear(AYear, AMonth, ADay);

  if ((Days >= DongZhi) && (Days - DongZhi <= 81)) /* �ڽ���ľž��� */
  {
    *JiuSeq = ((Days - DongZhi) / 9) + 1;
    *JiuDay = ((Days - DongZhi) % 9) + 1;
    return TRUE;
  }
  else
  { /* ����Ƿ���ǰһ�������ڵľž� */
    AYear--;
    if (AYear = 0)
      AYear--;

    /* �����һ��Ķ����� */
    DongZhi = (int)floor(GetJieQiDayTimeFromYear(AYear, 24));

    /* ��ô�������һ�����׵ĳ��� */
    Days = Days + 365;
    if (GetIsLeapYear(AYear))
      Days++;

    if ((Days >= DongZhi) && (Days - DongZhi <= 81))
    {
      *JiuSeq = ((Days - DongZhi) / 9) + 1;
      *JiuDay = ((Days - DongZhi) % 9) + 1;
      return TRUE;
    }
  }
  return FALSE;
}

/* ��ù������������������еĵڼ����ĵڼ��գ�1~3,1~10��Ӧ������ĩ�����գ�False Ϊ���ڷ����� */
int Get3FuDay(int AYear, int AMonth, int ADay, int *Fu, int *FuDay)
{
  int Days, XiaZhi, LiQiu;
  int Month, Day, i;
  int Gan, DummyZhi;
  int F1, F2, F3;

  Days = GetDayFromYear(AYear, AMonth, ADay);
  XiaZhi = (int)floor(GetJieQiDayTimeFromYear(AYear, 12)); /* ��������� */
  LiQiu = (int)floor(GetJieQiDayTimeFromYear(AYear, 15)); /* ��������� */

  for (i = XiaZhi + 1; i <= XiaZhi + 21; i++) /* ��֤�����������һ�����յĺ� 10 �죬�������ղ��� */
  {
    if (ExtractMonthDay(i, AYear, &Month, &Day))
    {
      if (ExtractGanZhi(GetGanZhiFromDay(AYear, Month, Day), &Gan, &DummyZhi))
      {
        if (Gan == 6) /* �������һ������ */
        {
          F1 = i + 20; /* �����գ����������� */
          F2 = i + 30; /* �з��գ����ĸ����� */

          if ((Days >= F1) && (Days < F1 + 9))
          {
            *Fu = 1;
            *FuDay = Days - F1 + 1;
            return TRUE;
          }
          else if (Days >= F2) /* �з� */
          {
            if ((Days < F2 + 10) || /* �з� 10 ���� */
              ((Days >= F2 + 10) && (Days < F2 + 20) && (F2 + 20 < LiQiu)))
            {
              *Fu = 2;
              *FuDay = Days - F2 + 1;
              return TRUE;
            }
          }

          break;
        }
      }
    }
  }

  for (i = LiQiu + 1; i <= LiQiu + 21; i++) /* ��֤����������һ�����յĺ� 10 �죬���ﵱ�ղ��� */
  {
    if (ExtractMonthDay(i, AYear, &Month, &Day))
    {
      if (ExtractGanZhi(GetGanZhiFromDay(AYear, Month, Day), &Gan, &DummyZhi))
      {
        if (Gan == 6) /* ������һ������ */
        {
          F3 = i; /* ĩ�� */

          if ((Days >= F3) && (Days < F3 + 10))
          {
            *Fu = 3;
            *FuDay = Days - F3 + 1;
            return TRUE;
          }

          break;
        }
      }
    }
  }

  return FALSE;
}

/* ���ĳ�������е���÷���ڣ�÷�꼾�ڵĿ�ʼ�գ�â�ֺ�ĵ�һ������ */
int GetRuMeiDay(int AYear, int *AMonth, int *ADay)
{
  int i, MangZhong;
  int Gan, DummyZhi;

  MangZhong = (int)floor(GetJieQiDayTimeFromYear(AYear, 11)); /* ���â���� */
  for (i = MangZhong + 1; i <= MangZhong + 21; i++)
  {
    if (ExtractMonthDay(i, AYear, AMonth, ADay))
    {
      if (ExtractGanZhi(GetGanZhiFromDay(AYear, *AMonth, *ADay), &Gan, &DummyZhi))
      {
        if (Gan = 2) // â�ֺ��һ������
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

/* ���ĳ�������еĳ�÷���ڣ�÷�꼾�ڵĽ����գ�С���ĵ�һ��δ�� */
int GetChuMeiDay(int AYear, int *AMonth, int *ADay)
{
  int i, XiaoShu;
  int DummyGan, Zhi;

  XiaoShu = (int)floor(GetJieQiDayTimeFromYear(AYear, 13)); /* ���С���� */
  for (i = XiaoShu + 1; i <= XiaoShu + 21; i++)
  {
    if (ExtractMonthDay(i, AYear, AMonth, ADay))
    {
      if (ExtractGanZhi(GetGanZhiFromDay(AYear, *AMonth, *ADay), &DummyGan, &Zhi))
      {
        if (Zhi = 7) // С����һ��δ��
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

/************************************************************************/
/* ũ������                                                             */
/************************************************************************/

/* ����ũ�� y�����ĸ��� 1-12 , û�򷵻� 0 */
static int leapMonth(int y)
{
  return lunarInfo[y - LUNAR_START_YEAR] & 0xf;
}

/* ����ũ�� y�����µ����� */
static int leapDays(int y)
{
  if (leapMonth(y))
    return (lunarInfo[y - LUNAR_START_YEAR] & 0x10000) ? 30 : 29;
  else 
    return 0;
}

/* ����ũ�� y��������� */
static int lYearDays(int y)
{
  int i, sum = 348;
  for (i = 0x8000; i > 0x8; i >>= 1)
    sum += (lunarInfo[y - LUNAR_START_YEAR] & i) ? 1 : 0;
  return sum + leapDays(y);
}

/* ����ũ�� y��m���Ƿ�С�� */
static int littleMonth(int y, int m, int leap)
{
  if (leap)
    return (lunarInfo[y - LUNAR_START_YEAR] & 0x10000) ? FALSE : TRUE;
  else
    return (lunarInfo[y - LUNAR_START_YEAR] & (0x10000 >> m)) ? FALSE : TRUE;
}

/* ����ũ�� y��m�µ������� */
static int monthDays(int y, int m, int leap)
{
  return littleMonth(y, m, leap) ? 29 : 30;
}

/* ����ũ����Ϣ */
int GetLunarInfo(ODATE *date, OLUNAR *lunar)
{
  int leap, i, temp, offset;

  /* �ж��Ƿ���Ч */
  memset(lunar, 0, sizeof(OLUNAR));
  if (!GetDateIsValid(date->year, date->month, date->day))
    return FALSE;
  if (date->year <= LUNAR_START_YEAR || date->year > LUNAR_END_YEAR)
    return FALSE;

  /* �����������ʼ�յ����� */
  offset = GetAllDays(date->year, date->month, date->day) - GetAllDays(LUNAR_START_YEAR, 1, 31);

  /* ������� */
  for (i = LUNAR_START_YEAR; (i < LUNAR_END_YEAR) && (offset >= 0); i++)
  {
    temp = lYearDays(i);
    if (offset >= temp)
      offset -= temp;
    else
      break;
  }
  lunar->year = i;

  /* ���ĸ��� */
  leap = leapMonth(lunar->year);
  lunar->leap = FALSE;

  for (i = 1; (i < 13) && (offset > 0); i++)
  {
    /* ���� */
    if (leap > 0 && i == (leap + 1) && (lunar->leap == FALSE))
    {
      i--;
      lunar->leap = TRUE;
      temp = leapDays(lunar->year);
    }
    else
    {
      temp = monthDays(lunar->year, i, FALSE);
    }

    /* ������� */
    if ((lunar->leap == TRUE) && (i == leap + 1))
      lunar->leap = FALSE;

    offset -= temp;
  }

  /* �����·� */
  if ((offset == 0) && (leap > 0) && (i == leap + 1))
  {
    if (lunar->leap)
    { 
      lunar->leap = FALSE;
    }
    else
    {
      lunar->leap = TRUE;
      i--;
    }
  }

  /* �����·ݺ����� */
  if (offset < 0)
  {
    offset += temp;
    i--;
  }
  lunar->month = i;
  lunar->day = offset + 1;
  lunar->little = littleMonth(lunar->year, lunar->month, lunar->leap);

  return TRUE;
}

/* ����ũ�����㹫������ */
int LunarToDate(OLUNAR *lunar, ODATE *date)
{
  int leap, i, offset;

  /* �ж��Ƿ���Ч */
  memset(date, 0, sizeof(ODATE));
  if (lunar->year < LUNAR_START_YEAR || lunar->year > LUNAR_END_YEAR ||
    lunar->month <= 0 || lunar->month > 12)
    return FALSE;

  /* �����������ʼ�յ����� */
  EncodeDate(LUNAR_START_YEAR, 1, 31, &offset);

  /* ������� */
  for (i = LUNAR_START_YEAR; i < lunar->year; i++)
    offset += lYearDays(i);

  /* �����·� */
  for (i = 1; i < lunar->month; i++)
    offset += monthDays(lunar->year, i, FALSE);
  leap = leapMonth(lunar->year);
  if (leap)
  {
    if (leap < lunar->month)
      offset += monthDays(lunar->year, leap, TRUE);
    else if (leap == lunar->month && lunar->leap)
      offset += monthDays(lunar->year, lunar->month, FALSE);
  }

  /* �������� */
  offset += lunar->day - 1;

  return DecodeDate(offset, &date->year, &date->month, &date->day);
}
