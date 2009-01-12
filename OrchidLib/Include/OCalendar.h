/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ���������ͷ�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OCalendar.h,v 1.12 2006/06/02 08:40:41 zjy Exp $
* ��    ע��
*******************************************************************************/

#ifndef OCALENDAR_H
#define OCALENDAR_H

#include "OTypes.h"

#if defined(__cplusplus)
extern "C" {
#endif

// ũ��������ʼ���
#define LUNAR_START_YEAR 1900
#define LUNAR_END_YEAR   2050

typedef enum CALENDER_TYPE_enum {
  ctInvalid,
  ctJulian,
  ctGregorian
} CALENDER_TYPE;

/* ũ���ṹ */
typedef struct OLUNAR_struct {
  int year;                        /* ũ���� */
  int month;                       /* ũ���� */
  int day;                         /* ũ���� */
  int leap;                        /* �Ƿ����� */
  int little;                      /* �Ƿ�С��29�� */
} OLUNAR;

/************************************************************************/
/* ��������                                                             */
/************************************************************************/

/* ����ɵ�֧��ϳɸ�֧��0-9 0-11 ת���� 0-59 */
int CombineGanZhi(int Gan, int Zhi);

/* ����֧��ֳ���ɵ�֧��0-59 ת���� 0-9 0-11 */
int ExtractGanZhi(int GanZhi, int *Gan, int *Zhi);

/* ���ݹ��������жϵ�ʱ���� */
CALENDER_TYPE GetCalendarType(int AYear, int AMonth, int ADay);

/* ����ĳ�����Ƿ����� */
int GetIsLeapYear(int AYear);

/* ȡ���������������� 1582 �� 10 �µ�������� */
int GetMonthDays(int AYear, int AMonth);

/* ���ع��������Ƿ�Ϸ� */
int GetDateIsValid(int AYear, int AMonth, int ADay);

/* ����ʱ���Ƿ�Ϸ� */
int GetTimeIsValid(int AHour, int AMinute, int ASecond);

/* ȡĳ���ڵ������������������ 1582 �� 10 �µ�������� */
int GetDayFromYear(int AYear, int AMonth, int ADay);

/* �Ӿ��������������º���������������ж��Ƿ������꣬���� False ��ʾ���Ϸ����� */
int ExtractMonthDay(int Days, int AYear, int *AMonth, int *ADay);

/* ��þ๫Ԫԭ������� */
int GetAllDays(int AYear, int AMonth, int ADay);

/* ��ȡ��Ч��׼�������˸���ϵ��ֲ�������ƺ��Ǿ� Gregorian ��Ԫ��Ԫ�������� */
int GetEquStandardDays(int AYear, int AMonth, int ADay);

/* ���ĳ���������ڼ���0-6 */
int GetWeek(int AYear, int AMonth, int ADay);

/* ��þ�1899-12-31�յ����� */
int EncodeDate(int AYear, int AMonth, int ADay, int* ADate);

/* ���ݾ�1899-12-31�յ��������������� */
int DecodeDate(int ADate, int* AYear, int* AMonth, int* ADay);

/* ����ƫ�� */
int OffsetDate(ODATE* date, int offset);

/* ��ü��µڼ������ڼ������ڣ���� AWeek >= 5 ��ʾ���һ�����ڼ� */
int GetDateFromMonthWeek(int AYear, int AMonth, int AWeek, int AWeekDay, int* ADay);

/* ���ĳ�������ڵĵ� N �������������������1-24����ӦС�������� */
double GetJieQiDayTimeFromYear(int AYear, int N);

/* ���ĳ������ĵ� N �������Ľ�������ʱ�֣�1-24����ӦС�������� */
int GetJieQiInAYear(int AYear, int N, int *AMonth, int *ADay, int *AHour, int *AMinute);

/* ��ù����������Ǳ����ʲô������1-24����ӦС�������������򷵻� -1 */
int GetJieQiFromDay(int AYear, int AMonth, int ADay);

/* ��ù������µ�����������Ӧ�����ںͽ�����š�1-24����ӦС�������� */
void GetJieQiFromMonth(int AYear, int AMonth, int *Day1, int *JieQi1, int *Day2, int *JieQi2);

/* ���ݹ��������ջ��ĳ���������ɵ�֧��������Ϊ��ֽ磬0-59 ��Ӧ ���ӵ��ﺥ */
int GetGanZhiFromYear(int AYear, int AMonth, int ADay);

/* ���ĳ�����µ���ɵ�֧��0-59 ��Ӧ ���ӵ��ﺥ */
int GetGanZhiFromMonth(int AYear, int AMonth, int ADay);

/* ���ĳ�����յ���ɵ�֧��0-59 ��Ӧ ���ӵ��ﺥ */
int GetGanZhiFromDay(int AYear, int AMonth, int ADay);

/* ���ĳ����ʱ����ɵ�֧��0-59 ��Ӧ ���ӵ��ﺥ */
int GetGanZhiFromHour(int AYear, int AMonth, int ADay, int AHour);

/* ���ĳ��/ũ�������ФҲ���ǵ�֧��0-11 ��Ӧ ���� */
int GetShengXiaoFromYear(int AYear);

/* ���ĳ�������յ�������0-11 ��Ӧ ����˫�� */
int GetXingZuoFromMonthDay(int AMonth, int ADay);

/* ��ù������������������еĵڼ��ŵĵڼ��գ�1~9,1~9��Ӧһ�ŵ��žţ�False Ϊ������������ */
int GetShu9Day(int AYear, int AMonth, int ADay, int *JiuSeq, int *JiuDay);

/* ��ù������������������еĵڼ����ĵڼ��գ�1~3,1~10��Ӧ������ĩ�����գ�False Ϊ���ڷ����� */
int Get3FuDay(int AYear, int AMonth, int ADay, int *Fu, int *FuDay);

/* ���ĳ�������е���÷���ڣ�÷�꼾�ڵĿ�ʼ�գ�â�ֺ�ĵ�һ������ */
int GetRuMeiDay(int AYear, int *AMonth, int *ADay);

/* ���ĳ�������еĳ�÷���ڣ�÷�꼾�ڵĽ����գ�С���ĵ�һ��δ�� */
int GetChuMeiDay(int AYear, int *AMonth, int *ADay);


/************************************************************************/
/* ũ������                                                             */
/************************************************************************/

/* ����ũ����Ϣ */
int GetLunarInfo(ODATE *date, OLUNAR *lunar);

/* ����ũ�����㹫������ */
int LunarToDate(OLUNAR *lunar, ODATE *date);

#if defined(__cplusplus)
}
#endif 

#endif /* OCALENDAR_H */