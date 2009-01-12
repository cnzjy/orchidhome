/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：历法函数头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OCalInfo.h,v 1.1 2006/03/19 15:27:20 zjy Exp $
* 备    注：
*******************************************************************************/

#ifndef OCALINFO_H
#define OCALINFO_H

#include "OTypes.h"
#include "OCalendar.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define MAX_DATE_LEN  16

/* 历法信息结构 */
typedef struct OCALENDAR_INFO_struct {
  ODATE date;                      /* 阳历日期 */
  OTIME time;                      /* 阳历时间 */

  int LeapYear;                    /* 是否闰年 */
  int Week;                        /* 星期 0-6 (星期日为0) */
  int WeekOfYear;                  /* 在该年中的第几个星期 */
  int WeekOfMonth;                 /* 在该月中的第几个星期 */

  OLUNAR lunar;                    /* 阴历日期 */

  int JieQi;                       /* 节气序号 0-23，不是为 -1 */
  int XingZuo;                     /* 星座序号 0-11 */
  int ShengXiao;                   /* 生肖序号 */

  int gzYear;                      /* 年份干支 */
  int gzMonth;                     /* 月份干支 */
  int gzDay;                       /* 日期干支 */
  int gzHour;                      /* 时间干支 */

  char sYear[MAX_DATE_LEN+1];      /* 中文年字符串 (如:公元2005年) */
  char sMonth[MAX_DATE_LEN+1];     /* 农历月字符串 */
  char sDay[MAX_DATE_LEN+1];       /* 农历日字符串 */
  char sWeek[MAX_DATE_LEN+1];      /* 星期字符串 */

  char sJieQi[MAX_DATE_LEN+1];     /* 节气 */
  char sXingZuo[MAX_DATE_LEN+1];   /* 星座 */
  char sShengXiao[MAX_DATE_LEN+1]; /* 生肖字符串 */

  char sgzYear[MAX_DATE_LEN+1];    /* 年份干支 */
  char sgzMonth[MAX_DATE_LEN+1];   /* 月份干支 */
  char sgzDay[MAX_DATE_LEN+1];     /* 日期干支 */
  char sgzHour[MAX_DATE_LEN+1];    /* 时间干支 */
} OCALENDAR_INFO;

/* 返回历法信息 */
int GetCalendarInfo(ODATE *date, OTIME *time, OCALENDAR_INFO *info);

#if defined(__cplusplus)
}
#endif 

#endif /* OCALINFO_H */