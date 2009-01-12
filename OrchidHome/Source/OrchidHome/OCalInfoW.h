/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：历法函数头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OCalInfoW.h,v 1.6 2006/06/02 10:02:47 zjy Exp $
* 备    注：
*******************************************************************************/

#ifndef OCALINFOW_H
#define OCALINFOW_H

#include "OTypes.h"
#include "OCalendar.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define STR_LEN  5
#define MAX_DATE_LEN  5

/* 历法信息结构 */
typedef struct OCALENDAR_INFO_struct {
  ODATE date;                       /* 阳历日期 */
  OTIME time;                       /* 阳历时间 */

  int LeapYear;                     /* 是否闰年 */
  int Week;                         /* 星期 0-6 (星期日为0) */
  int WeekOfYear;                   /* 在该年中的第几个星期 */
  int WeekOfMonth;                  /* 在该月中的第几个星期 */
                                    
  OLUNAR lunar;                     /* 阴历日期 */
                                   
  int JieQi;                        /* 节气序号 0-23，不是为 -1 */
  int XingZuo;                      /* 星座序号 0-11 */
  int ShengXiao;                    /* 生肖序号 */
                                   
  int gzYear;                       /* 年份干支 */
  int gzMonth;                      /* 月份干支 */
  int gzDay;                        /* 日期干支 */
  int gzHour;                       /* 时间干支 */

  wchar_t sYear[MAX_DATE_LEN * 3];  /* 中文年字符串 (如:公元2005年) */
  wchar_t sMonth[MAX_DATE_LEN];     /* 农历月字符串 */
  wchar_t sLeapMonth[MAX_DATE_LEN]; /* 农历闰月 */
  wchar_t sShortMonth[MAX_DATE_LEN];/* 农历短格式月字符串 */
  wchar_t sMonthLen[MAX_DATE_LEN];  /* 农历月大或小 */
  wchar_t sDay[MAX_DATE_LEN];       /* 农历日字符串 */
  wchar_t sWeek[MAX_DATE_LEN];      /* 星期字符串 */

  wchar_t sJieQi[MAX_DATE_LEN];     /* 节气 */
  wchar_t sXingZuo[MAX_DATE_LEN];   /* 星座 */
  wchar_t sShengXiao[MAX_DATE_LEN]; /* 生肖字符串 */

  wchar_t sgzYear[MAX_DATE_LEN];    /* 年份干支 */
  wchar_t sgzMonth[MAX_DATE_LEN];   /* 月份干支 */
  wchar_t sgzDay[MAX_DATE_LEN];     /* 日期干支 */
  wchar_t sgzHour[MAX_DATE_LEN];    /* 时间干支 */
} OCALENDAR_INFO;

/* 天干 */
extern wchar_t sGan[10][2];

/* 地支 */
extern wchar_t sZhi[12][2];

/* 生肖 */
extern wchar_t sShengXiao[12][2];

extern wchar_t sXingZuo[12][3];
extern wchar_t sZuo[STR_LEN];

/* 节气 */
extern wchar_t sJieQi[24][3];

/* 年份字符串 */
extern wchar_t sGuangXu[STR_LEN];
extern wchar_t sXuanTong[STR_LEN];
extern wchar_t sMinGuo[STR_LEN];
extern wchar_t sGongYuan[STR_LEN];
extern wchar_t sYuan[STR_LEN];
extern wchar_t sNian[STR_LEN];

/* 月份字符串 */
extern wchar_t sLeapMonth[STR_LEN];
extern wchar_t sMonthName[12][3];
extern wchar_t sMonthStr[STR_LEN];
extern wchar_t sBigMonth[STR_LEN];
extern wchar_t sLittleMonth[STR_LEN];

/* 日期字符串 */
extern wchar_t nStr1[11][2];
extern wchar_t nStr2[4][2];

/* 星期字符串 */
extern wchar_t sWeekName[7][2];
extern wchar_t sWeekStr[STR_LEN];

/* 初始化字符串表 */
void CalInitStrings(HINSTANCE hInst);

/* 根据农历日期取字符串 */
void GetLunarDayStr(int day, wchar_t* sDay);

/* 返回历法信息 */
int GetCalendarInfo(ODATE *date, OTIME *time, OCALENDAR_INFO *info);

#if defined(__cplusplus)
}
#endif 

#endif /* OCALINFOW_H */