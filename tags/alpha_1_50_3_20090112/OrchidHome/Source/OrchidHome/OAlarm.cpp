/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：提醒项处理单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OAlarm.cpp,v 1.2 2006/06/01 09:51:31 zjy Exp $
* 备    注：
*******************************************************************************/

#include "OAlarm.h"
#include "OUtils.h"
#include "OXMLUtils.h"

/************************************************************************/
/* 常量定义                                                             */
/************************************************************************/

static wchar_t* sAlarms = L"alarms";
static wchar_t* sItem = L"item";
static wchar_t* sEnabled = L"enabled";
static wchar_t* sAlarmType = L"alarmtype";
static wchar_t* sAlarmMode = L"alarmmode";
static wchar_t* sRingCount = L"ringcount";
static wchar_t* sRecurType = L"recurtype";
static wchar_t* sWeeks = L"weeks";
static wchar_t* sMonth = L"month";
static wchar_t* sDay = L"day";
static wchar_t* sInterval = L"interval";
static wchar_t* sSYear = L"syear";
static wchar_t* sSMonth = L"smonth";
static wchar_t* sSDay = L"sday";
static wchar_t* sSHour = L"shour";
static wchar_t* sSMinute = L"sminute";
static wchar_t* sEHour = L"ehour";
static wchar_t* sEMinute = L"eminute";
static wchar_t* sVolume = L"Volume";
static wchar_t* sText = L"text";
static wchar_t* sSndFile = L"sndfile";
static wchar_t* sTarget = L"target";
static wchar_t* sArgs = L"args";

static EnumDef AlarmTypes[] = {
  {atAlarm, L"Alarm"}, {atExecute, L"Execute"}, {atFlightMode, L"FlightMode"}, {atSilentMode, L"SilentMode"}, 
  {atRestart, L"Restart"}, {atPowerOff, L"PowerOff"}, {atChime, L"Chime"}
};

static EnumDef AlarmModes[] = {
  {amRing, L"Ring"}, {amVibrate, L"Vibrate"}, {amBoth, L"Both"}, {amVibrateFirst, L"VibrateFirst"}
};

static EnumDef RecurTypes[] = {
  {arOnce, L"Once"}, {arDaily, L"Daily"}, {arWeekDays, L"WeekDays"}, {arWeekEnds, L"WeekEnds"}, 
  {arWeekly, L"Weekly"}, {arMonthly, L"Monthly"}, {arAnnually, L"Annually"}, {arInterval, L"Interval"}
};

/************************************************************************/
/* 提醒类                                                               */
/************************************************************************/

CAlarm::CAlarm()
{
  memset(&data, 0, sizeof(data));
  InitString(text); 
  InitString(sndfile);
  InitString(target); 
  InitString(args); 
}

CAlarm::~CAlarm()
{
  FreeString(text); 
  FreeString(sndfile);
  FreeString(target); 
  FreeString(args); 
}

HRESULT CAlarm::LoadFromStream(IStream* pStm)
{
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, text)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, sndfile)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, target)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, args)))
    return E_FAIL;

  return S_OK;
}

HRESULT CAlarm::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, text)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, sndfile)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, target)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, args)))
    return E_FAIL;

  return S_OK;
}

void CAlarm::Clone(CAlarm* source)
{
  data = source->data;
  CloneString(text, source->text);
  CloneString(sndfile, source->sndfile);
  CloneString(target, source->target);
  CloneString(args, source->args);
}

/************************************************************************/
/* 节假日列表类                                                         */
/************************************************************************/

CAlarmList::CAlarmList()
{
  m_list = new CList;
}

CAlarmList::~CAlarmList()
{
  Clear();
  delete m_list;
}

HRESULT CAlarmList::LoadFromStream(IStream* pStm)
{
  int cnt;

  Clear();
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;

  if (FAILED(pStm->Read(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (; cnt; cnt--)
  {
    CAlarm* item = new CAlarm;
    if (FAILED(item->LoadFromStream(pStm)))
    {
      delete item;
      Clear();
      return E_FAIL;
    }
    m_list->Add(item);
  }

  return S_OK;
}

HRESULT CAlarmList::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;

  int cnt = m_list->GetCount();
  if (FAILED(pStm->Write(&cnt, sizeof(int), 0)))
    return E_FAIL;

  for (int i = 0; i < cnt; i++)
  {
    CAlarm* item = (CAlarm*)m_list->Get(i);
    if (FAILED(item->SaveToStream(pStm)))
      return E_FAIL;
  }

  return S_OK;
}

HRESULT CAlarmList::ReadItem(IXMLDOMNode* node, CAlarm* item)
{
  IXMLDOMNamedNodeMap* nodemap = NULL;
  node->get_attributes(&nodemap);
  if (nodemap)
  {
    int dow = 0;
    if (FAILED(XMLGetBoolAttribute(nodemap, sEnabled, &item->data.enabled)))
      return E_FAIL;
    if (FAILED(XMLGetEnumAttribute(nodemap, sAlarmType, &item->data.alarmType, AlarmTypes, LENOF(AlarmTypes))))
      return E_FAIL;
    if (FAILED(XMLGetEnumAttribute(nodemap, sAlarmMode, &item->data.alarmMode, AlarmModes, LENOF(AlarmModes))))
      return E_FAIL;
    if (FAILED(XMLGetEnumAttribute(nodemap, sRecurType, &item->data.recurInfo.type, RecurTypes, LENOF(RecurTypes))))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sWeeks, &dow)))
      return E_FAIL;
    for (int i = 0; i < 7; i++)
      item->data.recurInfo.weeks[i] = ((dow >> i) & 1) == 1;
    if (FAILED(XMLGetIntAttribute(nodemap, sMonth, &item->data.recurInfo.month)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sDay, &item->data.recurInfo.day)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sInterval, &item->data.recurInfo.interval)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sSYear, &item->data.dateStart.year)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sSMonth, &item->data.dateStart.month)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sSDay, &item->data.dateStart.day)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sSHour, &item->data.timeStart.hour)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sSMinute, &item->data.timeStart.minute)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sEHour, &item->data.timeEnd.hour)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sEMinute, &item->data.timeEnd.minute)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sRingCount, &item->data.ringCount)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sVolume, &item->data.volume)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(nodemap, sText, &item->text)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(nodemap, sSndFile, &item->sndfile)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(nodemap, sTarget, &item->target)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(nodemap, sArgs, &item->args)))
      return E_FAIL;
    nodemap->Release();
    nodemap = NULL;
  }

  return S_OK;
}

HRESULT CAlarmList::AppendTextNode(IXMLDOMDocument* xml, IXMLDOMElement* element, wchar_t* text)
{
  IXMLDOMText* node = NULL;
  xml->createTextNode(text, &node);
  if (node)
    return element->appendChild(node, NULL);
  else
    return S_FALSE;
}

HRESULT CAlarmList::SaveItem(IXMLDOMDocument* xml, IXMLDOMNode* node, CAlarm* item)
{
  IXMLDOMNamedNodeMap* nodemap = NULL;
  node->get_attributes(&nodemap);
  if (nodemap)
  {
    int dow = 0;
    if (FAILED(XMLSetBoolAttribute(xml, nodemap, sEnabled, item->data.enabled)))
      return E_FAIL;
    if (FAILED(XMLSetEnumAttribute(xml, nodemap, sAlarmType, item->data.alarmType, AlarmTypes, LENOF(AlarmTypes))))
      return E_FAIL;
    if (FAILED(XMLSetEnumAttribute(xml, nodemap, sAlarmMode, item->data.alarmMode, AlarmModes, LENOF(AlarmModes))))
      return E_FAIL;
    if (FAILED(XMLSetEnumAttribute(xml, nodemap, sRecurType, item->data.recurInfo.type, RecurTypes, LENOF(RecurTypes))))
      return E_FAIL;
    for (int i = 0; i < 7; i++)
      dow |= (item->data.recurInfo.weeks[i] ? 1 : 0) << i;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sWeeks, dow)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sMonth, item->data.recurInfo.month)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sDay, item->data.recurInfo.day)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sInterval, item->data.recurInfo.interval)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sSYear, item->data.dateStart.year)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sSMonth, item->data.dateStart.month)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sSDay, item->data.dateStart.day)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sSHour, item->data.timeStart.hour)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sSMinute, item->data.timeStart.minute)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sEHour, item->data.timeEnd.hour)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sEMinute, item->data.timeEnd.minute)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sRingCount, item->data.ringCount)))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sVolume, item->data.volume)))
      return E_FAIL;
    if (FAILED(XMLSetStrAttribute(xml, nodemap, sText, item->text)))
      return E_FAIL;
    if (FAILED(XMLSetStrAttribute(xml, nodemap, sSndFile, item->sndfile)))
      return E_FAIL;
    if (FAILED(XMLSetStrAttribute(xml, nodemap, sTarget, item->target)))
      return E_FAIL;
    if (FAILED(XMLSetStrAttribute(xml, nodemap, sArgs, item->args)))
      return E_FAIL;
    nodemap->Release();
    nodemap = NULL;
  }

  return S_OK;
}

bool CAlarmList::LoadFromXML(IXMLDOMDocument* xml)
{
  IXMLDOMNode* root = NULL;
  IXMLDOMNode* node = NULL;

  Clear();
  xml->get_firstChild(&root);
  while (root)
  {
    if (XMLNodeNameIs(root, sAlarms))
    {
      root->get_firstChild(&node);
      while (node)
      {
        if (XMLNodeNameIs(node, sItem))
        {
          CAlarm* alarm = new CAlarm;
          if (SUCCEEDED(ReadItem(node, alarm)))
          {
            m_list->Add(alarm);
          }
          else
          {
            delete alarm;
          }
        }
        node->get_nextSibling(&node);
      }
      break;
    }
    root->get_nextSibling(&root);
  }

  if (root) root->Release();
  if (node) node->Release();
  return true;
}

bool CAlarmList::SaveToXML(IXMLDOMDocument* xml)
{
  IXMLDOMElement* root = NULL;
  IXMLDOMElement* node = NULL;
  IXMLDOMProcessingInstruction* procnode = NULL;

  xml->createProcessingInstruction(L"xml", L"version='1.0'", &procnode);
  if (procnode)
    xml->appendChild(procnode, NULL);

  xml->createElement(sAlarms, &root);
  if (root)
  {
    xml->putref_documentElement(root);
    for (int i = 0; i < m_list->GetCount(); i++)
    {
      xml->createElement(sItem, &node);
      if (node)
      {
        AppendTextNode(xml, root, L"\r\n\t");
        if (SUCCEEDED(SaveItem(xml, node, (CAlarm*)m_list->Get(i))))
        {
          root->appendChild(node, NULL);
        }
      }
    }
    AppendTextNode(xml, root, L"\r\n");
  }

  if (procnode) procnode->Release();
  if (node) node->Release();
  if (root) root->Release();

  return true;
}

bool CAlarmList::LoadFromFile(wchar_t* filename)
{
  bool succ = false;
  Clear();
  if (FileExists(filename))
  {
    IXMLDOMDocument* xml = NULL;
    if (SUCCEEDED(XMLCreateDocument(&xml)))
    {
      if (SUCCEEDED(XMLLoadFromFile(xml, filename)))
        succ = LoadFromXML(xml);
    }

    if (xml) xml->Release();
  }
  return succ;
}

bool CAlarmList::SaveToFile(wchar_t* filename)
{
  IXMLDOMDocument* xml;
  bool succ = false;
  if (SUCCEEDED(XMLCreateDocument(&xml)))
  {
    if (SaveToXML(xml))
      succ = SUCCEEDED(XMLSaveToFile(xml, filename));
  }

  if (xml) xml->Release();
  return succ;
}

CAlarm* CAlarmList::Get(int i)
{
  return (CAlarm*)m_list->Get(i);
}

int CAlarmList::Count()
{
  return m_list->GetCount();
}

int CAlarmList::Add(CAlarm* item)
{
  return m_list->Add(item);
}

void CAlarmList::Insert(int Index, CAlarm* Item)
{
  m_list->Insert(Index, Item);
}

void CAlarmList::Delete(int index)
{
  CAlarm* item = Get(index);
  delete item;
  return m_list->Delete(index);
}

void CAlarmList::Clear()
{
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CAlarm* item = (CAlarm*)m_list->Get(i);
    delete item;
  }
  m_list->Clear();
}
