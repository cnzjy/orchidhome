/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：提醒项处理头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OAlarm.h,v 1.3 2006/06/01 09:51:31 zjy Exp $
* 备    注：
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
/* 提醒类                                                               */
/************************************************************************/

// 提醒类型
#define atAlarm       0   // 定时提醒
#define atExecute     1   // 执行程序
#define atFlightMode  2   // 飞行模式
#define atSilentMode  3   // 静音模式
#define atRestart     4   // 重新启动
#define atPowerOff    5   // 关机
#define atChime       6   // 整点报时
#define MAX_ALARMTYPE 6

// 重复提醒类型
#define arOnce        0   // 一次
#define arDaily       1   // 每天
#define arWeekDays    2   // 工作日
#define arWeekEnds    3   // 周末
#define arWeekly      4   // 每周
#define arMonthly     5   // 每月
#define arAnnually    6   // 每年
#define arInterval    7   // 每隔天数
#define MAX_RECURTYPE 7

// 提示方式
#define amRing        0   // 响铃
#define amVibrate     1   // 振动
#define amBoth        2   // 响铃加振动
#define amVibrateFirst 3  // 先振动后响铃
#define MAX_ALARMMODE 3


// 重复类型
struct RecurInfo
{
  int type;  // 重复类型
  bool weeks[7];
  int month;
  int day;
  int interval;
};

// 提醒项类型
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

// 提醒类
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
/* 提醒列表类                                                           */
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
