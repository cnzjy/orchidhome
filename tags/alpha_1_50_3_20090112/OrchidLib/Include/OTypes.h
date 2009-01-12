/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：公共类型定义头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OTypes.h,v 1.1 2006/03/13 10:50:42 zjy Exp $
* 备    注：
*******************************************************************************/

#ifndef OTYPES_H
#define OTYPES_H

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* 日期结构 */
typedef struct ODATE_struct {
  int year;
  int month;
  int day;
} ODATE;

/* 时间结构 */
typedef struct OTIME_struct {
  int hour;
  int minute;
  int second;
} OTIME;

#if defined(__cplusplus)
}
#endif 

#endif // OTYPES_H