/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：插件基础数据类单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OHomeClasses.cpp,v 1.32 2006/06/08 05:38:38 zjy Exp $
* 备    注：
*******************************************************************************/

#include "OHomeData.h"
#include "OSysInfo.h"

/************************************************************************/
/* 常量变量定义                                                         */
/************************************************************************/

// 减少点击计数的天数
#define CLICK_KEEP_DAYS   7
#define CHECK_ICON_TIME   30000

// 字符串定义
static wchar_t* sClickCount = L"ClickCount";
static wchar_t* sClickDays = L"ClickDays";

/************************************************************************/
/* 流持久化类                                                           */
/************************************************************************/

#define STRM_FLAG_STRING  0x71
#define STRM_FLAG_BLOCK   0x72
#define STRM_FLAG_OBJECT  0x73

HRESULT CPersistent::LoadFromStream(IStream* pStm)
{
  byte flag;
  if (FAILED(pStm->Read(&flag, sizeof(flag), 0)) || flag != STRM_FLAG_OBJECT)
    return E_FAIL;
  return S_OK;
}

HRESULT CPersistent::SaveToStream(IStream* pStm)
{
  byte flag = STRM_FLAG_OBJECT;
  if (FAILED(pStm->Write(&flag, sizeof(flag), 0)))
    return E_FAIL;
  return S_OK;
}

HRESULT CPersistent::DoLoadString(IStream* pStm, wchar_t* &s)
{
  byte flag;
  int len;
  if (FAILED(pStm->Read(&flag, sizeof(flag), 0)) || flag != STRM_FLAG_STRING)
    return E_FAIL;
  if (FAILED(pStm->Read(&len, sizeof(len), 0)))
    return E_FAIL;

  if (!AllocString(s, len))
    return E_FAIL;
  if (len > 0)
  {
    if (FAILED(pStm->Read(s, len * sizeof(wchar_t), 0)))
      return E_FAIL;
  }

  return S_OK;
}

HRESULT CPersistent::DoSaveString(IStream* pStm, wchar_t* s)
{
  byte flag = STRM_FLAG_STRING;
  int len = s ? wcslen(s) : 0;
  if (FAILED(pStm->Write(&flag, sizeof(flag), 0)))
    return E_FAIL;
  if (FAILED(pStm->Write(&len, sizeof(len), 0)))
    return E_FAIL;

  if (len > 0)
  {
    if (FAILED(pStm->Write(s, len * sizeof(wchar_t), 0)))
      return E_FAIL;
  }

  return S_OK;
}

HRESULT CPersistent::DoLoadBlock(IStream* pStm, void* data, int size)
{
  byte flag;
  int len;
  if (FAILED(pStm->Read(&flag, sizeof(flag), 0)) || flag != STRM_FLAG_BLOCK)
    return E_FAIL;
  if (FAILED(pStm->Read(&len, sizeof(len), 0)) || len != size)
    return E_FAIL;
  if (FAILED(pStm->Read(data, size, 0)))
    return E_FAIL;

  return S_OK;
}

HRESULT CPersistent::DoSaveBlock(IStream* pStm, void* data, int size)
{
  byte flag = STRM_FLAG_BLOCK;
  if (FAILED(pStm->Write(&flag, sizeof(flag), 0)))
    return E_FAIL;
  if (FAILED(pStm->Write(&size, sizeof(size), 0)))
    return E_FAIL;
  if (FAILED(pStm->Write(data, size, 0)))
    return E_FAIL;

  return S_OK;
}

/************************************************************************/
/* 倒数天数类                                                             */
/************************************************************************/

CDownCounter::CDownCounter()
{
  memset(&date, 0, sizeof(date));
  memset(&time, 0, sizeof(time));
  InitString(name);
  InitString(text);
}

CDownCounter::~CDownCounter()
{
  FreeString(name);
  FreeString(text);
}

HRESULT CDownCounter::LoadFromStream(IStream* pStm)
{
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadBlock(pStm, &date, sizeof(date))))
    return E_FAIL;
  if (FAILED(DoLoadBlock(pStm, &time, sizeof(time))))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, name)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, text)))
    return E_FAIL;

  return S_OK;
}

HRESULT CDownCounter::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveBlock(pStm, &date, sizeof(date))))
    return E_FAIL;
  if (FAILED(DoSaveBlock(pStm, &time, sizeof(time))))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, name)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, text)))
    return E_FAIL;

  return S_OK;
}

/************************************************************************/
/* 颜色类                                                               */
/************************************************************************/

CHomeColor::CHomeColor()
{
  InitString(name);
  color = COLOR_TRANSPARENT;
}

CHomeColor::~CHomeColor()
{
  FreeString(name);
}

HRESULT CHomeColor::LoadFromStream(IStream* pStm)
{
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, name)))
    return E_FAIL;

  return S_OK;
}

HRESULT CHomeColor::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, name)))
    return E_FAIL;

  return S_OK;
}

void CHomeColor::Clone(CHomeColor* source)
{
  CloneString(name, source->name);
  color = source->color;
}

/************************************************************************/
/* 字体类                                                               */
/************************************************************************/

CFont::CFont()
{
  memset(&data, 0, sizeof(data));
  data.shadowx = 1;
  data.shadowy = 1;
  InitString(fontface);
  color = new CHomeColor();
  bgcolor = new CHomeColor();
  shadowcolor = new CHomeColor();
}

CFont::~CFont()
{
  FreeString(fontface);
  delete color;
  delete bgcolor;
  delete shadowcolor;
}

HRESULT CFont::LoadFromStream(IStream* pStm)
{
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, fontface)))
    return E_FAIL;
  if (FAILED(color->LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(bgcolor->LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(shadowcolor->LoadFromStream(pStm)))
    return E_FAIL;

  return S_OK;
}

HRESULT CFont::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, fontface)))
    return E_FAIL;
  if (FAILED(color->SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(bgcolor->SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(shadowcolor->SaveToStream(pStm)))
    return E_FAIL;

  return S_OK;
}

void CFont::Clone(CFont* source)
{
  data = source->data;
  CloneString(fontface, source->fontface);
  color->Clone(source->color);
  bgcolor->Clone(source->bgcolor);
  shadowcolor->Clone(source->shadowcolor);
}

/************************************************************************/
/* 快捷方式类                                                           */
/************************************************************************/

CShortCut::CShortCut(CElement* aParent)
{
  parent = aParent;
  memset(&data, 0, sizeof(data));
  data.cmd = CMD_OPEN;
  InitString(title);
  InitString(desc);
  InitString(target);
  InitString(args);
  InitString(iconfile);
  InitString(relbar);
  icon = NULL;
  dir = NULL;
  plugin = NULL;
  m_tick = 0;
  m_iconload = false;
  clickcount = 0;
  clickdays = 0;
}

CShortCut::~CShortCut()
{
  if (icon) DestroyIcon(icon);
  FreeString(title);
  FreeString(desc);
  FreeString(target);
  FreeString(args);
  FreeString(iconfile);
  FreeString(relbar);
}

HRESULT CShortCut::LoadFromStream(IStream* pStm)
{
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, title)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, desc)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, target)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, args)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, iconfile)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, relbar)))
    return E_FAIL;

  UpdateTitle();

  LoadClickCount();

  return S_OK;
}

HRESULT CShortCut::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, title)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, desc)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, target)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, args)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, iconfile)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, relbar)))
    return E_FAIL;

  return S_OK;
}

HRESULT CShortCut::DoAction(bool &repaint)
{
  HRESULT ret = S_OK;

  repaint = false;
  if (data.confirm)
  {
    wchar_t buff[255];
    wsprintf(buff, sMsgExecConfirm, title);
    if (MessageBox(NULL, buff, NULL, MB_ICONQUESTION | MB_YESNO) == IDNO)
      return ret;
  }

  switch(data.cmd)
  {
  case CMD_OPEN:
    // 运行程序或打开文档
    if (target[0])
      ExecuteFile(target, args);
    break;
  case CMD_CALLPLUGIN:
    if (plugin)
    {
      PluginEvent ppe;
      memset(&ppe, 0, sizeof(ppe));
      ppe.idEvent = PE_ACTION;
      ret = plugin->OnEvent(&ppe);
    }
    break;
  case CMD_SHOWMAIN:
    HomeData->sysinfo->ShowMain();
    break;
  case CMD_SHOWCALENDAR:
    HomeData->sysinfo->ShowCalendar();
    break;
  case CMD_SHOWALARMS:
    HomeData->sysinfo->ShowAlarms();
    break;
  case CMD_SHOWUSERHOLIDAY:
    HomeData->sysinfo->ShowUserHoliday();
    break;
  case CMD_SHOWTOOLS:
    HomeData->sysinfo->ShowTools();
    break;
  case CMD_SHOWCALINFO:
    HomeData->sysinfo->ShowCalendarInfo();
    break;
  case CMD_SHOWHOLIDAYINFO:
    HomeData->sysinfo->ShowHolidayInfo();
    break;
  case CMD_SHOWPOWERINFO:
    HomeData->sysinfo->ShowPowerInfo();
    break;
  case CMD_SHOWMEMORYINFO:
    HomeData->sysinfo->ShowMemoryInfo();
    break;
  case CMD_SHOWABOUT:
    HomeData->sysinfo->ShowAbout();
    break;
  case CMD_SHOWOPTION:
    HomeData->sysinfo->ShowOption();
    break;
  case CMD_RESTART:
    RestartWindows();
    break;
  case CMD_SHUTDOWN:
    ShutdownWindows();
    break;
  }

  IncClickCount(repaint);

  return ret;
}

void CShortCut::UpdateTitle()
{
  if (target[0] && !title[0])
  {
    SHFILEINFO info;
    memset(&info, 0, sizeof(info));
    if (SHGetFileInfo(target, 0, &info, sizeof(info), SHGFI_DISPLAYNAME))
      CloneString(title, info.szDisplayName);
  }

  if (title[0] && !desc[0])
  {
    CloneString(desc, title);
  }
}

bool CShortCut::UpdateIcon(int iconsize)
{
  if (iconfile[0])
  {
    // 使用图标文件
    if (!m_iconload && !icon)
    {
      icon = LoadIconFromFile(iconfile, data.iconindex, iconsize == ICON_LARGE ?
        HomeData->iconsize : HomeData->smiconsize);
      m_iconload = true;
    }
  }

  if (target[0] && (!m_tick || abs(GetTickCount() - m_tick) >= CHECK_ICON_TIME))
  {
    // 释放原来的图标
    if (icon)
    {
      DestroyIcon(icon);
      icon = NULL;
    }

    // 获取新图标
    SHFILEINFO info;
    memset(&info, 0, sizeof(info));
    if (SHGetFileInfo(target, 0, &info, sizeof(info), SHGFI_ICON | (iconsize == ICON_LARGE ?
SHGFI_LARGEICON : SHGFI_SMALLICON)))
                  icon = info.hIcon;
    m_tick = GetTickCount();
  }

  return icon != NULL;
}

bool CShortCut::ClickCountValid()
{
  return parent && (parent->GetKind() == CElement::ShortCutBar) && (data.cmd != CMD_OPEN || target[0]) && title[0];
}

void CShortCut::CheckShort(CShortCut* shortcut, bool &repaint)
{
  if (parent && parent->GetKind() == CElement::ShortCutBar)
  {
    CShortCutBar* bar = (CShortCutBar*)parent;
    if (bar->bar.autosort)
    {
      if (shortcut)
      {
        CloneString(bar->m_lasttarget, target);
        CloneString(bar->m_lasttitle, title);
        bar->m_lastgroup = shortcut->data.group;
      }
      bar->SortShortCuts();
      repaint = true;
    }
  }
}

void CShortCut::IncClickCount(bool &repaint)
{
  if (ClickCountValid())
  {
    clickcount++;
    clickdays = HomeData->alldays;
    SaveClickCount();
    CheckShort(this, repaint);
  }
}

void CShortCut::CheckClickCount(bool &repaint)
{
  if (ClickCountValid() && (clickcount > 0) && (abs(HomeData->alldays - clickdays) > CLICK_KEEP_DAYS))
  {
    // 七天内未点击，点击率降到原来的一半
    clickcount = clickcount >> 1;
    clickdays = HomeData->alldays;
    SaveClickCount();
    CheckShort(NULL, repaint);
  }
}

void CShortCut::LoadClickCount()
{
  if (ClickCountValid())
  {
    wchar_t buff[MAX_PATH];
    clickdays = clickcount = 0;
    wsprintf(buff, L"%s.%d.%s", parent->name, data.group, title);
    HomeData->registry->ReadInt(sClickCount, buff, clickcount);
    HomeData->registry->ReadInt(sClickDays, buff, clickdays);
  }
}

void CShortCut::SaveClickCount()
{
  if (ClickCountValid())
  {
    wchar_t buff[MAX_PATH];
    wsprintf(buff, L"%s.%d.%s", parent->name, data.group, title);
    HomeData->registry->WriteInt(sClickCount, buff, clickcount);
    HomeData->registry->WriteInt(sClickDays, buff, clickdays);
  }
}

/************************************************************************/
/* 显示元素基类                                                         */
/************************************************************************/

CElement::CElement()
{
  memset(&data, 0, sizeof(data));
  InitString(name);
  font = new CFont;
  selfont = new CFont;
}

CElement::~CElement()
{
  FreeString(name);
  delete font;
  delete selfont;
}

HRESULT CElement::LoadFromStream(IStream* pStm)
{
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, name)))
    return E_FAIL;
  if (FAILED(font->LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(selfont->LoadFromStream(pStm)))
    return E_FAIL;

  return S_OK;
}

HRESULT CElement::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, name)))
    return E_FAIL;
  if (FAILED(font->SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(selfont->SaveToStream(pStm)))
    return E_FAIL;

  return S_OK;
}

/************************************************************************/
/* 普通元素基类                                                         */
/************************************************************************/

CStdElement::CStdElement()
{
  shortcut = new CShortCut(this);
}

CStdElement::~CStdElement()
{
  delete shortcut;
}

HRESULT CStdElement::LoadFromStream(IStream* pStm)
{
  if (FAILED(CElement::LoadFromStream(pStm)))
    return E_FAIL;

  if (FAILED(shortcut->LoadFromStream(pStm)))
    return E_FAIL;

  return S_OK;
}

HRESULT CStdElement::SaveToStream(IStream* pStm)
{
  if (FAILED(CElement::SaveToStream(pStm)))
    return E_FAIL;

  if (FAILED(shortcut->SaveToStream(pStm)))
    return E_FAIL;

  return S_OK;
}

bool CStdElement::HasShortCut()
{
  if (shortcut->data.exist)
  {
    if (shortcut->data.cmd != CMD_CALLPLUGIN)
    {
      return true;
    }
    else if (shortcut->plugin)
    {
      BOOL ret = false;
      if (SUCCEEDED(shortcut->plugin->GetSelectability(&ret)))
        return ret ? true : false;
    }
  }
  return false;
}

CShortCut* CStdElement::GetFirstShortCut()
{
  if (HasShortCut())
    return shortcut;
  else
    return NULL;
}

CShortCut* CStdElement::GetLastShortCut()
{
  if (HasShortCut())
    return shortcut;
  else
    return NULL;
}

bool CStdElement::GetNextShortCut(CShortCut* &shortcut)
{
  return false;
}

bool CStdElement::GetPrevShortCut(CShortCut* &shortcut)
{
  return false;
}

int CStdElement::GetShortCutCount()
{
  return HasShortCut() ? 1 : 0;
}

int CStdElement::GetShortCutIndex(CShortCut* shortcut)
{
  return (this->shortcut == shortcut)  ? 1 : 0;
}

bool CStdElement::SetGroup(int group, CShortCut* &shortcut)
{
  return false;
}

/************************************************************************/
/* 文本节点类                                                           */
/************************************************************************/

CTextNode::CTextNode()
{
  memset(&data, 0, sizeof(data));
  InitString(ls);
  InitString(rs);
  InitString(pluginname);
  InitString(relname);
  InitString(text);
  InitString(replace);
  color = new CHomeColor();
  selcolor = new CHomeColor();
}

CTextNode::~CTextNode()
{
  FreeString(ls);
  FreeString(rs);
  FreeString(pluginname);
  FreeString(relname);
  FreeString(text);
  FreeString(replace);
  delete color;
  delete selcolor;
}

HRESULT CTextNode::LoadFromStream(IStream* pStm)
{
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, ls)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, rs)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, pluginname)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, relname)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, text)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, replace)))
    return E_FAIL;
  if (FAILED(color->LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(selcolor->LoadFromStream(pStm)))
    return E_FAIL;

  return S_OK;
}

HRESULT CTextNode::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, ls)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, rs)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, pluginname)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, relname)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, text)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, replace)))
    return E_FAIL;
  if (FAILED(color->SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(selcolor->SaveToStream(pStm)))
    return E_FAIL;

  return S_OK;
}

/************************************************************************/
/* 标签类                                                               */
/************************************************************************/

CLabel::CLabel()
{
  nodes = new CList;
}

CLabel::~CLabel()
{
  Clear();
  delete nodes;
}

CElement::TElementKind CLabel::GetKind()
{
  return CElement::Label;
}

void CLabel::Clear()
{
  for (int i = 0; i < nodes->GetCount(); i++)
  {
    CTextNode* node = (CTextNode*)nodes->Get(i);
    delete node;
  }
  nodes->Clear();
}

HRESULT CLabel::LoadFromStream(IStream* pStm)
{
  int cnt;
  CTextNode* node;

  if (FAILED(CStdElement::LoadFromStream(pStm)))
    return E_FAIL;

  if (FAILED(pStm->Read(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (; cnt; cnt--)
  {
    node = new CTextNode;
    if (FAILED(node->LoadFromStream(pStm)))
    {
      delete node;
      return E_FAIL;
    }
    nodes->Add(node);
  }

  return S_OK;
}

HRESULT CLabel::SaveToStream(IStream* pStm)
{
  int cnt;

  if (FAILED(CStdElement::SaveToStream(pStm)))
    return E_FAIL;

  cnt = nodes->GetCount();
  if (FAILED(pStm->Write(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (int i = 0; i < nodes->GetCount(); i++)
  {
    CTextNode* node = (CTextNode*)nodes->Get(i);
    if (FAILED(node->SaveToStream(pStm)))
      return E_FAIL;
  }

  return S_OK;
}

int CLabel::GetMaxY()
{
  wchar_t buff[1024];
  int maxy = 0;
  for (int i = 0; i < nodes->GetCount(); i++)
  {
    HomeData->GetTextNodeValue((CTextNode*)nodes->Get(i), buff, 1024);
    if (buff[0])
    {
      maxy = data.y + max(font->data.height, data.h);
      break;
    }
  }
  return maxy;
}

/************************************************************************/
/* 图像类                                                               */
/************************************************************************/

CImage::CImage()
{
  memset(&info, 0, sizeof(info));
  transcolor = new CHomeColor;
  InitString(filename);
  bmp = NULL;
  height = 0;
  width = 0;
}

CImage::~CImage()
{
  FreeImage();
  delete transcolor;
  FreeString(filename);
}

CElement::TElementKind CImage::GetKind()
{
  return CElement::Image;
}

HRESULT CImage::LoadFromStream(IStream* pStm)
{
  if (FAILED(CStdElement::LoadFromStream(pStm)))
    return E_FAIL;

  if (FAILED(DoLoadBlock(pStm, &info, sizeof(info))))
    return E_FAIL;
  if (FAILED(transcolor->LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, filename)))
    return E_FAIL;

  LoadImage(filename);

  return S_OK;
}

HRESULT CImage::SaveToStream(IStream* pStm)
{
  if (FAILED(CStdElement::SaveToStream(pStm)))
    return E_FAIL;

  if (FAILED(DoSaveBlock(pStm, &info, sizeof(info))))
    return E_FAIL;
  if (FAILED(transcolor->SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, filename)))
    return E_FAIL;

  return S_OK;
}

int CImage::GetMaxY()
{
  return height > 0 ? data.y + height + info.border * 2 : 0;
}

void CImage::LoadImage(wchar_t* AName)
{
  FreeImage();
  bmp = SHLoadDIBitmap(AName);
  if (bmp)
  {
    BITMAP bm;
    GetObject(bmp, sizeof(BITMAP), &bm);
    width = bm.bmWidth;
    height = bm.bmHeight;
  }
}

void CImage::FreeImage()
{
  if (bmp)
  {
    DeleteObject(bmp);
    bmp = NULL;
    height = 0;
    width = 0;
  }
}

/************************************************************************/
/* 快捷方式目录类                                                       */
/************************************************************************/

CShortCutDir::CShortCutDir()
{
  memset(&data, 0, sizeof(data));
  data.group = GROUP_ALL;
  data.subdir = false;
  freesize = (uint)-1;
  InitString(dir);
  InitString(mask);
}

CShortCutDir::~CShortCutDir()
{
  FreeString(dir);
  FreeString(mask);
}

HRESULT CShortCutDir::LoadFromStream(IStream* pStm)
{
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, dir)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, mask)))
    return E_FAIL;

  return S_OK;
}

HRESULT CShortCutDir::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, dir)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, mask)))
    return E_FAIL;

  return S_OK;
}

/************************************************************************/
/* 快捷方式组类                                                         */
/************************************************************************/

CShortCutBar::CShortCutBar()
{
  memset(&bar, 0, sizeof(bar));
  bar.autosort = false;
  bar.lastontop = true;
  nodes = new CList;
  dirs = new CList;
  topindex = 0;
  m_lastgroup = 0;
  InitString(m_lasttarget);
  InitString(m_lasttitle);
}

CShortCutBar::~CShortCutBar()
{
  Clear();
  delete nodes;
  delete dirs;
  FreeString(m_lasttarget);
  FreeString(m_lasttitle);
}

CElement::TElementKind CShortCutBar::GetKind()
{
  return CElement::ShortCutBar;
}

void CShortCutBar::Clear()
{
  for (int i = 0; i < nodes->GetCount(); i++)
  {
    CShortCut* shortcut = (CShortCut*)nodes->Get(i);
    delete shortcut;
  }
  nodes->Clear();

  for (int i = 0; i < dirs->GetCount(); i++)
  {
    CShortCutDir* dir = (CShortCutDir*)dirs->Get(i);
    delete dir;
  }
  dirs->Clear();
}

HRESULT CShortCutBar::LoadFromStream(IStream* pStm)
{
  int cnt;

  if (FAILED(CElement::LoadFromStream(pStm)))
    return E_FAIL;

  if (FAILED(DoLoadBlock(pStm, &bar, sizeof(bar))))
    return E_FAIL;

  if (FAILED(pStm->Read(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (; cnt; cnt--)
  {
    CShortCut* shortcut = new CShortCut(this);
    if (FAILED(shortcut->LoadFromStream(pStm)))
    {
      delete shortcut;
      return E_FAIL;
    }
    nodes->Add(shortcut);
  }

  if (FAILED(pStm->Read(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (; cnt; cnt--)
  {
    CShortCutDir* dir = new CShortCutDir;
    if (FAILED(dir->LoadFromStream(pStm)))
    {
      delete dir;
      return E_FAIL;
    }
    dirs->Add(dir);
  }

  if (bar.grouped && bar.group != GROUP_ALL)
  {
    CShortCut* shortcut;
    SetGroup(bar.grouped, shortcut);
  }
  else
  {
    UpdateShortCutsFromDir(0);
  }

  return S_OK;
}

HRESULT CShortCutBar::SaveToStream(IStream* pStm)
{
  int cnt;

  if (FAILED(CElement::SaveToStream(pStm)))
    return E_FAIL;

  if (FAILED(DoSaveBlock(pStm, &bar, sizeof(bar))))
    return E_FAIL;

  cnt = nodes->GetCount();
  if (FAILED(pStm->Write(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (int i = 0; i < nodes->GetCount(); i++)
  {
    CShortCut* shortcut = (CShortCut*)nodes->Get(i);
    if (FAILED(shortcut->SaveToStream(pStm)))
      return E_FAIL;
  }

  cnt = dirs->GetCount();
  if (FAILED(pStm->Write(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (int i = 0; i < dirs->GetCount(); i++)
  {
    CShortCutDir* dir = (CShortCutDir*)dirs->Get(i);
    if (FAILED(dir->SaveToStream(pStm)))
      return E_FAIL;
  }

  return S_OK;
}

int CShortCutBar::GetMaxY()
{
  int hb;
  if (bar.mode == SM_TITLE)
    hb = font->data.height;
  else
    hb = (bar.iconsize == ICON_LARGE ? HomeData->iconsize : HomeData->smiconsize) + bar.iconspacing;
  return data.y + max(hb, data.h);
}

void CShortCutBar::DoGetShortCutsFromDir(CShortCutDir* dir, wchar_t* dirname, wchar_t* mask, int &idx, bool incsub)
{
  HANDLE hFind;
  WIN32_FIND_DATA fd;
  wchar_t path[MAX_PATH];

  // 取得路径名
  wsprintf(path, L"%s\\%s", dirname, mask);

  // 添加目录下的快捷方式
  hFind = FindFirstFile(path, &fd);
  while (hFind != INVALID_HANDLE_VALUE)
  {
    if ((fd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_HIDDEN)) == 0)
    {
      wchar_t buff[MAX_PATH];
      CShortCut* shortcut = new CShortCut(this);

      wsprintf(buff, L"%s\\%s", dirname, fd.cFileName);
      CloneString(shortcut->target, buff);

      shortcut->data.exist = true;
      shortcut->data.cmd = CMD_OPEN;
      shortcut->data.group = dir->data.group;
      shortcut->data.index = idx++;
      shortcut->dir = dir;
      shortcut->UpdateTitle();
      shortcut->LoadClickCount();

      nodes->Add(shortcut);
    }

    if (!FindNextFile(hFind, &fd))
    {
      FindClose(hFind);
      hFind = INVALID_HANDLE_VALUE;
    }
  }

  if (incsub)
  {
    // 搜索子目录
    wsprintf(path, L"%s\\*.*", dirname);

    // 添加子目录下的快捷方式
    hFind = FindFirstFile(path, &fd);
    while (hFind != INVALID_HANDLE_VALUE)
    {
      if (((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0) &&
        ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) &&
        (wcscmp(fd.cFileName, L".") != 0) && (wcscmp(fd.cFileName, L"..") != 0))
      {
        wchar_t buff[MAX_PATH];
        wsprintf(buff, L"%s\\%s", dirname, fd.cFileName);
        DoGetShortCutsFromDir(dir, buff, mask, idx, incsub);
      }

      if (!FindNextFile(hFind, &fd))
      {
        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
      }
    }
  }
}

void CShortCutBar::UpdateShortCutsFromDir(int group)
{
  for (int i = 0; i < dirs->GetCount(); i++)
  {
    CShortCutDir* dir = (CShortCutDir*)dirs->Get(i);

    // 未分组或分组号为所有组或目录分组号匹配
    if (!bar.grouped || group == GROUP_ALL || dir->data.group == group)
    {
      STORAGE_INFO info;
      GetStorageInfo(dir->dir, &info);

      // 目录内容改变时空间会发生变化
      if (info.free != dir->freesize)
      {
        dir->freesize = info.free;

        // 先删除当前目录关联的快捷方式
        for (int j = nodes->GetCount() - 1; j >= 0; j--)
        {
          CShortCut* shortcut = (CShortCut*)nodes->Get(j);
          if (shortcut->dir == dir)
          {
            nodes->Delete(j);
            delete shortcut;
          }
        }

        // 查找并添加快捷方式
        int index = dir->data.index;
        int tick;
        tick = GetTickCount();
        DoGetShortCutsFromDir(dir, dir->dir, dir->mask, index, dir->data.subdir);
      }
    }
  }

  SortShortCuts();
}

static bool _autosort = false;
static CShortCut* _lastshortcut = NULL;

static int ShortCutCompare(void* Item1, void* Item2)
{
  CShortCut* s1 = (CShortCut*)Item1;
  CShortCut* s2 = (CShortCut*)Item2;

  if (_autosort)
  {
    // 最后一次执行的快捷方式最优先
    if (s1 == _lastshortcut)
      return -1;
    else if (s2 == _lastshortcut)
      return 1;

    // 先按点击数排序
    if (s1->clickcount > s2->clickcount)
      return -1;
    else if (s1->clickcount < s2->clickcount)
      return 1;
  }

  // 按在 XML 中的索引号排序
  if (s1->data.index > s2->data.index)
    return 1;
  else if (s1->data.index < s2->data.index)
    return -1;
  else
    return 0;
}

void CShortCutBar::SortShortCuts()
{
  // 按点击排序
  _autosort = bar.autosort;

  // 因为快捷方式可能动态创建释放，所以使用快捷方式的内容来计算最后一次调用的快捷方式
  _lastshortcut = NULL;
  if (bar.autosort && bar.lastontop && (m_lasttarget[0] || m_lasttitle[0]))
  {
    for (int i = 0; i < nodes->GetCount(); i++)
    {
      CShortCut* shortcut = (CShortCut*)nodes->Get(i);
      if ((m_lastgroup == shortcut->data.group) && (wcscmp(m_lasttitle, shortcut->title) == 0)
        && (wcscmp(m_lasttarget, shortcut->target) == 0))
      {
        _lastshortcut = shortcut;
        break;
      }
    }
  }
  nodes->Sort(ShortCutCompare);
}

bool CShortCutBar::SetGroup(int group, CShortCut* &shortcut)
{
  topindex = 0;
  if (bar.grouped)
  {
    UpdateShortCutsFromDir(group);
    bar.group = group;
    shortcut = NULL;
    // 查找第一个有效快捷方式
    for (int i = 0; i < nodes->GetCount(); i++)
    {
      CShortCut* node = (CShortCut*)nodes->Get(i);
      if (group == GROUP_ALL || node->data.group == group)
      {
        shortcut = node;
        return true;
      }
    }
  }

  return false;
}

bool CShortCutBar::HasShortCut()
{
  if (bar.grouped)
  {
    for (int i = 0; i < nodes->GetCount(); i++)
    {
      CShortCut* shortcut = (CShortCut*)nodes->Get(i);
      if (bar.group == GROUP_ALL || shortcut->data.group == bar.group)
        return true;
    }
    return false;
  }
  else
  {
    return nodes->GetCount() > 0;
  }
}

CShortCut* CShortCutBar::GetFirstShortCut()
{
  if (bar.grouped)
  {
    for (int i = 0; i < nodes->GetCount(); i++)
    {
      CShortCut* node = (CShortCut*)nodes->Get(i);
      if (bar.group == GROUP_ALL || bar.group == node->data.group)
        return node;
    }
    return NULL;
  }
  else if (nodes->GetCount() > 0)
  {
    return (CShortCut*)nodes->First();
  }
  else
  {
    return NULL;
  }
}

CShortCut* CShortCutBar::GetLastShortCut()
{
  if (bar.grouped)
  {
    for (int i = nodes->GetCount() - 1; i >= 0; i--)
    {
      CShortCut* node = (CShortCut*)nodes->Get(i);
      if (bar.group == GROUP_ALL || bar.group == node->data.group)
        return node;
    }
    return NULL;
  }
  else if (nodes->GetCount() > 0)
  {
    return (CShortCut*)nodes->Last();
  }
  else
  {
    return NULL;
  }
}

bool CShortCutBar::GetNextShortCut(CShortCut* &shortcut)
{
  int idx;
  idx = nodes->IndexOf(shortcut);
  if (idx >= 0)
  {
    for (int i = idx + 1; i < nodes->GetCount(); i++)
    {
      CShortCut* node = (CShortCut*)nodes->Get(i);
      if (!bar.grouped || bar.group == GROUP_ALL || bar.group == node->data.group)
      {
        shortcut = node;
        return true;
      }
    }
  }
  return false;
}

bool CShortCutBar::GetPrevShortCut(CShortCut* &shortcut)
{
  int idx;
  idx = nodes->IndexOf(shortcut);
  if (idx >= 0)
  {
    for (int i = idx - 1; i >= 0; i--)
    {
      CShortCut* node = (CShortCut*)nodes->Get(i);
      if (!bar.grouped || bar.group == GROUP_ALL || bar.group == node->data.group)
      {
        shortcut = node;
        return true;
      }
    }
  }
  return false;
}

int CShortCutBar::GetShortCutCount()
{
  if (!bar.grouped || bar.group == GROUP_ALL)
  {
    return nodes->GetCount();
  }
  else
  {
    int ret = 0;
    for (int i = 0; i < nodes->GetCount(); i++)
    {
      CShortCut* node = (CShortCut*)nodes->Get(i);
      if (node->data.group == bar.group)
        ret++;
    }
    return ret;
  }
}

int CShortCutBar::GetShortCutIndex(CShortCut* shortcut)
{
  if (!bar.grouped || bar.group == GROUP_ALL)
  {
    return nodes->IndexOf(shortcut) + 1;
  }
  else
  {
    int ret = 0;
    for (int i = 0; i < nodes->GetCount(); i++)
    {
      CShortCut* node = (CShortCut*)nodes->Get(i);
      if (node->data.group == bar.group)
        ret++;
      if (node == shortcut)
        return ret;
    }
    return 0;
  }
}

/************************************************************************/
/* 子插件类                                                             */
/************************************************************************/

CChildPlugin::CChildPlugin()
{
  InitString(clsid);
  shortcut->data.exist = true;
  shortcut->data.cmd = CMD_CALLPLUGIN;
  plugin = NULL;
}

CChildPlugin::~CChildPlugin()
{
  FreeString(clsid);
  ReleasePlugin();
}

CElement::TElementKind CChildPlugin::GetKind()
{
  return CElement::ChildPlugin;
}

HRESULT CChildPlugin::LoadFromStream(IStream* pStm)
{
  if (FAILED(CStdElement::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, clsid)))
    return E_FAIL;

  return S_OK;
}

HRESULT CChildPlugin::SaveToStream(IStream* pStm)
{
  if (FAILED(CStdElement::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, clsid)))
    return E_FAIL;

  return S_OK;
}

void CChildPlugin::ReleasePlugin()
{
  if (plugin)
  {
    plugin->SetSite(NULL);
    plugin->Release();
    plugin = NULL;
    shortcut->plugin = NULL;
  }
}

int CChildPlugin::GetMaxY()
{
  int ret = 0;
  if (plugin)
    plugin->GetHeight(&ret);
  return ret;
}

/************************************************************************/
/* 插件参数类                                                           */
/************************************************************************/

CPluginParam::CPluginParam()
{
  memset(&data, 0, sizeof(data));
  InitString(name);
  font = new CFont;
  selfont = new CFont;
  bordercolor = new CHomeColor;
}

CPluginParam::~CPluginParam()
{
  FreeString(name);
  delete font;
  delete selfont;
  delete bordercolor;
}

HRESULT CPluginParam::LoadFromStream(IStream* pStm)
{
  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(DoLoadBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoLoadString(pStm, name)))
    return E_FAIL;
  if (FAILED(font->LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(selfont->LoadFromStream(pStm)))
    return E_FAIL;
  if (FAILED(bordercolor->LoadFromStream(pStm)))
    return E_FAIL;

  return S_OK;
}

HRESULT CPluginParam::SaveToStream(IStream* pStm)
{
  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(DoSaveBlock(pStm, &data, sizeof(data))))
    return E_FAIL;
  if (FAILED(DoSaveString(pStm, name)))
    return E_FAIL;
  if (FAILED(font->SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(selfont->SaveToStream(pStm)))
    return E_FAIL;
  if (FAILED(bordercolor->SaveToStream(pStm)))
    return E_FAIL;

  return S_OK;
}
