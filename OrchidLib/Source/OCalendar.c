/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：历法函数
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OCalendar.c,v 1.13 2006/06/02 08:40:41 zjy Exp $
* 备    注：
*******************************************************************************/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "OCalendar.h"

/* 农历信息 1900-2050 年 */
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

/* 每个月的日期数（平年和闰年） */
static const int MonthDays[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

/* 1/1/0001 到 12/31/1899 的天数 */
#define DateDelta 693594


/************************************************************************/
/* 历法计算                                                             */
/************************************************************************/

/* 将天干地支组合成干支，0-9 0-11 转换成 0-59 */
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

/* 将干支拆分成天干地支，0-59 转换成 0-9 0-11 */
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

/* 根据公历日期判断当时历法 */
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

/* 返回某公历是否闰年 */
int GetIsLeapYear(int AYear)
{
  if (GetCalendarType(AYear, 1, 1) == ctGregorian)
    return (AYear % 4 == 0) && ((AYear % 100 != 0) || (AYear % 400 == 0)) ? TRUE : FALSE;
  else if (AYear >= 0)
    return (AYear % 4 == 0) ? TRUE : FALSE;
  else /* 需要独立判断公元前的原因是没有公元 0 年 */
    return (AYear - 3) % 4 == 0 ? TRUE : FALSE;
}

/* 取本月天数，不考虑 1582 年 10 月的特殊情况 */
int GetMonthDays(int AYear, int AMonth)
{
  return MonthDays[GetIsLeapYear(AYear)][AMonth - 1];
}

/* 返回公历日期是否合法 */
int GetDateIsValid(int AYear, int AMonth, int ADay)
{
  int ret;
  ret = (AYear != 0) && (AMonth >= 1) && (AMonth <= 12) &&
    (ADay >= 1) && (ADay <= GetMonthDays(AYear, AMonth));
  if (ret && (AYear == 1582) && (AMonth == 10))
    ret = (ADay < 5) || (ADay > 14);
  return ret ? TRUE : FALSE;
}

/* 返回时间是否合法 */
int GetTimeIsValid(int AHour, int AMinute, int ASecond)
{
  return (AHour >= 0) && (AHour <= 23) && (AMinute >= 0) && (AMinute <= 59) && 
    (ASecond >= 0) && (ASecond <= 59) ? TRUE : FALSE;
}

/* 取某日期到年初的天数，不考虑 1582 年 10 月的特殊情况 */
int GetDayFromYear(int AYear, int AMonth, int ADay)
{
  int i, ret;
  ret = 0;
  for (i = 1; i < AMonth; i++)
    ret += GetMonthDays(AYear, i);
  ret += ADay;

  return ret;
}

/* 从距年首天数返回月和日数，年份用来判断是否是闰年，返回 False 表示不合法日期 */
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

/* 获得距公元原点的日数 */
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

  if (AYear >= 0) /* 公元后 */
  {
    if (GetCalendarType(AYear, AMonth, ADay) != ctGregorian)
    {
      LeapDays = 0;
    }
    else
    {
      /* 1582.10.5/15 前的 Julian 历只有四年一闰，历法此日后调整为 Gregorian 历 */
      LeapDays = 10; /* 被 Gregory 删去的 10 天 */

      if (AYear > 1700) /* 修正算法简化版，从 1701 年的 11 起 */
      {
        /* 每一世纪累加一 */
        LeapDays += 1 + ((AYear - 1701) / 100);
        /* 但 400 整除的世纪不加 */
        LeapDays -= (AYear - 1601) / 400;
      }
    }
    LeapDays = ((AYear - 1) / 4) - LeapDays; /* 4 年一闰数 */
  }
  else /* 公元前 */
  {
    LeapDays = - ((- AYear + 3) / 4);
  }

  return BaseDays + LeapDays;
}

/* 获取等效标准天数，此概念系移植而来，似乎是距 Gregorian 历元年元旦的天数 */
int GetEquStandardDays(int AYear, int AMonth, int ADay)
{
  if (GetCalendarType(AYear, AMonth, ADay) == ctJulian)
  {
    return (AYear - 1) * 365 + ((AYear - 1) / 4) + GetDayFromYear(AYear, AMonth, ADay) - 2;
    /* 为啥减 2？猜测公元 1 年到 1582 年，Julian 历较 Gregorian 历多闰了 12 天，
      (100, 200, 300, 500, 600, 700, 900, 1000, 1100, 1300, 1400, 1500)
      而 Gregory 只删去 10 天，所以留下了 2 天的差值。
      这说明，按 Gregorian 历从 1582.10.4 往前倒推得的 Gregorian 历元年元旦
      和实际公元元年元旦不是同一天。 */
  }
  else if (GetCalendarType(AYear, AMonth, ADay) == ctGregorian)
  {
    return (AYear - 1) * 365 + ((AYear - 1) / 4) -((AYear - 1) / 100) + 
      ((AYear - 1) / 400) + GetDayFromYear(AYear, AMonth, ADay);
  }
  else
    return -1;
}

/* 获得某日期是星期几，0-6 */
int GetWeek(int AYear, int AMonth, int ADay)
{
  int week;
  /* -2 源于公元 1 年 1 月 2 日才是星期天 */
  week = (GetAllDays(AYear, AMonth, ADay) - 2) % 7;
  if (week < 0)
    week += 7;
  return week;
}

/* 获得距1899-12-31日的天数 */
int EncodeDate(int AYear, int AMonth, int ADay, int* ADate)
{
  int i, leap;

  leap = GetIsLeapYear(AYear) ? 1 : 0;

  if ((AYear >= 1) && (AYear <= 9999) && (AMonth >= 1) && (AMonth <= 12) &&
    (ADay >= 1) && (ADay <= MonthDays[leap][AMonth - 1]))
  {
    /* 计算日期是这一年中的第几天 */
    for (i = 1; i < AMonth; i++)
      ADay += MonthDays[leap][i - 1];

    /* 年内日期加上从零到这一年元旦的天数 */
    i = AYear - 1;
    *ADate = i * 365 + i / 4 - i / 100 + i / 400 + ADay - DateDelta;

    return TRUE;
  } else {
    *ADate = 0;
    return FALSE;
  }
}

/* 根据距1899-12-31日的天数返回年月日 */
int DecodeDate(int ADate, int* AYear, int* AMonth, int* ADay)
{
  int d1 = 365; /* 一年的天数 */
  int d4 = d1 * 4 + 1; /* 四年的天数（四年一闰） */
  int d100 = d4 * 25 - 1; /* 一百年的天数（百年不闰） */
  int d400 = d100 * 4 + 1; /* 四百年的天数（四百年又闰） */
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
    /* 日期先减一，避免将 12/31 计算成 01/00 */
    d--;

    /* 计算年份 */
    y = d / d400 * 400;
    d = d % d400;

    y += d / d100 * 100;
    d = d % d100;

    y += d / d4 * 4;
    d = d % d4;

    y += d / d1 + 1;
    d = d % d1;

    /* 日期加一后为在该年的天数 */
    d++;

    leap = GetIsLeapYear(y);

    /* 计算月份和日期 */;
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

/* 日期偏移 */
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

/* 获得几月第几个星期几的日期，如果 AWeek >= 5 表示最后一个星期几 */
int GetDateFromMonthWeek(int AYear, int AMonth, int AWeek, int AWeekDay, int* ADay)
{
  if (GetDateIsValid(AYear, AMonth, 1))
  {
    if (AWeek < 5)
    {
      /* 第 AWeek 周的星期 AWeekDay */
      int week = GetWeek(AYear, AMonth, 1);
      *ADay = (week > AWeekDay ? 7 : 0) + 7 * (AWeek - 1) + (AWeekDay - week) + 1;
      return TRUE;
    }
    else
    {
      /* 最后一个星期 AWeekDay */
      int lastday = GetMonthDays(AYear, AMonth);
      int week = GetWeek(AYear, AMonth, lastday);
      *ADay = lastday - week + AWeekDay - (week < AWeekDay ? 7 : 0);
      return TRUE;
    }
  }
  return FALSE;
}

/* 获得某公历年内的第 N 个节气距年初的天数，1-24，对应小寒到冬至 */
double GetJieQiDayTimeFromYear(int AYear, int N)
{
  double juD, tht, yrD, shuoD;

  /* 由于进动章动等造成的岁差的影响，太阳两次通过各个定气点的时间并不是一精确回归年
    所以没法以 365.2422 为周期来直接计算各个节气的时刻。下面这个公式属移植而来。
    返回的天数是小数，可折算成月日时分。*/

  if (AYear <= 0) /* 对没有公元 0 年的调整 */
    AYear++;

  juD = AYear * (365.2423112 - 6.4e-14 * (AYear-100) * (AYear - 100)
    - 3.047e-8 * (AYear-100)) + 15.218427 * N + 1721050.71301;
  tht = 3e-4 * AYear - 0.372781384 - 0.2617913325 * N;
  yrD = (1.945 * sin(tht) - 0.01206 * sin(2*tht)) * (1.048994 - 2.583e-5 * AYear);
  shuoD = -18e-4 * sin(2.313908653 * AYear- 0.439822951 - 3.0443 * N);
  return juD + yrD + shuoD - GetEquStandardDays(AYear, 1, 0) - 1721425; /* 定气 */
  /* (juD - GetEquStandardDays(AYear, 1, 0) - 1721425); 平气 */
}

/* 获得某公历年的第 N 个节气的交节月日时分，1-24，对应小寒到冬至 */
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

/* 获得公历年月日是本年的什么节气，1-24，对应小寒到冬至，无则返回 -1 */
int GetJieQiFromDay(int AYear, int AMonth, int ADay)
{
  /* 计算该日期应该对应的节气号 */
  int N = (AMonth - 1) * 2 + ((ADay < 15) ? 1 : 2);
  int Month, Day, DummyHour, DummyMinute;
  GetJieQiInAYear(AYear, N, &Month, &Day, &DummyHour, &DummyMinute);
  if (Day == ADay)
    return N;
  else
    return -1;
}

/* 获得公历年月的两个节气对应的日期和节气编号。1-24，对应小寒到冬至 */
void GetJieQiFromMonth(int AYear, int AMonth, int *Day1, int *JieQi1, int *Day2, int *JieQi2)
{
  int Month, DummyHour, DummyMinute;
  *JieQi1 = (AMonth - 1) * 2 + 1;
  *JieQi2 = (AMonth - 1) * 2 + 2;
  GetJieQiInAYear(AYear, *JieQi1, &Month, Day1, &DummyHour, &DummyMinute);
  GetJieQiInAYear(AYear, *JieQi2, &Month, Day2, &DummyHour, &DummyMinute);
}

/* 根据公历年月日获得某公历年的天干地支，以立春为年分界，0-59 对应 甲子到癸亥 */
int GetGanZhiFromYear(int AYear, int AMonth, int ADay)
{
  int ret;

  /* 如是立春日前，属于前一年 */
  if (GetDayFromYear(AYear, AMonth, ADay) < floor(GetJieQiDayTimeFromYear(AYear, 3)))
    AYear--;

  if (AYear > 0)
    ret = (AYear - 4) % 60;
  else /* 需要独立判断公元前的原因是没有公元 0 年 */
    ret = (AYear - 3) % 60;

  if (ret < 0)
    ret += 60;

  return ret;
}

/* 获得某公历月的天干地支，0-59 对应 甲子到癸亥 */
int GetGanZhiFromMonth(int AYear, int AMonth, int ADay)
{
  int Gan, DummyZhi, i, Days, ret;

  /* 调整年和月记录，因为年月的天干地支计算是以立春和各个节气为分界的 */
  Days = GetDayFromYear(AYear, AMonth, ADay);
  /* 如本日是立春日前，则是属于前一年 */
  if (Days < floor(GetJieQiDayTimeFromYear(AYear, 3)))
  {
    /* 年调整为前一年 */
    AYear--;
    if (Days < floor(GetJieQiDayTimeFromYear(AYear, 1))) /* 如果小于小寒则算 11 月 */
      AMonth = 11;
    else /* 小寒和立春间算 12 月 */
      AMonth = 12;
  }
  else
  {
    /* 计算本年的节气，看该日落在哪俩节气内 */
    for (i = 1; i <= 12; i++) /* i 是以节气为分界的月份数 */
    {
      /* 如果 i 月首节气的距年头的日数小于此日 */
      if (Days >= floor(GetJieQiDayTimeFromYear(AYear, 2 * i + 1)))
        AMonth = i;
      else
        break;
    }
  }

  ret = -1;
  ExtractGanZhi(GetGanZhiFromYear(AYear, AMonth, ADay), &Gan, &DummyZhi);
  switch(Gan) /* 根据口诀从本年干数计算本年首月的干数 */
  {
  case 0: /* 甲己 丙佐首 */
  case 5:
    ret = 2;
    break;
  case 1: /* 乙庚 戊为头 */
  case 6:
    ret = 4;
    break;
  case 2: /* 丙辛 寻庚起 */
  case 7:
    ret = 6;
    break;
  case 3: /* 丁壬 壬位流 */
  case 8:
    ret = 8;
    break;
  case 4: /* 戊癸 甲好求 */
  case 9:
    ret = 0;
    break;
  }
  ret += (AMonth - 1) % 10; /* 计算本月干数 */
  if (ret >= 10)
    ret = ret % 10;

  return CombineGanZhi(ret, (AMonth - 1 + 2) % 12); /* 组合支数，正月为寅 */
}

/* 获得某公历日的天干地支，0-59 对应 甲子到癸亥 */
int GetGanZhiFromDay(int AYear, int AMonth, int ADay)
{
  int ret;
  ret = (GetAllDays(AYear, AMonth, ADay) + 12) % 60;
  if (ret < 0)
    ret += 60;
  return ret;
}

/* 获得某公历时的天干地支，0-59 对应 甲子到癸亥 */
int GetGanZhiFromHour(int AYear, int AMonth, int ADay, int AHour)
{
  int Gan, Zhi, DummyZhi;

  AHour = AHour % 24;
  ExtractGanZhi(GetGanZhiFromDay(AYear, AMonth, ADay), &Gan, &DummyZhi);

  /* Zhi是时辰数(0-11)也就是支数 */
  if (AHour == 23)
  {
    /* 次日子时 */
    Gan = (Gan + 1) % 10;
    Zhi = 0;
  }
  else
  {
    AHour++;
    Zhi = AHour / 2;
  }

  /* Gan 此时是本日干数，根据规则换算成本日首时辰干数
     应该也有口诀，但可不需要，可简化如下 */
  if (Gan >= 5)
    Gan -= 5;
  Gan = 2 * Gan;
  
  /* 计算此时辰干数 */
  Gan = (Gan + Zhi) % 10;

  return CombineGanZhi(Gan, Zhi);
}

/* 获得某公/农历年的生肖也就是地支，0-11 对应 鼠到猪 */
int GetShengXiaoFromYear(int AYear)
{
  int ret;

  if (AYear > 0)
    ret = (AYear - 4) % 12;
  else /* 需要独立判断公元前的原因是没有公元 0 年 */
    ret = (AYear - 3) % 12;

  if (ret < 0)
    ret += 12;

  return ret;
}

/* 获得某公历月日的星座，0-11 对应 白羊到双鱼 */
int GetXingZuoFromMonthDay(int AMonth, int ADay)
{
  /* 每个星座的起始日期 */
  const int XingZuoDays[12] = {120, 219, 321, 421, 521, 622, 723, 823, 923, 1023, 1123, 1222};
  int i, Days;

  Days = AMonth * 100 + ADay;
  for (i = 0; i <= 11; i++)
  {
    /* 数组内第一个星座是宝瓶所以得加个偏移 */
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

/* 获得公历年月日在数九日中的第几九的第几日，1~9,1~9对应一九到九九，False 为不在数九日内 */
int GetShu9Day(int AYear, int AMonth, int ADay, int *JiuSeq, int *JiuDay)
{
  int DongZhi, Days;

  DongZhi = (int)floor(GetJieQiDayTimeFromYear(AYear, 24));
  Days = GetDayFromYear(AYear, AMonth, ADay);

  if ((Days >= DongZhi) && (Days - DongZhi <= 81)) /* 在今年的九九内 */
  {
    *JiuSeq = ((Days - DongZhi) / 9) + 1;
    *JiuDay = ((Days - DongZhi) % 9) + 1;
    return TRUE;
  }
  else
  { /* 检查是否是前一公历年内的九九 */
    AYear--;
    if (AYear = 0)
      AYear--;

    /* 获得上一年的冬至日 */
    DongZhi = (int)floor(GetJieQiDayTimeFromYear(AYear, 24));

    /* 获得此日离上一年年首的长度 */
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

/* 获得公历年月日在三伏日中的第几伏的第几日，1~3,1~10对应初伏到末伏伏日，False 为不在伏日内 */
int Get3FuDay(int AYear, int AMonth, int ADay, int *Fu, int *FuDay)
{
  int Days, XiaZhi, LiQiu;
  int Month, Day, i;
  int Gan, DummyZhi;
  int F1, F2, F3;

  Days = GetDayFromYear(AYear, AMonth, ADay);
  XiaZhi = (int)floor(GetJieQiDayTimeFromYear(AYear, 12)); /* 获得夏至日 */
  LiQiu = (int)floor(GetJieQiDayTimeFromYear(AYear, 15)); /* 获得立秋日 */

  for (i = XiaZhi + 1; i <= XiaZhi + 21; i++) /* 保证包括夏至后第一个庚日的后 10 天，夏至当日不算 */
  {
    if (ExtractMonthDay(i, AYear, &Month, &Day))
    {
      if (ExtractGanZhi(GetGanZhiFromDay(AYear, Month, Day), &Gan, &DummyZhi))
      {
        if (Gan == 6) /* 夏至后第一个庚日 */
        {
          F1 = i + 20; /* 初伏日，第三个庚日 */
          F2 = i + 30; /* 中伏日，第四个庚日 */

          if ((Days >= F1) && (Days < F1 + 9))
          {
            *Fu = 1;
            *FuDay = Days - F1 + 1;
            return TRUE;
          }
          else if (Days >= F2) /* 中伏 */
          {
            if ((Days < F2 + 10) || /* 中伏 10 日内 */
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

  for (i = LiQiu + 1; i <= LiQiu + 21; i++) /* 保证包括立秋后第一个庚日的后 10 天，立秋当日不算 */
  {
    if (ExtractMonthDay(i, AYear, &Month, &Day))
    {
      if (ExtractGanZhi(GetGanZhiFromDay(AYear, Month, Day), &Gan, &DummyZhi))
      {
        if (Gan == 6) /* 立秋后第一个庚日 */
        {
          F3 = i; /* 末伏 */

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

/* 获得某公历年中的入梅日期，梅雨季节的开始日，芒种后的第一个丙日 */
int GetRuMeiDay(int AYear, int *AMonth, int *ADay)
{
  int i, MangZhong;
  int Gan, DummyZhi;

  MangZhong = (int)floor(GetJieQiDayTimeFromYear(AYear, 11)); /* 获得芒种日 */
  for (i = MangZhong + 1; i <= MangZhong + 21; i++)
  {
    if (ExtractMonthDay(i, AYear, AMonth, ADay))
    {
      if (ExtractGanZhi(GetGanZhiFromDay(AYear, *AMonth, *ADay), &Gan, &DummyZhi))
      {
        if (Gan = 2) // 芒种后第一个丙日
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

/* 获得某公历年中的出梅日期，梅雨季节的结束日，小暑后的第一个未日 */
int GetChuMeiDay(int AYear, int *AMonth, int *ADay)
{
  int i, XiaoShu;
  int DummyGan, Zhi;

  XiaoShu = (int)floor(GetJieQiDayTimeFromYear(AYear, 13)); /* 获得小暑日 */
  for (i = XiaoShu + 1; i <= XiaoShu + 21; i++)
  {
    if (ExtractMonthDay(i, AYear, AMonth, ADay))
    {
      if (ExtractGanZhi(GetGanZhiFromDay(AYear, *AMonth, *ADay), &DummyGan, &Zhi))
      {
        if (Zhi = 7) // 小暑后第一个未日
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

/************************************************************************/
/* 农历计算                                                             */
/************************************************************************/

/* 返回农历 y年闰哪个月 1-12 , 没闰返回 0 */
static int leapMonth(int y)
{
  return lunarInfo[y - LUNAR_START_YEAR] & 0xf;
}

/* 返回农历 y年闰月的天数 */
static int leapDays(int y)
{
  if (leapMonth(y))
    return (lunarInfo[y - LUNAR_START_YEAR] & 0x10000) ? 30 : 29;
  else 
    return 0;
}

/* 返回农历 y年的总天数 */
static int lYearDays(int y)
{
  int i, sum = 348;
  for (i = 0x8000; i > 0x8; i >>= 1)
    sum += (lunarInfo[y - LUNAR_START_YEAR] & i) ? 1 : 0;
  return sum + leapDays(y);
}

/* 返回农历 y年m月是否小月 */
static int littleMonth(int y, int m, int leap)
{
  if (leap)
    return (lunarInfo[y - LUNAR_START_YEAR] & 0x10000) ? FALSE : TRUE;
  else
    return (lunarInfo[y - LUNAR_START_YEAR] & (0x10000 >> m)) ? FALSE : TRUE;
}

/* 返回农历 y年m月的总天数 */
static int monthDays(int y, int m, int leap)
{
  return littleMonth(y, m, leap) ? 29 : 30;
}

/* 返回农历信息 */
int GetLunarInfo(ODATE *date, OLUNAR *lunar)
{
  int leap, i, temp, offset;

  /* 判断是否有效 */
  memset(lunar, 0, sizeof(OLUNAR));
  if (!GetDateIsValid(date->year, date->month, date->day))
    return FALSE;
  if (date->year <= LUNAR_START_YEAR || date->year > LUNAR_END_YEAR)
    return FALSE;

  /* 计算相对于起始日的天数 */
  offset = GetAllDays(date->year, date->month, date->day) - GetAllDays(LUNAR_START_YEAR, 1, 31);

  /* 计算年份 */
  for (i = LUNAR_START_YEAR; (i < LUNAR_END_YEAR) && (offset >= 0); i++)
  {
    temp = lYearDays(i);
    if (offset >= temp)
      offset -= temp;
    else
      break;
  }
  lunar->year = i;

  /* 闰哪个月 */
  leap = leapMonth(lunar->year);
  lunar->leap = FALSE;

  for (i = 1; (i < 13) && (offset > 0); i++)
  {
    /* 闰月 */
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

    /* 解除闰月 */
    if ((lunar->leap == TRUE) && (i == leap + 1))
      lunar->leap = FALSE;

    offset -= temp;
  }

  /* 修正月份 */
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

  /* 阴历月份和日期 */
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

/* 根据农历计算公历日期 */
int LunarToDate(OLUNAR *lunar, ODATE *date)
{
  int leap, i, offset;

  /* 判断是否有效 */
  memset(date, 0, sizeof(ODATE));
  if (lunar->year < LUNAR_START_YEAR || lunar->year > LUNAR_END_YEAR ||
    lunar->month <= 0 || lunar->month > 12)
    return FALSE;

  /* 计算相对于起始日的天数 */
  EncodeDate(LUNAR_START_YEAR, 1, 31, &offset);

  /* 处理年份 */
  for (i = LUNAR_START_YEAR; i < lunar->year; i++)
    offset += lYearDays(i);

  /* 处理月份 */
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

  /* 处理日期 */
  offset += lunar->day - 1;

  return DecodeDate(offset, &date->year, &date->month, &date->day);
}
