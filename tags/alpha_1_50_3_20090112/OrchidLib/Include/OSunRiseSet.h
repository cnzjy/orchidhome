/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ��ճ�����ʱ�����ͷ�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OSunRiseSet.h,v 1.3 2006/06/02 08:40:41 zjy Exp $
* ��    ע��
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

/* �����ճ�����ʱ�� */
SUNTYPE GetSunRiseSetTime(int year, int month, int day, double longitude, 
                          double latitude, int zonetime, SUNTIME *suntime);

#if defined(__cplusplus)
}
#endif 

#endif /* OSUNRISESET_H */