/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：历法函数
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OCalInfo.c,v 1.4 2006/06/02 10:02:28 zjy Exp $
* 备    注：
*******************************************************************************/

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "OCalInfo.h"

/* 天干 */
static const char* sGan[10] = {"甲","乙","丙","丁","戊","己","庚","辛","壬","癸"};

/* 地支 */
static const char* sZhi[12] = {"子","丑","寅","卯","辰","巳","午","未","申","酉","戌","亥"};

/* 生肖 */
static const char* sShengXiao[12] = {"鼠","牛","虎","兔","龙","蛇","马","羊","猴","鸡","狗","猪"};

static const char* sXingZuo[12] = {"白羊","金牛","双子","巨蟹","狮子","处女","天秤","天蝎","射手","摩羯","宝瓶","双鱼"};
static const char* sZuo = "座";

/* 节气 */
static const char* sJieQi[24] = {"小寒","大寒","立春","雨水","惊蛰","春分","清明","谷雨",
                                 "立夏","小满","芒种","夏至","小暑","大暑","立秋","白露",
                                 "秋分","处暑","寒露","霜降","立冬","小雪","大雪","冬至"};

/* 年份字符串 */
static const char* sGuangXu = "光绪";
static const char* sXuanTong = "宣统";
static const char* sMinGuo = "民国";
static const char* sGongYuan = "公元";
static const char* sYuan = "元";
static const char* sNian = "年";

/* 月份字符串 */
static const char* sLeapMonth = "闰";
static const char* sMonthName[12] = {"一","二","三","四","五","六","七","八","九","十","十一","十二"};
static const char* sMonth = "月";
static const char* sBigMonth = "大";
static const char* sLittleMonth = "小";

/* 日期字符串 */
static const char* nStr1[11] = {"日","一","二","三","四","五","六","七","八","九","十"};
static const char* nStr2[4] = {"初","十","廿","卅"};

/* 星期字符串 */
static const char* sWeek = "星期";
static const char* sWeekName[7] = {"日","一","二","三","四","五","六"};

/* 传入 offset 返回干支, 0=甲子 */
static void cyclical(int num, char* sz)
{
  strcpy(sz, sGan[num % 10]);
  strcat(sz, sZhi[num % 12]);
}

/* 返回历法信息 */
int GetCalendarInfo(ODATE *date, OTIME *time, OCALENDAR_INFO *info)
{
  memset(info, 0, sizeof(OCALENDAR_INFO));
  if (!GetDateIsValid(date->year, date->month, date->day))
    return FALSE;

  /* 初始化数据 */
  info->date = *date;
  info->time = *time;
  info->LeapYear = GetIsLeapYear(date->year);
  info->Week = GetWeek(date->year, date->month, date->day);

  /* 计算节气 */
  info->JieQi = GetJieQiFromDay(date->year, date->month, date->day);

  /* 计算星座 */
  info->XingZuo = GetXingZuoFromMonthDay(date->month, date->day);

  /* 计算生肖 */
  info->ShengXiao = GetShengXiaoFromYear(date->year);

  /* 计算干支 */
  info->gzYear = GetGanZhiFromYear(date->year, date->month, date->day);
  info->gzMonth = GetGanZhiFromMonth(date->year, date->month, date->day);
  info->gzDay = GetGanZhiFromDay(date->year, date->month, date->day);
  info->gzHour = GetGanZhiFromHour(date->year, date->month, date->day, time->hour);

  /* 计算农历 */
  if (GetLunarInfo(date, &info->lunar))
  {
    /* 计算生肖 */
    info->ShengXiao = GetShengXiaoFromYear(info->lunar.year);

    /* 农历年份 */
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

    /* 农历月份 */
    if (info->lunar.leap)
      sprintf(info->sMonth, "%s%s", sLeapMonth, sMonthName[info->lunar.month - 1]);
    else
      strcpy(info->sMonth, sMonthName[info->lunar.month - 1]);
    strcat(info->sMonth, sMonth);
    strcat(info->sMonth, info->lunar.little ? sLittleMonth : sBigMonth);

    /* 农历日期 */
    if (info->lunar.day == 10)
      sprintf(info->sDay, "%s%s", nStr2[0], nStr1[10]);
    else if (info->lunar.day == 20)
      sprintf(info->sDay, "%s%s", nStr2[2], nStr1[10]);
    else if (info->lunar.day == 30)
      sprintf(info->sDay, "%s%s", nStr1[3], nStr1[10]);
    else
      sprintf(info->sDay, "%s%s", nStr2[info->lunar.day / 10], nStr1[info->lunar.day % 10]);
  }

  /* 星期字符串 */
  sprintf(info->sWeek, "%s%s", sWeek, sWeekName[info->Week]);

  /* 节气字符串 */
  if (info->JieQi > 0)
    strcpy(info->sJieQi, sJieQi[info->JieQi - 1]);

  /* 星座 */
  sprintf(info->sXingZuo, "%s%s", sXingZuo[info->XingZuo], sZuo);

  /* 生肖 */
  strcpy(info->sShengXiao, sShengXiao[info->ShengXiao]);

  /* 干支字符串 */
  cyclical(info->gzYear, info->sgzYear);
  cyclical(info->gzMonth, info->sgzMonth);
  cyclical(info->gzDay, info->sgzDay);
  cyclical(info->gzHour, info->sgzHour);

  return TRUE;
}
