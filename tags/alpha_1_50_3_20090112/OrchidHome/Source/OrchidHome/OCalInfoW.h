/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ���������ͷ�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OCalInfoW.h,v 1.6 2006/06/02 10:02:47 zjy Exp $
* ��    ע��
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

/* ������Ϣ�ṹ */
typedef struct OCALENDAR_INFO_struct {
  ODATE date;                       /* �������� */
  OTIME time;                       /* ����ʱ�� */

  int LeapYear;                     /* �Ƿ����� */
  int Week;                         /* ���� 0-6 (������Ϊ0) */
  int WeekOfYear;                   /* �ڸ����еĵڼ������� */
  int WeekOfMonth;                  /* �ڸ����еĵڼ������� */
                                    
  OLUNAR lunar;                     /* �������� */
                                   
  int JieQi;                        /* ������� 0-23������Ϊ -1 */
  int XingZuo;                      /* ������� 0-11 */
  int ShengXiao;                    /* ��Ф��� */
                                   
  int gzYear;                       /* ��ݸ�֧ */
  int gzMonth;                      /* �·ݸ�֧ */
  int gzDay;                        /* ���ڸ�֧ */
  int gzHour;                       /* ʱ���֧ */

  wchar_t sYear[MAX_DATE_LEN * 3];  /* �������ַ��� (��:��Ԫ2005��) */
  wchar_t sMonth[MAX_DATE_LEN];     /* ũ�����ַ��� */
  wchar_t sLeapMonth[MAX_DATE_LEN]; /* ũ������ */
  wchar_t sShortMonth[MAX_DATE_LEN];/* ũ���̸�ʽ���ַ��� */
  wchar_t sMonthLen[MAX_DATE_LEN];  /* ũ���´��С */
  wchar_t sDay[MAX_DATE_LEN];       /* ũ�����ַ��� */
  wchar_t sWeek[MAX_DATE_LEN];      /* �����ַ��� */

  wchar_t sJieQi[MAX_DATE_LEN];     /* ���� */
  wchar_t sXingZuo[MAX_DATE_LEN];   /* ���� */
  wchar_t sShengXiao[MAX_DATE_LEN]; /* ��Ф�ַ��� */

  wchar_t sgzYear[MAX_DATE_LEN];    /* ��ݸ�֧ */
  wchar_t sgzMonth[MAX_DATE_LEN];   /* �·ݸ�֧ */
  wchar_t sgzDay[MAX_DATE_LEN];     /* ���ڸ�֧ */
  wchar_t sgzHour[MAX_DATE_LEN];    /* ʱ���֧ */
} OCALENDAR_INFO;

/* ��� */
extern wchar_t sGan[10][2];

/* ��֧ */
extern wchar_t sZhi[12][2];

/* ��Ф */
extern wchar_t sShengXiao[12][2];

extern wchar_t sXingZuo[12][3];
extern wchar_t sZuo[STR_LEN];

/* ���� */
extern wchar_t sJieQi[24][3];

/* ����ַ��� */
extern wchar_t sGuangXu[STR_LEN];
extern wchar_t sXuanTong[STR_LEN];
extern wchar_t sMinGuo[STR_LEN];
extern wchar_t sGongYuan[STR_LEN];
extern wchar_t sYuan[STR_LEN];
extern wchar_t sNian[STR_LEN];

/* �·��ַ��� */
extern wchar_t sLeapMonth[STR_LEN];
extern wchar_t sMonthName[12][3];
extern wchar_t sMonthStr[STR_LEN];
extern wchar_t sBigMonth[STR_LEN];
extern wchar_t sLittleMonth[STR_LEN];

/* �����ַ��� */
extern wchar_t nStr1[11][2];
extern wchar_t nStr2[4][2];

/* �����ַ��� */
extern wchar_t sWeekName[7][2];
extern wchar_t sWeekStr[STR_LEN];

/* ��ʼ���ַ����� */
void CalInitStrings(HINSTANCE hInst);

/* ����ũ������ȡ�ַ��� */
void GetLunarDayStr(int day, wchar_t* sDay);

/* ����������Ϣ */
int GetCalendarInfo(ODATE *date, OTIME *time, OCALENDAR_INFO *info);

#if defined(__cplusplus)
}
#endif 

#endif /* OCALINFOW_H */