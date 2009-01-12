/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：插件数据类单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OHomeData.h,v 1.60 2006/06/08 06:01:09 zjy Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <wchar.h>
#include <tapi.h>
#include <projects.h>

#include "ORegistry.h"
#include "OHomeClasses.h"
#include "OHoliday.h"
#include "OAlarm.h"

extern HINSTANCE hInst;
extern HINSTANCE hRes;

#define NAME_LEN          32
#define TEXT_LEN          64

// 检查电量等参数的时间间隔
#define SECOND_DELAY      0
#define TIME_DELAY        1
#define DATA_DELAY        2
#define MAX_DELAY         3

#define SECOND_INTERVAL   250
#define TIME_INTERVAL     2000
#define DATA_INTERVAL     5000
#define MAX_INTERVAL      (60 * 1000)

// 全部分组号
#define GROUP_ALL         0

// 标签类型
#define NT_TEXT              0
#define NT_SPACE             1
#define NT_BR                2
#define NT_YEAR              3
#define NT_MONTH             4
#define NT_DAY               5
#define NT_CWEEK             6
#define NT_HOUR              7
#define NT_MINUTE            8
#define NT_SECOND            9
#define NT_LYEAR             10
#define NT_LMONTH            11
#define NT_LLEAPMONTH        12
#define NT_LSHORTMONTH       13
#define NT_LMONTHLEN         14
#define NT_LDAY              15
#define NT_GZYEAR            16
#define NT_GZMONTH           17
#define NT_GZDAY             18
#define NT_GZHOUR            19
#define NT_JIEQI             20
#define NT_XINGZUO           21
#define NT_SHENGXIAO         22
#define NT_HOLIDAY           23
#define NT_SHORTHOLIDAY      24
#define NT_DOWNTEXT          25
#define NT_DOWNDAY           26
#define NT_DOWNHOUR          27
#define NT_DOWNMINUTE        28
#define NT_DOWNSECOND        29
#define NT_LIGHTTIME         30
#define NT_RISETIME          31
#define NT_TRANSITTIME       32
#define NT_SETTIME           33
#define NT_DARKTIME          34
#define NT_MEMORYLOAD        35
#define NT_TOTALMEMORY       36
#define NT_FREEMEMORY        37
#define NT_STORELOAD         38
#define NT_TOTALSTORE        39
#define NT_FREESTORE         40
#define NT_STORAGELOAD       41
#define NT_TOTALSTORAGE      42
#define NT_FREESTORAGE       43
#define NT_STORAGECARDLOAD   44
#define NT_TOTALSTORAGECARD  45
#define NT_FREESTORAGECARD   46
#define NT_BATTERY           47
#define NT_TEMPERATURE       48
#define NT_VOLTAGE           49
#define NT_CURRENT           50
#define NT_AVERCURRENT       51
#define NT_SHORTCUTTITLE     52
#define NT_SHORTCUTDESC      53
#define NT_SHORTCUTCOUNT     54
#define NT_SHORTCUTINDEX     55
#define NT_SIGNAL            56
                             
#define NT_START             0
#define NT_END               56

// 需要更新的数据类型
#define DATA_NONE            (0)
#define DATA_DAY             (1 << 0)
#define DATA_HOUR            (1 << 1)
#define DATA_MINUTE          (1 << 2)
#define DATA_SECOND          (1 << 3)
#define DATA_CALENDAR        (1 << 4)
#define DATA_RISESET         (1 << 5)
#define DATA_MEM             (1 << 6)
#define DATA_MEMRATE         (1 << 7)
#define DATA_STORE           (1 << 8)
#define DATA_STORERATE       (1 << 9)
#define DATA_STORAGE         (1 << 10)
#define DATA_STORAGERATE     (1 << 11)
#define DATA_STORAGECARD     (1 << 12)
#define DATA_STORAGECARDRATE (1 << 13)
#define DATA_POWER           (1 << 14)
#define DATA_POWERRATE       (1 << 15)
#define DATA_LINEDEV         (1 << 16)
#define DATA_LINEDEVRATE     (1 << 17)
#define DATA_SHORTCUT        (1 << 18)
#define DATA_HOLIDAY         (1 << 19)
#define DATA_FORCE           (1 << 31)

/************************************************************************/
/* 通知器类                                                             */
/************************************************************************/

// 通知器类
class CHomeNotifier : public CPersistent
{
public:
  virtual void DataChanged(void* sender, uint mask) = 0;
  virtual void SetShortCutGroup(wchar_t* name, int group) = 0;
  virtual wchar_t* GetPluginName() = 0;
  virtual CShortCut* GetSelected() = 0;
};

/************************************************************************/
/* Home Screen 数据类                                                   */
/************************************************************************/

// 预声明
class CSysInfo;

// 数据类
class CHomeData
{
  friend CSysInfo;

  int m_cRef;

protected:
  uint m_datatick;
  uint m_sectick;
  uint m_mask;
  uint m_maskdelay;
  uint m_infointerval;
  CList* m_list;
  CList* m_riseset;
  HLINEAPP m_lineapp;
  HLINE m_line;
  DWORD m_numdevs;
  DWORD m_APIVersion;

  OCALENDAR_INFO m_calinfo;
  MEMORYSTATUS m_memstat;
  STORE_INFORMATION m_store;
  int m_storeload;
  STORAGE_INFO m_storage;
  STORAGE_INFO m_storagecard;
  SYSTEM_POWER_STATUS_EX2 m_power;
  LINEDEVSTATUS m_linestatus;
  int m_signal;
  
public:

  CHomeData();
  virtual ~CHomeData();

  static void InitInstance();
  static void ReleaseInstance();

  int AddNotifier(CHomeNotifier* notifier);
  void RemoveNotifier(CHomeNotifier* notifier);

  int AddRiseSet(CTextNode* node);
  void RemoveRiseSet(CTextNode* node);

  uint GetMask(int labelkind);
  void AddMask(uint mask);

  uint GetInfoInterval();
  void SetInfoInterval(uint value);

  void GetFullPathFileName(wchar_t* &filename);

  void CheckData(bool forceupdate);
  void GetTextNodeValue(CTextNode* pText, wchar_t* text, int slen);

  void GetSunTimeStr(SUNTYPE suntype, OTIME* time, wchar_t* text);
  void GetTextNodeStrValue(CTextNode* pText, wchar_t* s, wchar_t* text, int slen);
  void GetTextNodeIntValue(CTextNode* pText, int val, wchar_t* text, int slen);
  void GetHolidayStr(OHOLIDAY_ITEM* item, bool shortfmt, bool incdate, wchar_t* text, int slen);
  void GetShortHolidayStr(CTextNode* pText, wchar_t* text, int slen);
  void GetHolidayStrValue(CTextNode* pText, wchar_t* text, int slen);
  CDownCounter* GetDownCounterByName(wchar_t* name);
  wchar_t* GetDownCounterStr(CTextNode* pText);
  int GetDownCounterValue(CTextNode* pText);

  void SetShortCutGroup(wchar_t* name, int group);
  void NotifyPlugins(void* sender, uint mask);

  void Initialize();

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);

  void LoadHoliday();
  void SaveHoliday();

  void LoadAlarms();
  void SaveAlarms();

  void LoadSettings();
  void SaveSettings();

  void AddToolToList(int cmd, wchar_t* title, wchar_t* target, wchar_t* args, bool confirm = false);

  CSysInfo* sysinfo;
  CHomeRegistry* registry;

  CHolidayList* holiday;
  CHolidayList* userday;
  CHolidayItems* holidayitems;

  CAlarmList* alarms;
  CList* tools;
  CList* counters;

  CDownCounter* downcounter;
  bool downenabled;

  GEO_INFO geo;
  bool temp_fix;
  int temp_adjust;
  uint interval;
  int iconsize;
  int smiconsize;
  int alldays;
  int holidaylevel;
  LANGID lid;
  SYSTEMTIME m_time;

  wchar_t m_plugindir[MAX_PATH];
  wchar_t m_homedir[MAX_PATH];
  wchar_t m_storagedir[NAME_LEN];
  wchar_t m_storagecarddir[NAME_LEN];

protected:
  void GetSysDirs();
  void LoadResDll();
  void FreeResDll();
  void CalcRiseSet(CTextNode* pText);
  void ProcTextNodeValue(CTextNode* pText, wchar_t* s, wchar_t* text, int slen);
  CShortCut* GetPluginShortCut(wchar_t* name);
  bool InitLineApp();
  void FreeLineApp();
  void UpdateHolidayItems();
  uint GetMaskDelay(uint mask);
  int FixTemperature(int temp);
  void AddToolFromRes(int cmd, UINT resid, bool confirm = false);
  void InitTools();
  void ClearToolList();
  void ClearCounterList();
  void UpdateInterval();

};

extern CHomeData* HomeData;
