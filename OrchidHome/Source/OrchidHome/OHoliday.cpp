/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：节假日处理单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OHoliday.cpp,v 1.17 2006/06/05 11:50:22 zjy Exp $
* 备    注：
*******************************************************************************/

#include "OHoliday.h"
#include "OUtils.h"
#include "OXMLUtils.h"

/************************************************************************/
/* 常量定义                                                             */
/************************************************************************/

static wchar_t* sLang = L"lang";
static wchar_t* sHolidays = L"holidays";
static wchar_t* sItem = L"item";
static wchar_t* sType = L"type";
static wchar_t* sLevel = L"level";
static wchar_t* sYear = L"year";
static wchar_t* sMonth = L"month";
static wchar_t* sDay = L"day";
static wchar_t* sWeek = L"week";
static wchar_t* sWeekDay = L"weekday";
static wchar_t* sText = L"text";
static wchar_t* sShortText = L"short";

static EnumDef HolidayTypes[] = {
  {htSolar, L"Solar"}, {htLunar, L"Lunar"}, {htWeek, L"Week"}, {htSolarBirth, L"SolarBirth"}, 
  {htLunarBirth, L"LunarBirth"}, {htSolarDay, L"SolarDay"}, {htLunarDay, L"LunarDay"}
};

/************************************************************************/
/* 节假日类                                                             */
/************************************************************************/

CHoliday::CHoliday()
{
  memset(&data, 0, sizeof(data));
  InitString(text); 
  InitString(stext);
}

CHoliday::~CHoliday()
{
  FreeString(text);
  FreeString(stext);
}

HRESULT CHoliday::LoadFromStream(IStream* pStm)
{
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, text)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, stext)))
    return E_FAIL;

  return S_OK;
}

HRESULT CHoliday::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, text)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, stext)))
    return E_FAIL;

  return S_OK;
}

void CHoliday::Clone(CHoliday* source)
{
  data = source->data;
  CloneString(text, source->text);
  CloneString(stext, source->stext);
}

/************************************************************************/
/* 节假日项列表类                                                       */
/************************************************************************/

OHOLIDAY_ITEM* CHolidayItems::Get(int Index)
{
  return (OHOLIDAY_ITEM*)CList::Get(Index);
}

OHOLIDAY_ITEM* CHolidayItems::Add()
{
  OHOLIDAY_ITEM* item = (OHOLIDAY_ITEM*)calloc(1, sizeof(OHOLIDAY_ITEM));
  CList::Add(item);
  return item;
}

int HolidayCompare(void* Item1, void* Item2)
{
  OHOLIDAY_ITEM* p1 = (OHOLIDAY_ITEM*)Item1;
  OHOLIDAY_ITEM* p2 = (OHOLIDAY_ITEM*)Item2;
  // 先按日期排序再按显示优先级排序
  if (p1->days > p2->days)
    return 1;
  else if (p1->days < p2->days)
    return -1;
  else if (p1->holiday->data.level > p2->holiday->data.level)
    return 1;
  else if (p1->holiday->data.level < p2->holiday->data.level)
    return -1;
  else if (p1->holiday->data.index > p2->holiday->data.index)
    return 1;
  else if (p1->holiday->data.index < p2->holiday->data.index)
    return -1;
  else
    return 0;
}

void CHolidayItems::Sort()
{
  CList::Sort(HolidayCompare);
}

void CHolidayItems::Notify(void* Ptr, TListNotification Action)
{
  if (Action == CList::lnDeleted)
  {
    OHOLIDAY_ITEM* item = (OHOLIDAY_ITEM*)Ptr;
    free(item);
  }
}

/************************************************************************/
/* 节假日列表类                                                         */
/************************************************************************/

CHolidayList::CHolidayList(int langid)
{
  m_list = new CList;
  m_lid = langid;
}

CHolidayList::~CHolidayList()
{
  Clear();
  delete m_list;
}

HRESULT CHolidayList::LoadFromStream(IStream* pStm)
{
  int cnt;

  Clear();
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;

  if (FAILED(pStm->Read(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (; cnt; cnt--)
  {
    CHoliday* item = new CHoliday;
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

HRESULT CHolidayList::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;

  int cnt = m_list->GetCount();
  if (FAILED(pStm->Write(&cnt, sizeof(int), 0)))
    return E_FAIL;

  for (int i = 0; i < cnt; i++)
  {
    CHoliday* item = (CHoliday*)m_list->Get(i);
    if (FAILED(item->SaveToStream(pStm)))
      return E_FAIL;
  }

  return S_OK;
}

HRESULT CHolidayList::ReadItem(IXMLDOMNode* node, CHoliday* item)
{
  IXMLDOMNamedNodeMap* nodemap = NULL;
  node->get_attributes(&nodemap);
  if (nodemap)
  {
    if (FAILED(XMLGetEnumAttribute(nodemap, sType, &item->data.type, HolidayTypes, LENOF(HolidayTypes))))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sLevel, &item->data.level)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sYear, &item->data.year)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sMonth, &item->data.month)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sDay, &item->data.day)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sWeek, &item->data.week)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(nodemap, sWeekDay, &item->data.weekday)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(nodemap, sText, &item->text)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(nodemap, sShortText, &item->stext)))
      return E_FAIL;
    nodemap->Release();
    nodemap = NULL;
  }

  return S_OK;
}

HRESULT CHolidayList::AppendTextNode(IXMLDOMDocument* xml, IXMLDOMElement* element, wchar_t* text)
{
  IXMLDOMText* node = NULL;
  xml->createTextNode(text, &node);
  if (node)
    return element->appendChild(node, NULL);
  else
    return S_FALSE;
}

HRESULT CHolidayList::SaveItem(IXMLDOMDocument* xml, IXMLDOMNode* node, CHoliday* item)
{
  IXMLDOMNamedNodeMap* nodemap = NULL;
  node->get_attributes(&nodemap);
  if (nodemap)
  {
    if (FAILED(XMLSetEnumAttribute(xml, nodemap, sType, item->data.type, HolidayTypes, LENOF(HolidayTypes))))
      return E_FAIL;
    if (FAILED(XMLSetIntAttribute(xml, nodemap, sLevel, item->data.level)))
      return E_FAIL;
    if (item->data.type != htWeek)
    {
      if (item->data.type != htSolar && item->data.type != htLunar)
        if (FAILED(XMLSetIntAttribute(xml, nodemap, sYear, item->data.year)))
          return E_FAIL;
      if (FAILED(XMLSetIntAttribute(xml, nodemap, sMonth, item->data.month)))
        return E_FAIL;
      if (FAILED(XMLSetIntAttribute(xml, nodemap, sDay, item->data.day)))
        return E_FAIL;
    }
    else
    {
      if (FAILED(XMLSetIntAttribute(xml, nodemap, sMonth, item->data.month)))
        return E_FAIL;
      if (FAILED(XMLSetIntAttribute(xml, nodemap, sWeek, item->data.week)))
        return E_FAIL;
      if (FAILED(XMLSetIntAttribute(xml, nodemap, sWeekDay, item->data.weekday)))
        return E_FAIL;
    }
    if (FAILED(XMLSetStrAttribute(xml, nodemap, sText, item->text)))
      return E_FAIL;
    if (FAILED(XMLSetStrAttribute(xml, nodemap, sShortText, item->stext)))
      return E_FAIL;
    nodemap->Release();
    nodemap = NULL;
  }

  return S_OK;
}

// 判断语言 ID 是否匹配
HRESULT CHolidayList::CheckLangAttr(IXMLDOMNode* pNode)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;
  wchar_t langstr[64];
  int lang = 0;
  langstr[0] = 0;

  if (FAILED(pNode->get_attributes(&pNodeMap)))
    return E_FAIL;

  if (pNodeMap)
  {
    if (FAILED(XMLGetStrAttribute(pNodeMap, sLang, langstr, 64)))
      return E_FAIL;
    if (langstr[0]) 
      lang = _wtoi(langstr);

    pNodeMap->Release();
  }

  if (!langstr[0] || lang == m_lid)
    return S_OK;
  else
    return S_FALSE;
}

bool CHolidayList::LoadFromXML(IXMLDOMDocument* xml)
{
  IXMLDOMNode* root = NULL;
  IXMLDOMNode* node = NULL;

  Clear();
  xml->get_firstChild(&root);
  while (root)
  {
    if (XMLNodeNameIs(root, sHolidays))
    {
      root->get_firstChild(&node);
      while (node)
      {
        if ((CheckLangAttr(node) == S_OK) && XMLNodeNameIs(node, sItem))
        {
          CHoliday* holiday = new CHoliday;
          if (SUCCEEDED(ReadItem(node, holiday)))
          {
            m_list->Add(holiday);
          }
          else
          {
            delete holiday;
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

  UpdateItemIndex();

  return true;
}

bool CHolidayList::SaveToXML(IXMLDOMDocument* xml)
{
  IXMLDOMElement* root = NULL;
  IXMLDOMElement* node = NULL;
  IXMLDOMProcessingInstruction* procnode = NULL;

  xml->createProcessingInstruction(L"xml", L"version='1.0'", &procnode);
  if (procnode)
    xml->appendChild(procnode, NULL);

  xml->createElement(sHolidays, &root);
  if (root)
  {
    xml->putref_documentElement(root);
    for (int i = 0; i < m_list->GetCount(); i++)
    {
      xml->createElement(sItem, &node);
      if (node)
      {
        AppendTextNode(xml, root, L"\r\n\t");
        if (SUCCEEDED(SaveItem(xml, node, (CHoliday*)m_list->Get(i))))
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

bool CHolidayList::LoadFromFile(wchar_t* filename)
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

bool CHolidayList::SaveToFile(wchar_t* filename)
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

CHoliday* CHolidayList::Get(int i)
{
  return (CHoliday*)m_list->Get(i);
}

int CHolidayList::Count()
{
  return m_list->GetCount();
}

int CHolidayList::Add(CHoliday* item)
{
  return m_list->Add(item);
}

void CHolidayList::Insert(int Index, CHoliday* Item)
{
  m_list->Insert(Index, Item);
}

void CHolidayList::Delete(int index)
{
  CHoliday* item = Get(index);
  delete item;
  return m_list->Delete(index);
}

void CHolidayList::Clear()
{
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CHoliday* item = (CHoliday*)m_list->Get(i);
    delete item;
  }
  m_list->Clear();
}

void CHolidayList::UpdateItemIndex()
{
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CHoliday* item = (CHoliday*)m_list->Get(i);
    item->data.index = i;
  }
}

void CHolidayList::GetHoliday(CHolidayItems* items, ODATE* sdate, ODATE* edate, int level)
{
  int sday, eday, days;
  bool valid;
  EncodeDate(sdate->year, sdate->month, sdate->day, &sday);
  EncodeDate(edate->year, edate->month, edate->day, &eday);
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CHoliday* item = (CHoliday*)m_list->Get(i);
    if (item->data.level <= level)
    {
      valid = false;
      switch (item->data.type)
      {
      case htSolar:
      case htSolarBirth:
      case htSolarDay:
        for (int y = sdate->year; y <= edate->year; y++)
        {
          // 检查公历节日是否满足条件
          if (EncodeDate(y, item->data.month, item->data.day, &days))
            if (days >= sday && days <= eday && (y >= item->data.year || item->data.type == htSolar))
            {
              valid = true;
              break;
            }
        }
    	  break;
      case htLunar:
      case htLunarBirth:
      case htLunarDay:
        for (int y = sdate->year - 1; y <= edate->year; y++)
        {
          OLUNAR lunar;
          ODATE date;
          memset(&lunar, 0, sizeof(lunar));
          lunar.year = y;
          lunar.month = item->data.month;
          lunar.day = item->data.day;
          // 检查农历节日是否满足条件
          if (LunarToDate(&lunar, &date) && EncodeDate(date.year, date.month, date.day, &days))
            if (days >= sday && days <= eday && (y >= item->data.year || item->data.type == htLunar))
            {
              valid = true;
              break;
            }
        }
        break;
      case htWeek:
        for (int y = sdate->year; y <= edate->year; y++)
        {
          if (EncodeDate(y, item->data.month, 1, &days))
          {
            int day;
            // 检查星期节日是否满足条件
            if (GetDateFromMonthWeek(y, item->data.month, item->data.week, item->data.weekday, &day))
            {
              days += day - 1;
              if (days >= sday && days <= eday)
              {
                valid = true;
                break;
              }
            }
          }
        }
        break;
      }

      if (valid)
      {
        // 增加到列表中
        OHOLIDAY_ITEM* p = items->Add();
        p->holiday = item;
        p->days = days;
        DecodeDate(days, &p->date.year, &p->date.month, &p->date.day);
        GetLunarInfo(&p->date, &p->lunar);
      }
    }
  }
}
