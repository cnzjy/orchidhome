/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：数据处理单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OHomeData.cpp,v 1.73 2006/06/08 09:29:29 zjy Exp $
* 备    注：
*******************************************************************************/

#include "OHomeData.h"
#include "OSysInfo.h"
#include <shlobj.h>

/************************************************************************/
/* 常量变量定义                                                         */
/************************************************************************/

// DLL 句柄
HINSTANCE hInst = 0;
HINSTANCE hRes = 0;

// 全局对象实例
CHomeData* HomeData = NULL;

// 其它字符串
static wchar_t* sStorageDir = L"\\Storage";
static wchar_t* sStorageCardDir = L"\\Storage Card";
static wchar_t* sWindowsPath = L"\\Windows\\";
static wchar_t* sRegBase = L"\\Software\\Orchid\\Home\\";

static wchar_t* sHoliday = L"holiday.xml";
static wchar_t* sUserDay = L"userday.xml";
static wchar_t* sAlarms = L"alarms.xml";

static wchar_t* sOption = L"Option";
static wchar_t* sLongitude = L"Longitude";
static wchar_t* sLatitude = L"Latitude";
static wchar_t* sLangID = L"LangID";
static wchar_t* sZoneTime = L"ZoneTime";
static wchar_t* sTempFix = L"TempFix";
static wchar_t* sTempAdjust = L"TempAdjust";
static wchar_t* sHolidayLevel = L"HolidayLevel";
static wchar_t* sInfoInterval = L"InfoInterval";

static wchar_t* sCounter = L"Counter";
static wchar_t* sEnabled = L"Enabled";
static wchar_t* sText = L"Text";
static wchar_t* sDate = L"Date";
static wchar_t* sTime = L"Time";

// TAPI 版本
#define TAPI_VERSION_1_0      0x00010003

// 标签类型对应的数据类型
uint NodeMasks[NT_END - NT_START + 1] = {
  DATA_NONE,           // NT_TEXT
  DATA_NONE,           // NT_SPACE
  DATA_NONE,           // NT_BR
  DATA_DAY,            // NT_YEAR
  DATA_DAY,            // NT_MONTH
  DATA_DAY,            // NT_DAY
  DATA_DAY,            // NT_CWEEK
  DATA_HOUR,           // NT_HOUR
  DATA_MINUTE,         // NT_MINUTE
  DATA_SECOND,         // NT_SECOND
  DATA_CALENDAR,       // NT_LYEAR
  DATA_CALENDAR,       // NT_LMONTH
  DATA_CALENDAR,       // NT_LLEAPMONTH
  DATA_CALENDAR,       // NT_LSHORTMONTH
  DATA_CALENDAR,       // NT_LMONTHLEN
  DATA_CALENDAR,       // NT_LDAY
  DATA_CALENDAR,       // NT_GZYEAR
  DATA_CALENDAR,       // NT_GZMONTH
  DATA_CALENDAR,       // NT_GZDAY
  DATA_CALENDAR,       // NT_GZHOUR
  DATA_CALENDAR,       // NT_JIEQI
  DATA_CALENDAR,       // NT_XINGZUO
  DATA_CALENDAR,       // NT_SHENGXIAO
  DATA_HOLIDAY,        // NT_HOLIDAY
  DATA_HOLIDAY,        // NT_SHORTHOLIDAY
  DATA_NONE,           // NT_DOWNTEXT
  DATA_DAY,            // NT_DOWNDAY
  DATA_HOUR,           // NT_DOWNHOUR
  DATA_MINUTE,         // NT_DOWNMINUTE
  DATA_SECOND,         // NT_DOWNSECOND
  DATA_RISESET,        // NT_LIGHTTIME
  DATA_RISESET,        // NT_RISETIME
  DATA_RISESET,        // NT_TRANSITTIME
  DATA_RISESET,        // NT_SETTIME
  DATA_RISESET,        // NT_DARKTIME
  DATA_MEMRATE,        // NT_MEMORYLOAD
  DATA_MEM,            // NT_TOTALMEMORY
  DATA_MEM,            // NT_FREEMEMORY
  DATA_STORERATE,      // NT_STORELOAD
  DATA_STORE,          // NT_TOTALSTORE
  DATA_STORE,          // NT_FREESTORE
  DATA_STORAGERATE,    // NT_STORAGELOAD
  DATA_STORAGE,        // NT_TOTALSTORAGE
  DATA_STORAGE,        // NT_FREESTORAGE
  DATA_STORAGECARDRATE,// NT_STORAGECARDLOAD
  DATA_STORAGECARD,    // NT_TOTALSTORAGECARD
  DATA_STORAGECARD,    // NT_FREESTORAGECARD
  DATA_POWERRATE,      // NT_BATTERY
  DATA_POWERRATE,      // NT_TEMPERATURE
  DATA_POWER,          // NT_VOLTAGE
  DATA_POWER,          // NT_CURRENT
  DATA_POWER,          // NT_AVERCURRENT
  DATA_SHORTCUT,       // NT_SHORTCUTNAME
  DATA_SHORTCUT,       // NT_SHORTCUTDESC
  DATA_SHORTCUT,       // NT_SHORTCUTCOUNT
  DATA_SHORTCUT,       // NT_SHORTCUTINDEX
  DATA_LINEDEVRATE     // NT_SIGNAL
};

// 数据类型对应的定时器时间
uint DataDelays[19] = {
  TIME_DELAY,      // DATA_DAY
  TIME_DELAY,      // DATA_HOUR
  TIME_DELAY,      // DATA_MINUTE
  SECOND_DELAY,    // DATA_SECOND
  TIME_DELAY,      // DATA_CALENDAR
  TIME_DELAY,      // DATA_RISESET
  DATA_DELAY,      // DATA_MEM
  DATA_DELAY,      // DATA_MEMRATE
  DATA_DELAY,      // DATA_STORE
  DATA_DELAY,      // DATA_STORERATE
  DATA_DELAY,      // DATA_STORAGE
  DATA_DELAY,      // DATA_STORAGERATE
  DATA_DELAY,      // DATA_STORAGECARD
  DATA_DELAY,      // DATA_STORAGECARDRATE
  DATA_DELAY,      // DATA_POWER
  DATA_DELAY,      // DATA_POWERRATE
  DATA_DELAY,      // DATA_LINEDEV
  DATA_DELAY,      // DATA_LINEDEVRATE
  MAX_DELAY        // DATA_SHORTCUT
};

/************************************************************************/
/* DLL 入口函数                                                         */
/************************************************************************/

BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
  if (!hInst)
    hInst = (HINSTANCE)hinstDLL;
  return TRUE;
}

/************************************************************************/
/* CHomeData 类实现                                                     */
/************************************************************************/

void CHomeData::InitInstance()
{
  if (HomeData == NULL)
    HomeData = new CHomeData;
  else
    HomeData->m_cRef++;
}

void CHomeData::ReleaseInstance()
{
  if (HomeData != NULL)
  {
    HomeData->m_cRef--;
    if (HomeData->m_cRef <= 0)
    {
      delete HomeData;
      HomeData = NULL;
    }
  }
}

CHomeData::CHomeData()
{
  DEBUGLOG1(L"CHomeData create: %x", (int)this);

  m_cRef = 1;
  m_mask = 0;
  m_datatick = 0;
  m_sectick = 0;
  m_lineapp = NULL;
  m_line = NULL;
  m_numdevs = 0;
  m_APIVersion = 0;

  // 默认手机温度除 10
  temp_fix = true;
  temp_adjust = -3;

  // 默认显示全部节日
  holidaylevel = 3;

  memset(&m_time, 0, sizeof(m_time));
  memset(&m_calinfo, 0, sizeof(m_calinfo));
  memset(&m_memstat, 0, sizeof(m_memstat));
  memset(&m_store, 0, sizeof(m_store));
  memset(&m_storage, 0, sizeof(m_storage));
  memset(&m_storagecard, 0, sizeof(m_storagecard));
  memset(&m_power, 0, sizeof(m_power));
  memset(&m_linestatus, 0, sizeof(m_linestatus));

  // 地理位置默认为北京
  geo.longitude = 116.4;
  geo.latitude = 39.92;
  TIME_ZONE_INFORMATION zoneinfo;
  memset(&zoneinfo, 0, sizeof(zoneinfo));
  GetTimeZoneInformation(&zoneinfo);
  geo.zonetime = -(zoneinfo.Bias / 60);

  m_storeload = 0;
  m_signal = 0;

  alldays = 0;
  m_infointerval = DATA_INTERVAL;
  interval = MAX_DELAY;
  iconsize = GetSystemMetrics(SM_CXICON);
  smiconsize = GetSystemMetrics(SM_CXSMICON);
  GetSysDirs();

  registry = new CHomeRegistry(sRegBase);
  lid = GetUserDefaultUILanguage();
  int langid = lid;
  registry->ReadInt(sOption, sLangID, langid);
  lid = langid;

  FreeResDll();
  LoadResDll();

  m_list = new CList;
  counters = new CList;
  m_riseset = new CList;

  sysinfo = new CSysInfo;

  holiday = new CHolidayList(lid);
  userday = new CHolidayList(lid);
  holidayitems = new CHolidayItems;

  alarms = new CAlarmList;

  tools = new CList;

  downcounter = new CDownCounter;
  downenabled = false;

  InitStringTable(hRes);

  LoadSettings();
}

CHomeData::~CHomeData()
{
  DEBUGLOG(L"CHomeData destroy");

  delete m_list;
  delete m_riseset;

  delete sysinfo;
  delete registry;

  delete holiday;
  delete userday;
  delete holidayitems;

  delete alarms;

  ClearCounterList();
  delete downcounter;

  ClearToolList();
  delete tools;

  FreeLineApp();

  FreeStringTable();

  FreeResDll();
}

// 装载资源 DLL
void CHomeData::LoadResDll()
{
  if (!hRes)
  {
    wchar_t dllname[MAX_PATH], muiname[MAX_PATH];
    GetModuleFileName(hInst, dllname, LENOF(dllname));
    wsprintf(muiname, L"%s.%04x.mui", dllname, lid);
    DEBUGLOG1(L"Resource dll name: %s", muiname);
    hRes = LoadLibrary(muiname);
    if (!hRes)
    {
      DEBUGLOG(L"Load resource dll fail");
      hRes = hInst;
    }
  }
}

// 释放资源 DLL
void CHomeData::FreeResDll()
{
  if (hRes && (hRes != hInst))
  {
    FreeLibrary(hRes);
    hRes = NULL;
  }
}

// 取得系统路径
void CHomeData::GetSysDirs()
{
  WIN32_FIND_DATA fd;
  HANDLE hFind;

  // 取得插件路径
  memset(m_plugindir, 0, sizeof(m_plugindir));
  GetModuleFileName(hInst, m_plugindir, LENOF(m_plugindir));
  DEBUGLOG1(L"Plugin dll name: %s", m_plugindir);
  for (int i = wcslen(m_plugindir) - 1; i; i--)
    if (m_plugindir[i] == '\\')
    {
      m_plugindir[i] = 0;
      break;
    }
  DEBUGLOG1(L"Plugin dir: %s", m_plugindir);

  // Home 路径
  SHGetSpecialFolderPath(0, m_homedir, CSIDL_APPDATA, false);
  wcscat(m_homedir, L"\\Home");
  DEBUGLOG1(L"Home dir: %s", m_homedir);
  
  // 取得手机存储卡的路径
  memset(m_storagedir, 0, sizeof(m_storagedir));
  memset(m_storagecarddir, 0, sizeof(m_storagecarddir));
  hFind = FindFirstFlashCard(&fd);
  if (hFind != INVALID_HANDLE_VALUE)
  {
    wsprintf(m_storagedir, L"\\%s", fd.cFileName);
    if (FindNextFlashCard(hFind, &fd))
      wsprintf(m_storagecarddir, L"\\%s", fd.cFileName);
    FindClose(hFind);
  }

  if (!m_storagedir[0])
    wcscpy(m_storagedir, sStorageDir);
  if (!m_storagecarddir[0])
    wcscpy(m_storagecarddir, sStorageCardDir);

  DEBUGLOG1(L"Storage dir: %s", m_storagedir);
  DEBUGLOG1(L"Storage Card dir: %s", m_storagecarddir);
}

// 装入节假日数据
void CHomeData::LoadHoliday()
{
  wchar_t buff[MAX_PATH];
  wsprintf(buff, L"%s\\%s", m_plugindir, sHoliday);
  holiday->LoadFromFile(buff);
  DEBUGLOG1(L"Load holiday: count = %d", holiday->Count());
  wsprintf(buff, L"%s\\%s", m_plugindir, sUserDay);
  userday->LoadFromFile(buff);
  DEBUGLOG1(L"Load user holiday: count = %d", userday->Count());
}

// 保存节假日数据
void CHomeData::SaveHoliday()
{
  wchar_t buff[MAX_PATH];
  wsprintf(buff, L"%s\\%s", m_plugindir, sUserDay);
  userday->SaveToFile(buff);
}

// 装入节假日数据
void CHomeData::LoadAlarms()
{
  wchar_t buff[MAX_PATH];
  wsprintf(buff, L"%s\\%s", m_plugindir, sAlarms);
  alarms->LoadFromFile(buff);
  DEBUGLOG1(L"Load alarms: count = %d", alarms->Count());
}

// 保存节假日数据
void CHomeData::SaveAlarms()
{
  wchar_t buff[MAX_PATH];
  wsprintf(buff, L"%s\\%s", m_plugindir, sAlarms);
  alarms->SaveToFile(buff);
}

// 装入初始化数据
void CHomeData::Initialize()
{
  DEBUGLOG(L"CHomeData::InitData");
  LoadHoliday();
  LoadAlarms();
}

// 读取设置
void CHomeData::LoadSettings()
{
  int value;
  value = (int)(geo.longitude * 100);
  if (registry->ReadInt(sOption, sLongitude, value))
    geo.longitude = ((double)value) / 100;
  value = (int)(geo.latitude * 100);
  if (registry->ReadInt(sOption, sLatitude, value))
    geo.latitude = ((double)value) / 100;
  registry->ReadInt(sOption, sZoneTime, geo.zonetime);

  registry->ReadBool(sOption, sTempFix, temp_fix);
  registry->ReadInt(sOption, sTempAdjust, temp_adjust);

  registry->ReadBool(sCounter, sEnabled, downenabled);
  registry->ReadString(sCounter, sText, downcounter->text);
  registry->ReadBlock(sCounter, sDate, &downcounter->date, sizeof(downcounter->date));
  registry->ReadBlock(sCounter, sTime, &downcounter->time, sizeof(downcounter->time));

  registry->ReadInt(sOption, sHolidayLevel, holidaylevel);
  if (holidaylevel > 3) holidaylevel = 3;
  if (holidaylevel < 0) holidaylevel = 0;
  registry->ReadInt(sOption, sInfoInterval, (int&)m_infointerval);
  UpdateInterval();
}

// 保存设置
void CHomeData::SaveSettings()
{
  int value;
  value = (int)(geo.longitude * 100);
  registry->WriteInt(sOption, sLongitude, value);
  value = (int)(geo.latitude * 100);
  registry->WriteInt(sOption, sLatitude, value);
  registry->WriteInt(sOption, sZoneTime, geo.zonetime);

  registry->WriteBool(sOption, sTempFix, temp_fix);
  registry->WriteInt(sOption, sTempAdjust, temp_adjust);

  registry->WriteBool(sCounter, sEnabled, downenabled);
  registry->WriteString(sCounter, sText, downcounter->text);
  registry->WriteBlock(sCounter, sDate, &downcounter->date, sizeof(downcounter->date));
  registry->WriteBlock(sCounter, sTime, &downcounter->time, sizeof(downcounter->time));

  registry->WriteInt(sOption, sHolidayLevel, holidaylevel);
  registry->WriteInt(sOption, sInfoInterval, m_infointerval);
}

// 从流中装入数据
HRESULT CHomeData::LoadFromStream(IStream* pStm)
{
  DEBUGLOG(L"CHomeData::LoadFromStream");
  FILETIME time1, time2;
  wchar_t buff[MAX_PATH];
  LANGID langid;
  int cnt;

  // 检查语言是否已修改
  if (FAILED(pStm->Read(&langid, sizeof(langid), 0)))
    return E_FAIL;
  if (langid != lid)
    return E_FAIL;

  // 如果当前已经有一个使用插件的主题，再切换新主题时，holidayitems 的内容需要清空
  // 以防止 holiday 列表清空后旧的主题更新界面时出错。
  holidayitems->Clear();

  // 检查节日文件日期是否已修改
  if (FAILED(pStm->Read(&time1, sizeof(time1), 0)))
    return E_FAIL;
  wsprintf(buff, L"%s\\%s", m_plugindir, sHoliday);
  GetFileWriteTime(buff, time2);
  if (CompareFileTime(&time1, &time2) != 0)
    return E_FAIL;

  if (FAILED(holiday->LoadFromStream(pStm)))
    return E_FAIL;

  // 检查用户节日文件日期是否已修改
  if (FAILED(pStm->Read(&time1, sizeof(time1), 0)))
    return E_FAIL;
  wsprintf(buff, L"%s\\%s", m_plugindir, sUserDay);
  GetFileWriteTime(buff, time2);
  if (CompareFileTime(&time1, &time2) != 0)
    return E_FAIL;

  if (FAILED(userday->LoadFromStream(pStm)))
    return E_FAIL;

  // 检查闹钟文件日期是否已修改
  if (FAILED(pStm->Read(&time1, sizeof(time1), 0)))
    return E_FAIL;
  wsprintf(buff, L"%s\\%s", m_plugindir, sAlarms);
  GetFileWriteTime(buff, time2);
  if (CompareFileTime(&time1, &time2) != 0)
    return E_FAIL;

  if (FAILED(alarms->LoadFromStream(pStm)))
    return E_FAIL;

  // DownCounter 列表
  ClearCounterList();
  if (FAILED(pStm->Read(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (; cnt; cnt--)
  {
    CDownCounter* item = new CDownCounter;
    if (FAILED(item->LoadFromStream(pStm)))
      return E_FAIL;
    counters->Add(item);
  }

  // 读取工具列表
  // todo: 从流中读取 tools 列表
  ClearToolList();
  InitTools();

  return S_OK;
}

// 保存数据到流
HRESULT CHomeData::SaveToStream(IStream* pStm)
{
  DEBUGLOG(L"CHomeData::SaveToStream");
  FILETIME time;
  wchar_t buff[MAX_PATH];
  int i, cnt;

  // 保存语言ID
  if (FAILED(pStm->Write(&lid, sizeof(lid), 0)))
    return E_FAIL;

  // 保存文件日期
  wsprintf(buff, L"%s\\%s", m_plugindir, sHoliday);
  GetFileWriteTime(buff, time);
  if (FAILED(pStm->Write(&time, sizeof(time), 0)))
    return E_FAIL;
  if (FAILED(holiday->SaveToStream(pStm)))
    return E_FAIL;

  // 保存文件日期
  wsprintf(buff, L"%s\\%s", m_plugindir, sUserDay);
  GetFileWriteTime(buff, time);
  if (FAILED(pStm->Write(&time, sizeof(time), 0)))
    return E_FAIL;
  if (FAILED(userday->SaveToStream(pStm)))
    return E_FAIL;

  // 保存文件日期
  wsprintf(buff, L"%s\\%s", m_plugindir, sAlarms);
  GetFileWriteTime(buff, time);
  if (FAILED(pStm->Write(&time, sizeof(time), 0)))
    return E_FAIL;
  if (FAILED(alarms->SaveToStream(pStm)))
    return E_FAIL;

  // DownCounter 列表
  cnt = counters->GetCount();
  if (FAILED(pStm->Write(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (i = 0; i < cnt; i++)
  {
    CDownCounter* item = (CDownCounter*)counters->Get(i);
    if (FAILED(item->SaveToStream(pStm)))
      return E_FAIL;
  }

  return S_OK;
}

VOID CALLBACK lineCallbackFunc(DWORD hDevice, DWORD dwMsg, DWORD dwCallbackInstance, 
                               DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
  // todo:
}

// 初始化线路
bool CHomeData::InitLineApp()
{
  if (!m_lineapp)
  {
    DEBUGLOG(L"lineInitialize");
    if (lineInitialize(&m_lineapp, hInst, (LINECALLBACK)lineCallbackFunc, NULL, &m_numdevs) != 0)
      return false;
  }

  if (!m_APIVersion)
  {
    DEBUGLOG(L"lineNegotiateAPIVersion");
    if (lineNegotiateAPIVersion(m_lineapp, 0, TAPI_VERSION_1_0, TAPI_CURRENT_VERSION, &m_APIVersion, NULL) != 0)
      return false;
  }

  if (!m_line)
  {
    DEBUGLOG(L"lineOpen");
    if (lineOpen(m_lineapp, 0, &m_line, m_APIVersion, 0, 0, LINECALLPRIVILEGE_NONE, 0, NULL) != 0)
      return false;
  }

  return true;
}

void CHomeData::FreeLineApp()
{
  if (m_line)
  {
    DEBUGLOG(L"lineClose");
    lineClose(m_line);
    m_line = NULL;
  }

  if (m_lineapp)
  {
    DEBUGLOG(L"lineShutdown");
    lineShutdown(m_lineapp);
    m_lineapp = NULL;
  }
}

int CHomeData::AddNotifier(CHomeNotifier* notifier)
{
  return m_list->Add(notifier);
}

void CHomeData::RemoveNotifier(CHomeNotifier* notifier)
{
  m_list->Remove(notifier);
}

int CHomeData::AddRiseSet(CTextNode* node)
{
  CalcRiseSet(node);
  return m_riseset->Add(node);
}

void CHomeData::RemoveRiseSet(CTextNode* node)
{
  m_riseset->Remove(node);
}

void CHomeData::NotifyPlugins(void* sender, uint mask)
{
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    ((CHomeNotifier*)m_list->Get(i))->DataChanged(sender, mask);
  }
}

void CHomeData::SetShortCutGroup(wchar_t* name, int group)
{
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    ((CHomeNotifier*)m_list->Get(i))->SetShortCutGroup(name, group);
  }
}

uint CHomeData::GetMask(int labelkind)
{
  if (labelkind >= NT_START)
    return NodeMasks[labelkind - NT_START];
  else
    return DATA_NONE;
}

uint CHomeData::GetMaskDelay(uint mask)
{
  uint ret = MAX_DELAY;
  for (int i = 0; i < LENOF(DataDelays); i++)
    if ((mask & (1 << i)) && (DataDelays[i] < ret))
      ret = DataDelays[i];
  return ret;
}

void CHomeData::GetFullPathFileName(wchar_t* &filename)
{
  PathUnixToWin(filename);
  if (filename[0] && filename[0] != '\\' && filename[0] != '%')
  {
    wchar_t* name;
    InitString(name);

    // 无路径的文件默认为安装目录、Windows路径或主题目录
    CloneString(name, filename);
    AddDefaultPath(name, m_plugindir);
    if (!FileExists(name))
    {
      CloneString(name, filename);
      AddDefaultPath(name, sWindowsPath);
      if (!FileExists(name))
      {
        CloneString(name, filename);
        AddDefaultPath(name, m_homedir);
      }
    }
    CloneString(filename, name);
    FreeString(name);
  }
}

void CHomeData::AddMask(uint mask)
{
  m_mask |= mask;
  m_maskdelay = GetMaskDelay(m_mask);
  UpdateInterval();
}

uint CHomeData::GetInfoInterval()
{
  return m_infointerval;
}

void CHomeData::SetInfoInterval(uint value)
{
  m_infointerval = value;
  if (m_infointerval < 1000 || m_infointerval > 100 * 1000)
    m_infointerval = DATA_INTERVAL;
  UpdateInterval();
}

void CHomeData::CalcRiseSet(CTextNode* pText)
{
  SYSTEMTIME stime;
  SUNTIME suntime;
  GetLocalTime(&stime);
  pText->data.suntype = GetSunRiseSetTime(stime.wYear, stime.wMonth, stime.wDay, 
    pText->data.geo.longitude, pText->data.geo.latitude, pText->data.geo.zonetime, &suntime);
  if (pText->data.node == NT_LIGHTTIME)
    pText->data.suntime = suntime.lighttime;
  else if (pText->data.node == NT_RISETIME)
    pText->data.suntime = suntime.risetime;
  else if (pText->data.node == NT_TRANSITTIME)
    pText->data.suntime = suntime.transittime;
  else if (pText->data.node == NT_SETTIME)
    pText->data.suntime = suntime.settime;
  else if (pText->data.node == NT_DARKTIME)
    pText->data.suntime = suntime.darktime;
}

int CHomeData::FixTemperature(int temp)
{
  if (temp)
  {
    if (temp_fix)
      temp /= 10;
    temp += temp_adjust;
    return temp;
  }
  else
    return 0;
}

void CHomeData::AddToolToList(int cmd, wchar_t* title, wchar_t* target, wchar_t* args, bool confirm)
{
  CShortCut* shortcut = new CShortCut(NULL);
  shortcut->data.cmd = cmd;
  shortcut->data.exist = true;
  shortcut->data.confirm = confirm;
  CloneString(shortcut->title, title);
  CloneString(shortcut->target, target);
  CloneString(shortcut->args, args);
  tools->Add(shortcut);
}

void CHomeData::AddToolFromRes(int cmd, UINT resid, bool confirm)
{
  wchar_t buff[TEXT_LEN];
  LoadString(hRes, resid, buff, TEXT_LEN - 1);
  AddToolToList(cmd, buff, L"", L"", confirm);
}

void CHomeData::InitTools()
{
  if (tools->GetCount() == 0)
  {
    AddToolFromRes(CMD_SHOWCALINFO, IDS_CAP_CALENDER);
    AddToolFromRes(CMD_SHOWHOLIDAYINFO, IDS_CAP_HOLIDAY);
    AddToolFromRes(CMD_SHOWPOWERINFO, IDS_CAP_POWER);
    AddToolFromRes(CMD_SHOWMEMORYINFO, IDS_CAP_MEM);
    AddToolFromRes(CMD_RESTART, IDS_CAP_RESTART, true);
    AddToolFromRes(CMD_SHUTDOWN, IDS_CAP_SHUTDOWN, true);
  }
}

// 更新定时器间隔
void CHomeData::UpdateInterval()
{
  switch (m_maskdelay)
  {
  case SECOND_DELAY:
    interval = SECOND_INTERVAL;
    break;
  case TIME_DELAY:
    interval = TIME_INTERVAL;
    break;
  case DATA_DELAY:
    interval = m_infointerval;
    break;
  default:
    interval = MAX_INTERVAL;
    break;
  }
}

void CHomeData::UpdateHolidayItems()
{
  ODATE sdate, edate;
  sdate.year = m_time.wYear;
  sdate.month = m_time.wMonth;
  sdate.day = m_time.wDay;
  edate = sdate;
  // 保证把一年的节日都算出来
  OffsetDate(&edate, 400);

  holidayitems->Clear();
  holiday->GetHoliday(holidayitems, &sdate, &edate, 0xFFFF);
  userday->GetHoliday(holidayitems, &sdate, &edate, 0xFFFF);
  holidayitems->Sort();
}

void CHomeData::ClearToolList()
{
  for (int i = 0; i < tools->GetCount(); i++)
  {
    CShortCut* item = (CShortCut*)tools->Get(i);
    delete item;
  }
  tools->Clear();
}

void CHomeData::ClearCounterList()
{
  for (int i = 0; i < counters->GetCount(); i++)
  {
    CDownCounter* item = (CDownCounter*)counters->Get(i);
    delete item;
  }
  counters->Clear();
}

void CHomeData::CheckData(bool forceupdate)
{
  SYSTEMTIME stime;
  uint tick = GetTickCount();
  uint mask = forceupdate ? DATA_FORCE : 0;

  if (forceupdate || !m_datatick || tick - m_datatick >= m_infointerval)
  {
    m_datatick = tick;

    // 更新内存信息
    if (forceupdate || m_mask & (DATA_MEM | DATA_MEMRATE))
    {
      MEMORYSTATUS memstat;
      memstat.dwLength = sizeof(memstat);
      GlobalMemoryStatus(&memstat);
      if (memstat.dwTotalPhys != m_memstat.dwTotalPhys || memstat.dwAvailPhys != m_memstat.dwAvailPhys)
        mask |= DATA_MEM;
      if (memstat.dwMemoryLoad != m_memstat.dwMemoryLoad)
        mask |= DATA_MEMRATE;
      m_memstat = memstat;
    }

    // 更新存储对象信息
    if (forceupdate || m_mask & (DATA_STORE | DATA_STORERATE))
    {
      STORE_INFORMATION store;
      GetStoreInformation(&store);
      if (store.dwFreeSize != m_store.dwFreeSize || store.dwStoreSize != m_store.dwStoreSize)
      {
        int storeload = 0;
        mask |= DATA_STORE;
        if (m_store.dwStoreSize > 0)
          storeload = ((m_store.dwStoreSize - m_store.dwFreeSize) >> 8) * 100 / (m_store.dwStoreSize >> 8);
        if (m_storeload != storeload)
          mask |= DATA_STORERATE;
        m_store = store;
        m_storeload = storeload;
      }
    }

    // 更新存储器信息
    if (forceupdate || m_mask & (DATA_STORAGE | DATA_STORAGERATE))
    {
      STORAGE_INFO storage;
      GetStorageInfo(m_storagedir, &storage);
      if (storage.free != m_storage.free || storage.total != m_storage.total)
        mask |= DATA_STORAGE;
      if (storage.load != m_storage.load)
        mask |= DATA_STORAGERATE;
      m_storage = storage;
    }

    // 更新SD卡信息
    if (forceupdate || m_mask & (DATA_STORAGECARD | DATA_STORAGECARDRATE))
    {
      STORAGE_INFO storagecard;
      GetStorageInfo(m_storagecarddir, &storagecard);
      if (storagecard.free != m_storagecard.free || storagecard.total != m_storagecard.total)
        mask |= DATA_STORAGECARD;
      if (storagecard.load != m_storagecard.load)
        mask |= DATA_STORAGECARDRATE;
      m_storagecard = storagecard;
    }

    // 更新电池信息
    if (forceupdate || m_mask & (DATA_POWER | DATA_POWERRATE))
    {
      SYSTEM_POWER_STATUS_EX2 power;
      if (GetSystemPowerStatusEx2(&power, sizeof(power), forceupdate ? TRUE : FALSE) ||
        GetSystemPowerStatusEx((PSYSTEM_POWER_STATUS_EX)&power, forceupdate ? TRUE : FALSE))
      {
        if (power.BatteryVoltage != m_power.BatteryVoltage || power.BatteryCurrent != m_power.BatteryCurrent ||
          power.BatteryAverageCurrent != m_power.BatteryAverageCurrent)
          mask |= DATA_POWER;
        power.BatteryTemperature = FixTemperature(power.BatteryTemperature);
        if (power.BatteryLifePercent != m_power.BatteryLifePercent || power.BatteryTemperature != power.BatteryTemperature)
          mask |= DATA_POWERRATE;
        m_power = power;
      }
      else
        memset(&m_power, 0, sizeof(m_power));
    }

    // 更新线路信息
    if (forceupdate || m_mask & (DATA_LINEDEV | DATA_LINEDEVRATE))
    {
      if (InitLineApp())
      {
        LINEDEVSTATUS linestatus;
        memset(&linestatus, 0, sizeof(linestatus));
        linestatus.dwTotalSize = sizeof(linestatus);
        if (lineGetLineDevStatus(m_line, &linestatus) == 0)
        {
          int signal = ((linestatus.dwSignalLevel + 1) * 100) >> 16;
          if (linestatus.dwSignalLevel != m_linestatus.dwSignalLevel)
            mask |= DATA_LINEDEV;
          if (signal != m_signal)
            mask |= DATA_LINEDEVRATE;
          m_linestatus = linestatus;
          m_signal = signal;
        }
      }
      else
        memset(&m_power, 0, sizeof(m_power));
    }
  }

  if (forceupdate || m_sectick || m_mask & DATA_SECOND || tick - m_sectick >= SECOND_INTERVAL)
  {
    m_sectick = tick;

    GetLocalTime(&stime);

    if (stime.wYear != m_time.wYear || stime.wMonth != m_time.wMonth || stime.wDay != m_time.wDay)
    {
      EncodeDate(stime.wYear, stime.wMonth, stime.wDay, &alldays);
      mask |= DATA_DAY;
    }

    if (stime.wHour != m_time.wHour)
      mask |= DATA_HOUR;

    if (stime.wMinute != m_time.wMinute)
      mask |= DATA_MINUTE;

    if (stime.wSecond != m_time.wSecond)
      mask |= DATA_SECOND;

    m_time = stime;

    // 更新日历信息
    if (forceupdate || m_mask & DATA_CALENDAR)
    {
      if (!m_calinfo.date.year || mask & (DATA_DAY | DATA_HOUR))
      {
        ODATE date;
        OTIME time;

        date.year = stime.wYear;
        date.month = stime.wMonth;
        date.day = stime.wDay;
        time.hour = stime.wHour;
        time.minute = stime.wMinute;
        time.second = stime.wSecond;

        GetCalendarInfo(&date, &time, &m_calinfo);

        mask |= DATA_CALENDAR;
      }
    }

    // 更新日出日落
    if (forceupdate || m_mask & DATA_RISESET)
    {
      if (mask & DATA_DAY)
      {
        for (int i = 0; i < m_riseset->GetCount(); i++)
        {
          CalcRiseSet((CTextNode*)m_riseset->Get(i));
        }
        mask |= DATA_RISESET;
      }
    }

    // 更新节假日
    if (forceupdate || mask & DATA_DAY)
    {
      UpdateHolidayItems();
      mask |= DATA_HOLIDAY;
    }

    // 通知插件更新显示
    if (mask)
    {
      NotifyPlugins(this, mask);
    }
  }
}

/************************************************************************/
/* 数据计算                                                             */
/************************************************************************/

void CHomeData::GetSunTimeStr(SUNTYPE suntype, OTIME* time, wchar_t* text)
{
  if (suntype == stNormal)
    wsprintf(text, L"%d:%02d", time->hour, time->minute + (time->second >= 30 ? 1 : 0));
  else if (suntype == stAllwaysUp)
    wcscpy(text, sSunAllwaysUp);
  else if (suntype == stAllwaysDown)
    wcscpy(text, sSunAllwaysDown);
  else
    wcscpy(text, sSunError);
}

void CHomeData::GetHolidayStr(OHOLIDAY_ITEM* item, bool shortfmt, bool incdate, wchar_t* text, int slen)
{
  wchar_t buff[256];
  text[0] = 0;

  // 处理日期
  if (incdate)
  {
    wsprintf(buff, sFmtHoliDate, item->date.month, item->date.day);
    wcsncpy(text, buff, slen - wcslen(text) - 1);
  }

  // 处理节假日内容
  switch (item->holiday->data.type)
  {
  case htSolar:
  case htLunar:
  case htWeek:
    wcsncat(text, item->holiday->text, slen - wcslen(text) - 1);
    break;
  case htSolarBirth:
    if (shortfmt)
      wsprintf(buff, sFmtHoliBirth, item->holiday->text);
    else
      wsprintf(buff, sFmtHoliBirthL, item->holiday->text, item->date.year - item->holiday->data.year);
    wcsncat(text, buff, slen - wcslen(text) - 1);
    break;
  case htSolarDay:
    if (shortfmt)
      wsprintf(buff, sFmtHoliLDay, item->holiday->text);
    else
      wsprintf(buff, sFmtHoliLDayL, item->holiday->text, item->date.year - item->holiday->data.year);
    wcsncat(text, buff, slen - wcslen(text) - 1);
    break;
  case htLunarBirth:
    if (shortfmt)
      wsprintf(buff, sFmtHoliBirth, item->holiday->text);
    else
      wsprintf(buff, sFmtHoliLBirth, item->holiday->text, item->lunar.year - item->holiday->data.year);
    wcsncat(text, buff, slen - wcslen(text) - 1);
    break;
  case htLunarDay:
    if (shortfmt)
      wsprintf(buff, sFmtHoliLDay, item->holiday->text);
    else
      wsprintf(buff, sFmtHoliLDayL, item->holiday->text, item->lunar.year - item->holiday->data.year);
    wcsncat(text, buff, slen - wcslen(text) - 1);
    break;
  }

  // 处理农历日期
  if (incdate)
  {
    switch (item->holiday->data.type)
    {
    case htLunar:
    case htLunarBirth:
    case htLunarDay:
      wsprintf(buff, sFmtHoliLDate, item->lunar.month, item->lunar.day);
      wcsncat(text, buff, slen - wcslen(text) - 1);
      break;
    }
  }
}

void CHomeData::GetShortHolidayStr(CTextNode* pText, wchar_t* text, int slen)
{
  int level = pText->data.level >= 0 ? pText->data.level : holidaylevel;
  for (int i = 0; i < holidayitems->GetCount(); i++)
  {
    OHOLIDAY_ITEM* item = (OHOLIDAY_ITEM*)holidayitems->Get(i);
    // 日期为当天并且类型匹配
    if (item->days == alldays && (pText->data.showmask & (1 << item->holiday->data.type)))
    {
      if (item->holiday->data.level <= level && item->holiday->stext[0])
      {
        ProcTextNodeValue(pText, item->holiday->stext, text, slen);
        return;
      }
    }
    else
      break;
  }
}

void CHomeData::GetHolidayStrValue(CTextNode* pText, wchar_t* text, int slen)
{
  wchar_t buff[256], str[TEXT_LEN];
  int level = pText->data.level >= 0 ? pText->data.level : holidaylevel;
  buff[0] = 0;
  for (int i = 0; i < holidayitems->GetCount(); i++)
  {
    OHOLIDAY_ITEM* item = (OHOLIDAY_ITEM*)holidayitems->Get(i);
    int offset = item->days - alldays;
    // 日期符合并且类型匹配
    if (offset >= pText->data.sday && offset <= pText->data.eday && 
      (pText->data.showmask & (1 << item->holiday->data.type)))
    {
      if (item->holiday->data.level <= level && item->holiday->text[0])
      {
        GetHolidayStr(item, !pText->data.fullmode, pText->data.incdate, str, TEXT_LEN);
        if (buff[0]) wcsncat(buff, L" ", 255 - wcslen(buff));
        wcsncat(buff, str, 255 - wcslen(buff));
      }
    }
    else if (offset > pText->data.eday)
      break;
  }

  if (buff[0])
    ProcTextNodeValue(pText, buff, text, slen);
}

CDownCounter* CHomeData::GetDownCounterByName(wchar_t* name)
{
  if (name == NULL || name[0] == 0)
  {
    if (downenabled)
      return downcounter;
    else
      return NULL;
  }
  else
  {
    for (int i = 0; i < counters->GetCount(); i++)
    {
      CDownCounter* item = (CDownCounter*)counters->Get(i);
      if (IsSameText(name, item->name))
        return item;
    }
  }
  return NULL;
}

wchar_t* CHomeData::GetDownCounterStr(CTextNode* pText)
{
  CDownCounter* counter = GetDownCounterByName(pText->relname);
  if (counter)
    return counter->text;
  else
    return L"";
}

int CHomeData::GetDownCounterValue(CTextNode* pText)
{
  CDownCounter* counter = GetDownCounterByName(pText->relname);
  if (counter)
  {
    int d1, d2, h1, h2, m1, m2, s1, s2;
    // 计算相差的日期和时间
    EncodeDate(m_time.wYear, m_time.wMonth, m_time.wDay, &d1);
    EncodeDate(counter->date.year, counter->date.month, counter->date.day, &d2);
    h1 = m_time.wHour;
    h2 = counter->time.hour;
    m1 = h1 * 60 + m_time.wMinute;
    m2 = h2 * 60 + counter->time.minute;
    s1 = m1 * 60 + m_time.wSecond;
    s2 = m2 * 60 + counter->time.second;

    // 已经过期返回 0
    if (d1 > d2)
      return 0;
    else if (d1 == d2 && s1 >= s2)
      return 0;

    switch (pText->data.node)
    {
    case NT_DOWNDAY:
      if (pText->data.isend || s1 <= s2)
        return d2 - d1;
      else
        return d2 - d1 - 1;
    case NT_DOWNHOUR:
      if (pText->data.isend || (m_time.wMinute * 60 + m_time.wSecond <=
        counter->time.hour * 60 + counter->time.second))
        return (h2 - h1 + 24) % 24;
      else
        return (h2 - h1 + 24 - 1) % 24;
    case NT_DOWNMINUTE:
      if (pText->data.isend || (m_time.wSecond <= counter->time.second))
        return (m2 - m1 + 60) % 60;
      else
        return (m2 - m1 + 60 - 1) % 60;
    case NT_DOWNSECOND:
      return (s2 - s1) % 60;
    default:
      return 0;
    }
  }
  else
    return 0;
}

CShortCut* CHomeData::GetPluginShortCut(wchar_t* name)
{
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CHomeNotifier* home = (CHomeNotifier*)m_list->Get(i);
    if (IsSameText(name, home->GetPluginName()))
    {
      return home->GetSelected();
    }
  }

  return NULL;
}

void CHomeData::ProcTextNodeValue(CTextNode* pText, wchar_t* s, wchar_t* text, int slen)
{
  // 复制左字符串
  if (pText->ls[0])
    wcsncpy(text, pText->ls, slen - 1);

  // 处理整字符串替换
  if (pText->replace[0])
  {
    uint i, j, k;
    wchar_t buff[TEXT_LEN];

    wcsncpy(buff, s, TEXT_LEN - 1);

    // replace 的内容可能是："十一=冬,一=正"
    for (i = 0; i < wcslen(pText->replace); i++)
    {
      // 查找当前替换字符串
      for (j = i; j < wcslen(pText->replace); j++)
        if (pText->replace[j] == '=')
        {
          for (k = j; k < wcslen(pText->replace); k++)
            if (pText->replace[k] == ',')
              break;
          break;
        }

      // 判断输入是否与当前值相等
      if ((wcslen(s) == j - i) && (wmemcmp(s, &pText->replace[i], j - i) == 0))
      {
        int len = min(k - j - 1, TEXT_LEN - 1);
        wcsncpy(buff, &pText->replace[j + 1], len);
        buff[len] = 0;
        break;
      }

      i = k;
    }

    wcsncat(text, buff, slen - wcslen(text) - 1);
  }
  else
  {
    wcsncat(text, s, slen - wcslen(text) - 1);
  }

  // 连接右字符串
  if (pText->rs[0])
    wcsncat(text, pText->rs, slen - wcslen(text) - 1);
}

void CHomeData::GetTextNodeStrValue(CTextNode* pText, wchar_t* s, wchar_t* text, int slen)
{
  text[0] = 0;

  if (s && s[0])
  {
    ProcTextNodeValue(pText, s, text, slen);
    if (pText->data.len > 0 && pText->data.len < slen)
      text[pText->data.len] = 0;
  }
}

void CHomeData::GetTextNodeIntValue(CTextNode* pText, int val, wchar_t* text, int slen)
{
  wchar_t fmt[TEXT_LEN], buff[TEXT_LEN];
  double value;

  text[0] = fmt[0] = buff[0] = 0;

  if (pText->data.valid != DEF_VALID && val != pText->data.valid)
    return;

  if (pText->data.invalid != DEF_VALID && val == pText->data.invalid)
    return;

  value = val;
  switch (pText->data.op)
  {
    case OP_ADD:
      value += pText->data.opnum;
      break;
    case OP_SUBX:
      value -= pText->data.opnum;
      break;
    case OP_XSUB:
      value = pText->data.opnum - value;
      break;
    case OP_MUL:
      value *= pText->data.opnum;
      break;
    case OP_DIVX:
      if (pText->data.opnum != 0)
        value /= pText->data.opnum;
      else
        value = 0;
      break;
    case OP_XDIV:
      if (value != 0)
        value = pText->data.opnum / value;
      else
        value = 0;
      break;
  }

  if (pText->data.len)
  {
    if (pText->data.decdig)
    {
      wsprintf(fmt, L"%%%s%d.%df", pText->data.leadzero ? L"0" : L"", pText->data.len, pText->data.decdig);
      wsprintf(buff, fmt, value);
    }
    else
    {
      wsprintf(fmt, L"%%%s%dd", pText->data.leadzero ? L"0" : L"", pText->data.len);
      wsprintf(buff, fmt, (int)value);
    }
  }
  else
  {
    wsprintf(buff, L"%d", (int)value);
  }

  ProcTextNodeValue(pText, buff, text, slen);
}

#define GetNodeStrValue(s) GetTextNodeStrValue(pText, s, text, slen)
#define GetNodeIntValue(v) GetTextNodeIntValue(pText, v, text, slen)

void CHomeData::GetTextNodeValue(CTextNode* pText, wchar_t* text, int slen)
{
  CShortCut* shortcut;

  text[0] = 0;
  switch(pText->data.node)
  {
  case NT_TEXT:
    GetNodeStrValue(pText->text);
    break;
  case NT_SPACE:
    for (int i = 0; i < pText->data.len; i++)
      text[i] = ' ';
    text[pText->data.len] = 0;
    break;
  case NT_BR:
    wcscpy(text, L"\n");
    break;
  case NT_YEAR:
    GetNodeIntValue(m_calinfo.date.year);
    break;
  case NT_MONTH:
    GetNodeIntValue(m_calinfo.date.month);
    break;
  case NT_DAY:
    GetNodeIntValue(m_calinfo.date.day);
    break;
  case NT_CWEEK:
    GetNodeStrValue(m_calinfo.sWeek);
    break;
  case NT_HOUR:
    GetNodeIntValue(m_time.wHour);
    break;
  case NT_MINUTE:
    GetNodeIntValue(m_time.wMinute);
    break;
  case NT_SECOND:
    GetNodeIntValue(m_time.wSecond);
    break;
  case NT_LYEAR:
    GetNodeStrValue(m_calinfo.sYear);
    break;
  case NT_LMONTH:
    GetNodeStrValue(m_calinfo.sMonth);
    break;
  case NT_LLEAPMONTH:
    GetNodeStrValue(m_calinfo.sLeapMonth);
    break;
  case NT_LSHORTMONTH:
    GetNodeStrValue(m_calinfo.sShortMonth);
    break;
  case NT_LMONTHLEN:
    GetNodeStrValue(m_calinfo.sMonthLen);
    break;
  case NT_LDAY:
    GetNodeStrValue(m_calinfo.sDay);
    break;
  case NT_GZYEAR:
    GetNodeStrValue(m_calinfo.sgzYear);
    break;
  case NT_GZMONTH:
    GetNodeStrValue(m_calinfo.sgzMonth);
    break;
  case NT_GZDAY:
    GetNodeStrValue(m_calinfo.sgzDay);
    break;
  case NT_GZHOUR:
    GetNodeStrValue(m_calinfo.sgzHour);
    break;
  case NT_JIEQI:
    GetNodeStrValue(m_calinfo.sJieQi);
    break;
  case NT_XINGZUO:
    GetNodeStrValue(m_calinfo.sXingZuo);
    break;
  case NT_SHENGXIAO:
    GetNodeStrValue(m_calinfo.sShengXiao);
    break;
  case NT_HOLIDAY:
    GetHolidayStrValue(pText, text, slen);
    break;
  case NT_SHORTHOLIDAY:
    GetShortHolidayStr(pText, text, slen);
    break;
  case NT_DOWNTEXT:
    GetNodeStrValue(GetDownCounterStr(pText));
    break;
  case NT_DOWNDAY:
  case NT_DOWNHOUR:
  case NT_DOWNMINUTE:
  case NT_DOWNSECOND:
    GetNodeIntValue(GetDownCounterValue(pText));
    break;
  case NT_LIGHTTIME:
  case NT_RISETIME:
  case NT_TRANSITTIME:
  case NT_SETTIME:
  case NT_DARKTIME:
    GetSunTimeStr(pText->data.suntype, &pText->data.suntime, text);
    break;
  case NT_MEMORYLOAD:
    GetNodeIntValue(m_memstat.dwMemoryLoad);
    break;
  case NT_TOTALMEMORY:
    GetNodeIntValue(m_memstat.dwTotalPhys);
    break;
  case NT_FREEMEMORY:
    GetNodeIntValue(m_memstat.dwAvailPhys);
    break;
  case NT_STORELOAD:
    GetNodeIntValue(m_storeload);
    break;
  case NT_TOTALSTORE:
    GetNodeIntValue(m_store.dwStoreSize);
    break;
  case NT_FREESTORE:
    GetNodeIntValue(m_store.dwFreeSize);
    break;
  case NT_STORAGELOAD:
    GetNodeIntValue(m_storage.load);
    break;
  case NT_TOTALSTORAGE:
    GetNodeIntValue(m_storage.total);
    break;
  case NT_FREESTORAGE:
    GetNodeIntValue(m_storage.free);
    break;
  case NT_STORAGECARDLOAD:
    GetNodeIntValue(m_storagecard.load);
    break;
  case NT_TOTALSTORAGECARD:
    GetNodeIntValue(m_storagecard.total);
    break;
  case NT_FREESTORAGECARD:
    GetNodeIntValue(m_storagecard.free);
    break;
  case NT_BATTERY:
    GetNodeIntValue(m_power.BatteryLifePercent <= 100 ? m_power.BatteryLifePercent : 0);
    break;
  case NT_TEMPERATURE:
    GetNodeIntValue(m_power.BatteryTemperature);
    break;
  case NT_VOLTAGE:
    GetNodeIntValue(m_power.BatteryVoltage);
    break;
  case NT_CURRENT:
    GetNodeIntValue(m_power.BatteryCurrent);
    break;
  case NT_AVERCURRENT:
    GetNodeIntValue(m_power.BatteryAverageCurrent);
    break;
  case NT_SHORTCUTTITLE:
    shortcut = GetPluginShortCut(pText->pluginname);
    GetNodeStrValue(shortcut ? shortcut->title : L"");
    break;
  case NT_SHORTCUTDESC:
    shortcut = GetPluginShortCut(pText->pluginname);
    GetNodeStrValue(shortcut ? shortcut->desc : L"");
    break;
  case NT_SHORTCUTCOUNT:
    shortcut = GetPluginShortCut(pText->pluginname);
    GetNodeIntValue(shortcut ? shortcut->parent->GetShortCutCount() : 0);
    break;
  case NT_SHORTCUTINDEX:
    shortcut = GetPluginShortCut(pText->pluginname);
    GetNodeIntValue(shortcut ? shortcut->parent->GetShortCutIndex(shortcut) : 0);
    shortcut = GetPluginShortCut(pText->pluginname);
    break;
  case NT_SIGNAL:
    GetNodeIntValue(m_signal);
  default:
    break;
  }
}
