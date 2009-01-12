/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ���������ͷ�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OCalInfo.h,v 1.1 2006/03/19 15:27:20 zjy Exp $
* ��    ע��
*******************************************************************************/

#ifndef OCALINFO_H
#define OCALINFO_H

#include "OTypes.h"
#include "OCalendar.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define MAX_DATE_LEN  16

/* ������Ϣ�ṹ */
typedef struct OCALENDAR_INFO_struct {
  ODATE date;                      /* �������� */
  OTIME time;                      /* ����ʱ�� */

  int LeapYear;                    /* �Ƿ����� */
  int Week;                        /* ���� 0-6 (������Ϊ0) */
  int WeekOfYear;                  /* �ڸ����еĵڼ������� */
  int WeekOfMonth;                 /* �ڸ����еĵڼ������� */

  OLUNAR lunar;                    /* �������� */

  int JieQi;                       /* ������� 0-23������Ϊ -1 */
  int XingZuo;                     /* ������� 0-11 */
  int ShengXiao;                   /* ��Ф��� */

  int gzYear;                      /* ��ݸ�֧ */
  int gzMonth;                     /* �·ݸ�֧ */
  int gzDay;                       /* ���ڸ�֧ */
  int gzHour;                      /* ʱ���֧ */

  char sYear[MAX_DATE_LEN+1];      /* �������ַ��� (��:��Ԫ2005��) */
  char sMonth[MAX_DATE_LEN+1];     /* ũ�����ַ��� */
  char sDay[MAX_DATE_LEN+1];       /* ũ�����ַ��� */
  char sWeek[MAX_DATE_LEN+1];      /* �����ַ��� */

  char sJieQi[MAX_DATE_LEN+1];     /* ���� */
  char sXingZuo[MAX_DATE_LEN+1];   /* ���� */
  char sShengXiao[MAX_DATE_LEN+1]; /* ��Ф�ַ��� */

  char sgzYear[MAX_DATE_LEN+1];    /* ��ݸ�֧ */
  char sgzMonth[MAX_DATE_LEN+1];   /* �·ݸ�֧ */
  char sgzDay[MAX_DATE_LEN+1];     /* ���ڸ�֧ */
  char sgzHour[MAX_DATE_LEN+1];    /* ʱ���֧ */
} OCALENDAR_INFO;

/* ����������Ϣ */
int GetCalendarInfo(ODATE *date, OTIME *time, OCALENDAR_INFO *info);

#if defined(__cplusplus)
}
#endif 

#endif /* OCALINFO_H */