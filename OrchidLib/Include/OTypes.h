/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ��������Ͷ���ͷ�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OTypes.h,v 1.1 2006/03/13 10:50:42 zjy Exp $
* ��    ע��
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

/* ���ڽṹ */
typedef struct ODATE_struct {
  int year;
  int month;
  int day;
} ODATE;

/* ʱ��ṹ */
typedef struct OTIME_struct {
  int hour;
  int minute;
  int second;
} OTIME;

#if defined(__cplusplus)
}
#endif 

#endif // OTYPES_H