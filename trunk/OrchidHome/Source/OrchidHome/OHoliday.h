/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：节假日处理头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OHoliday.h,v 1.11 2006/06/05 11:50:22 zjy Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <windows.h>
#include <memory.h>
#include <msxml.h>
#include <ocidl.h>
#include "OCalendar.h"
#include "OList.h"
#include "OHomeClasses.h"

/************************************************************************/
/* 节假日类                                                             */
/************************************************************************/

#define htSolar          0
#define htLunar          1
#define htWeek           2
#define htSolarBirth     3
#define htLunarBirth     4
#define htSolarDay       5
#define htLunarDay       6
#define MAX_HOLIDAY      6

struct OHOLIDAY 
{
  int type;
  int level;
  int month;
  int year;
  int day;
  int week;
  int weekday;
  int index;
};

class CHoliday : public CPersistent
{
public:
  CHoliday();
  ~CHoliday();

  OHOLIDAY data;
  wchar_t* text;
  wchar_t* stext;

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
  void Clone(CHoliday* source);
};

/************************************************************************/
/* 节假日项列表类                                                       */
/************************************************************************/

struct OHOLIDAY_ITEM 
{
  CHoliday* holiday;
  ODATE date;
  OLUNAR lunar;
  int days;
};

class CHolidayItems : public CList
{
public:
  OHOLIDAY_ITEM* Get(int Index);
  OHOLIDAY_ITEM* Add();
  void Sort();

protected:
  void Notify(void* Ptr, TListNotification Action);

};

/************************************************************************/
/* 节假日列表类                                                         */
/************************************************************************/

class CHolidayList : public CPersistent
{
public:
  CHolidayList(int langid);
  ~CHolidayList();

  bool LoadFromXML(IXMLDOMDocument* xml);
  bool SaveToXML(IXMLDOMDocument* xml);

  bool LoadFromFile(wchar_t* filename);
  bool SaveToFile(wchar_t* filename);

  CHoliday* Get(int i);
  int Count();
  int Add(CHoliday* item);
  void Insert(int Index, CHoliday* Item);
  void Delete(int index);
  void Clear();
  void UpdateItemIndex();

  void GetHoliday(CHolidayItems* items, ODATE* sdate, ODATE* edate, int level);

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);

protected:
  CList* m_list;
  LANGID m_lid;

  HRESULT ReadItem(IXMLDOMNode* node, CHoliday* item);
  HRESULT SaveItem(IXMLDOMDocument* xml, IXMLDOMNode* node, CHoliday* item);
  HRESULT AppendTextNode(IXMLDOMDocument* xml, IXMLDOMElement* element, wchar_t* text);
  HRESULT CheckLangAttr(IXMLDOMNode* pNode);

private:
};
