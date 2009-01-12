/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：日出日落时间计算头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OSunRiseSet.h,v 1.3 2006/06/02 08:40:41 zjy Exp $
* 备    注：
*******************************************************************************/

#ifndef OSUNRISESET_H
#define OSUNRISESET_H

#include "OTypes.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum SUNTYPE_enum {
  stNormal,
  stAllwaysUp, 
  stAllwaysDown, 
  stError
} SUNTYPE;

typedef struct SUNTIME_struct {
  OTIME lighttime;
  OTIME risetime;
  OTIME transittime;
  OTIME settime;
  OTIME darktime;
} SUNTIME;

/* 计算日出日落时间 */
SUNTYPE GetSunRiseSetTime(int year, int month, int day, double longitude, 
                          double latitude, int zonetime, SUNTIME *suntime);

#if defined(__cplusplus)
}
#endif 

#endif /* OSUNRISESET_H */