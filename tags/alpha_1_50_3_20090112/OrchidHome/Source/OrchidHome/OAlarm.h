/*******************************************************************************
* ������ƣ�Orchid Mobile Software Home Plugin
* �ļ����ƣ��������ͷ�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OAlarm.h,v 1.3 2006/06/01 09:51:31 zjy Exp $
* ��    ע��
*******************************************************************************/

#pragma once

#include <windows.h>
#include <memory.h>
#include <msxml.h>
#include <ocidl.h>
#include "OTypes.h"
#include "OList.h"
#include "OHomeClasses.h"

/************************************************************************/
/* ������                                                               */
/************************************************************************/

// ��������
#define atAlarm       0   // ��ʱ����
#define atExecute     1   // ִ�г���
#define atFlightMode  2   // ����ģʽ
#define atSilentMode  3   // ����ģʽ
#define atRestart     4   // ��������
#define atPowerOff    5   // �ػ�
#define atChime       6   // ���㱨ʱ
#define MAX_ALARMTYPE 6

// �ظ���������
#define arOnce        0   // һ��
#define arDaily       1   // ÿ��
#define arWeekDays    2   // ������
#define arWeekEnds    3   // ��ĩ
#define arWeekly      4   // ÿ��
#define arMonthly     5   // ÿ��
#define arAnnually    6   // ÿ��
#define arInterval    7   // ÿ������
#define MAX_RECURTYPE 7

// ��ʾ��ʽ
#define amRing        0   // ����
#define amVibrate     1   // ��
#define amBoth        2   // �������
#define amVibrateFirst 3  // ���񶯺�����
#define MAX_ALARMMODE 3


// �ظ�����
struct RecurInfo
{
  int type;  // �ظ�����
  bool weeks[7];
  int month;
  int day;
  int interval;
};

// ����������
struct OALARM
{
  bool enabled;
  int alarmType;
  int alarmMode;
  RecurInfo recurInfo;
  OTIME timeStart;
  OTIME timeEnd;
  ODATE dateStart;
  int ringCount;
  int volume;
};

// ������
class CAlarm : public CPersistent
{
public:
  CAlarm();
  ~CAlarm();

  OALARM data;
  wchar_t* text;
  wchar_t* sndfile;
  wchar_t* target;
  wchar_t* args;

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
  void Clone(CAlarm* source);
};

/************************************************************************/
/* �����б���                                                           */
/************************************************************************/

class CAlarmList : public CPersistent
{
public:
  CAlarmList();
  ~CAlarmList();

  bool LoadFromXML(IXMLDOMDocument* xml);
  bool SaveToXML(IXMLDOMDocument* xml);

  bool LoadFromFile(wchar_t* filename);
  bool SaveToFile(wchar_t* filename);

  CAlarm* Get(int i);
  int Count();
  int Add(CAlarm* item);
  void Insert(int Index, CAlarm* Item);
  void Delete(int index);
  void Clear();

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);

protected:
  CList* m_list;

  HRESULT ReadItem(IXMLDOMNode* node, CAlarm* item);
  HRESULT SaveItem(IXMLDOMDocument* xml, IXMLDOMNode* node, CAlarm* item);
  HRESULT AppendTextNode(IXMLDOMDocument* xml, IXMLDOMElement* element, wchar_t* text);

private:
};
