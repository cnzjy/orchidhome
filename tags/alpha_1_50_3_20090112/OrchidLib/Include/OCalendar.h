/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：历法函数头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OCalendar.h,v 1.12 2006/06/02 08:40:41 zjy Exp $
* 备    注：
*******************************************************************************/

#ifndef OCALENDAR_H
#define OCALENDAR_H

#include "OTypes.h"

#if defined(__cplusplus)
extern "C" {
#endif

// 农历数据起始年份
#define LUNAR_START_YEAR 1900
#define LUNAR_END_YEAR   2050

typedef enum CALENDER_TYPE_enum {
  ctInvalid,
  ctJulian,
  ctGregorian
} CALENDER_TYPE;

/* 农历结构 */
typedef struct OLUNAR_struct {
  int year;                        /* 农历年 */
  int month;                       /* 农历月 */
  int day;                         /* 农历日 */
  int leap;                        /* 是否闰月 */
  int little;                      /* 是否小月29天 */
} OLUNAR;

/************************************************************************/
/* 历法计算                                                             */
/************************************************************************/

/* 将天干地支组合成干支，0-9 0-11 转换成 0-59 */
int CombineGanZhi(int Gan, int Zhi);

/* 将干支拆分成天干地支，0-59 转换成 0-9 0-11 */
int ExtractGanZhi(int GanZhi, int *Gan, int *Zhi);

/* 根据公历日期判断当时历法 */
CALENDER_TYPE GetCalendarType(int AYear, int AMonth, int ADay);

/* 返回某公历是否闰年 */
int GetIsLeapYear(int AYear);

/* 取本月天数，不考虑 1582 年 10 月的特殊情况 */
int GetMonthDays(int AYear, int AMonth);

/* 返回公历日期是否合法 */
int GetDateIsValid(int AYear, int AMonth, int ADay);

/* 返回时间是否合法 */
int GetTimeIsValid(int AHour, int AMinute, int ASecond);

/* 取某日期到年初的天数，不考虑 1582 年 10 月的特殊情况 */
int GetDayFromYear(int AYear, int AMonth, int ADay);

/* 从距年首天数返回月和日数，年份用来判断是否是闰年，返回 False 表示不合法日期 */
int ExtractMonthDay(int Days, int AYear, int *AMonth, int *ADay);

/* 获得距公元原点的日数 */
int GetAllDays(int AYear, int AMonth, int ADay);

/* 获取等效标准天数，此概念系移植而来，似乎是距 Gregorian 历元年元旦的天数 */
int GetEquStandardDays(int AYear, int AMonth, int ADay);

/* 获得某日期是星期几，0-6 */
int GetWeek(int AYear, int AMonth, int ADay);

/* 获得距1899-12-31日的天数 */
int EncodeDate(int AYear, int AMonth, int ADay, int* ADate);

/* 根据距1899-12-31日的天数返回年月日 */
int DecodeDate(int ADate, int* AYear, int* AMonth, int* ADay);

/* 日期偏移 */
int OffsetDate(ODATE* date, int offset);

/* 获得几月第几个星期几的日期，如果 AWeek >= 5 表示最后一个星期几 */
int GetDateFromMonthWeek(int AYear, int AMonth, int AWeek, int AWeekDay, int* ADay);

/* 获得某公历年内的第 N 个节气距年初的天数，1-24，对应小寒到冬至 */
double GetJieQiDayTimeFromYear(int AYear, int N);

/* 获得某公历年的第 N 个节气的交节月日时分，1-24，对应小寒到冬至 */
int GetJieQiInAYear(int AYear, int N, int *AMonth, int *ADay, int *AHour, int *AMinute);

/* 获得公历年月日是本年的什么节气，1-24，对应小寒到冬至，无则返回 -1 */
int GetJieQiFromDay(int AYear, int AMonth, int ADay);

/* 获得公历年月的两个节气对应的日期和节气编号。1-24，对应小寒到冬至 */
void GetJieQiFromMonth(int AYear, int AMonth, int *Day1, int *JieQi1, int *Day2, int *JieQi2);

/* 根据公历年月日获得某公历年的天干地支，以立春为年分界，0-59 对应 甲子到癸亥 */
int GetGanZhiFromYear(int AYear, int AMonth, int ADay);

/* 获得某公历月的天干地支，0-59 对应 甲子到癸亥 */
int GetGanZhiFromMonth(int AYear, int AMonth, int ADay);

/* 获得某公历日的天干地支，0-59 对应 甲子到癸亥 */
int GetGanZhiFromDay(int AYear, int AMonth, int ADay);

/* 获得某公历时的天干地支，0-59 对应 甲子到癸亥 */
int GetGanZhiFromHour(int AYear, int AMonth, int ADay, int AHour);

/* 获得某公/农历年的生肖也就是地支，0-11 对应 鼠到猪 */
int GetShengXiaoFromYear(int AYear);

/* 获得某公历月日的星座，0-11 对应 白羊到双鱼 */
int GetXingZuoFromMonthDay(int AMonth, int ADay);

/* 获得公历年月日在数九日中的第几九的第几日，1~9,1~9对应一九到九九，False 为不在数九日内 */
int GetShu9Day(int AYear, int AMonth, int ADay, int *JiuSeq, int *JiuDay);

/* 获得公历年月日在三伏日中的第几伏的第几日，1~3,1~10对应初伏到末伏伏日，False 为不在伏日内 */
int Get3FuDay(int AYear, int AMonth, int ADay, int *Fu, int *FuDay);

/* 获得某公历年中的入梅日期，梅雨季节的开始日，芒种后的第一个丙日 */
int GetRuMeiDay(int AYear, int *AMonth, int *ADay);

/* 获得某公历年中的出梅日期，梅雨季节的结束日，小暑后的第一个未日 */
int GetChuMeiDay(int AYear, int *AMonth, int *ADay);


/************************************************************************/
/* 农历计算                                                             */
/************************************************************************/

/* 返回农历信息 */
int GetLunarInfo(ODATE *date, OLUNAR *lunar);

/* 根据农历计算公历日期 */
int LunarToDate(OLUNAR *lunar, ODATE *date);

#if defined(__cplusplus)
}
#endif 

#endif /* OCALENDAR_H */