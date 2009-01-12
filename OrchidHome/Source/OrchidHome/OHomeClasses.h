/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：插件基础数据类单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OHomeClasses.h,v 1.30 2006/06/08 05:38:38 zjy Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <windows.h>
#include <memory.h>
#include <ocidl.h>
#include <home.h>

#include "OTypes.h"
#include "OCalendar.h"
#include "OCalInfoW.h"
#include "OSunRiseSet.h"
#include "OResStr.h"
#include "OUtils.h"
#include "ODebug.h"
#include "OList.h"
#include "OVersion.h"
#include "resource.h"

/************************************************************************/
/* 结构定义                                                             */
/************************************************************************/

// 经纬度信息结构
struct GEO_INFO
{
  double longitude;  // 经度
  double latitude;   // 纬度
  int zonetime;      // 时区
};

/************************************************************************/
/* 流持久化类                                                           */
/************************************************************************/

class CPersistent
{
public:
  CPersistent() {};
  virtual ~CPersistent() {};
  virtual HRESULT LoadFromStream(IStream* pStm);
  virtual HRESULT SaveToStream(IStream* pStm);
protected:
  HRESULT DoLoadString(IStream* pStm, wchar_t* &s);
  HRESULT DoSaveString(IStream* pStm, wchar_t* s);
  HRESULT DoLoadBlock(IStream* pStm, void* data, int size);
  HRESULT DoSaveBlock(IStream* pStm, void* data, int size);
};

/************************************************************************/
/* 倒数天数类                                                             */
/************************************************************************/

class CDownCounter : public CPersistent
{
public:
  CDownCounter();
  ~CDownCounter();

  wchar_t* name;
  wchar_t* text;
  ODATE date;
  OTIME time;

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
};

/************************************************************************/
/* 颜色类                                                               */
/************************************************************************/

class CHomeColor : public CPersistent
{
public:
  CHomeColor();
  ~CHomeColor();

  wchar_t* name;
  COLORREF color;

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
  void Clone(CHomeColor* source);
};

/************************************************************************/
/* 字体类                                                               */
/************************************************************************/

struct TFontInfo
{
  int height;
  int width;
  bool bold;
  bool italic;
  bool underline;
  bool cleartype;
  bool shadow;
  int shadowx;
  int shadowy;
  int xspace;
  int yspace;
};

class CFont : public CPersistent
{
public:
  CFont();
  ~CFont();

  TFontInfo data;
  wchar_t* fontface;
  CHomeColor* color;
  CHomeColor* bgcolor;
  CHomeColor* shadowcolor;

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
  void Clone(CFont* source);
};

/************************************************************************/
/* 快捷方式类                                                           */
/************************************************************************/

// 快捷方式命令
#define CMD_OPEN                   00
#define CMD_CALLPLUGIN             01

#define CMD_SHOWMAIN               10
#define CMD_SHOWCALENDAR           11
#define CMD_SHOWALARMS             12
#define CMD_SHOWUSERHOLIDAY        13
#define CMD_SHOWTOOLS              14

#define CMD_SHOWCALINFO            21
#define CMD_SHOWHOLIDAYINFO        22
#define CMD_SHOWPOWERINFO          23
#define CMD_SHOWMEMORYINFO         24

#define CMD_SHOWOPTION             31
#define CMD_SHOWABOUT              32

#define CMD_RESTART                91
#define CMD_SHUTDOWN               92

struct TShortCutInfo
{
  bool exist;
  int cmd;
  int relgroup;
  int iconindex;
  int group;
  int index;
  bool confirm;
};

class CShortCutDir;

class CElement;

class CShortCut : public CPersistent
{
  bool m_iconload;
  uint m_tick;
public:
  CShortCut(CElement* aParent);
  ~CShortCut();

  CElement* parent;
  TShortCutInfo data;
  wchar_t* title;
  wchar_t* desc;
  wchar_t* target;
  wchar_t* args;
  wchar_t* iconfile;
  wchar_t* relbar;
  CShortCutDir* dir;

  HICON icon;
  int clickcount;
  int clickdays;

  IHomePlugin* plugin;

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);

  HRESULT DoAction(bool &repaint);

  void UpdateTitle();
  bool UpdateIcon(int iconsize);

  void IncClickCount(bool &repaint);
  void CheckClickCount(bool &repaint);
  void LoadClickCount();
  void SaveClickCount();

protected:
  bool ClickCountValid();
  void CheckShort(CShortCut* shortcut, bool &repaint);
};

/************************************************************************/
/* 显示元素基类                                                         */
/************************************************************************/

// 两端对齐，暂未实现
#define DT_ALIGNSIDE      0x8000

struct TElementInfo
{
  int x;
  int y;
  int w;
  int h;
  int align;
  int valign;
  int group;
  bool ellipsis; // 超长时显示省略号
};

class CElement : public CPersistent
{
public:
  enum TElementKind { Label, Image, ShortCutBar, ChildPlugin };

  CElement();
  ~CElement();

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);

  virtual bool HasShortCut() = 0;
  virtual CShortCut* GetFirstShortCut() = 0;
  virtual CShortCut* GetLastShortCut() = 0;
  virtual bool GetNextShortCut(CShortCut* &shortcut) = 0;
  virtual bool GetPrevShortCut(CShortCut* &shortcut) = 0;
  virtual int GetShortCutCount() = 0;
  virtual int GetShortCutIndex(CShortCut* shortcut) = 0;

  virtual bool SetGroup(int group, CShortCut* &shortcut) = 0;

  virtual int GetMaxY() = 0;

  virtual TElementKind GetKind() = 0;

  TElementInfo data;
  wchar_t* name;
  CFont* font;
  CFont* selfont;
};

/************************************************************************/
/* 普通元素基类                                                         */
/************************************************************************/

class CStdElement : public CElement
{
public:
  CStdElement();
  ~CStdElement();

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);

  bool HasShortCut();
  CShortCut* GetFirstShortCut();
  CShortCut* GetLastShortCut();
  bool GetNextShortCut(CShortCut* &shortcut);
  bool GetPrevShortCut(CShortCut* &shortcut);
  int GetShortCutCount();
  int GetShortCutIndex(CShortCut* shortcut);

  bool SetGroup(int group, CShortCut* &shortcut);

  CShortCut* shortcut;
};

/************************************************************************/
/* 文本节点类                                                           */
/************************************************************************/

// 数字运算操作符
#define OP_NONE      0
#define OP_ADD       1
#define OP_SUBX      2
#define OP_XSUB      3
#define OP_MUL       4
#define OP_DIVX      5
#define OP_XDIV      6

// 默认的无效值
#define DEF_VALID    0x5A5AA5A5

struct TTextNodeInfo
{
  int node;  // 节点类型
  int group; // 分组号
  union {
    struct
    {
      int len;        // 文本长度或数字位数
      int decdig;     // 小数位数
      bool leadzero;  // 前面加0
      int op;         // 简单运算
      double opnum;   // 运算数
      int valid;      // 有效值
      int invalid;    // 无效值
      bool isend;     // 是否结束
    };
    struct
    {
      // 节假日属性
      bool fullmode;
      int level;
      bool incdate;
      int sday;
      int eday;
      int showmask;
    };
    struct
    {
      // 日出日落属性
      GEO_INFO geo;
      OTIME suntime;
      SUNTYPE suntype;
    };
  };
};

class CTextNode : public CPersistent
{
public:
  CTextNode();
  ~CTextNode();

  TTextNodeInfo data;
  wchar_t* ls;         // 左填充文本
  wchar_t* rs;         // 右填充文本
  wchar_t* pluginname; // 关联插件名
  wchar_t* relname;    // 关联的名称
  wchar_t* text;       // 文本
  wchar_t* replace;    // 替换文本列表
  CHomeColor* color;   // 文本颜色
  CHomeColor* selcolor;// 选中后的文本颜色

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
};

/************************************************************************/
/* 标签类                                                               */
/************************************************************************/

class CLabel : public CStdElement
{
public:
  CLabel();
  ~CLabel();

  TElementKind GetKind();

  void Clear();
  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
  int GetMaxY();

  CList* nodes;
};

/************************************************************************/
/* 图像类                                                               */
/************************************************************************/

struct TImageInfo 
{
  bool transparent;
  int border;
};

class CImage : public CStdElement
{
public:
  CImage();
  ~CImage();

  TImageInfo info;
  CHomeColor* transcolor;
  wchar_t* filename;
  HBITMAP bmp;
  int width;
  int height;

  TElementKind GetKind();

  void Clear();
  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
  int GetMaxY();

  void LoadImage(wchar_t* AName);
  void FreeImage();
};

/************************************************************************/
/* 快捷方式目录类                                                       */
/************************************************************************/

struct TShortCutDirInfo 
{
  int group;
  bool subdir;
  int index;
};

class CShortCutDir : public CPersistent
{
public:
  CShortCutDir();
  ~CShortCutDir();

  TShortCutDirInfo data;
  wchar_t* dir;
  wchar_t* mask;
  uint freesize;

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
};

/************************************************************************/
/* 快捷方式类                                                           */
/************************************************************************/

// 图标大小 iconsize
#define ICON_SMALL    0
#define ICON_LARGE    1

// 图标模式 mode
#define SM_ICON       0
#define SM_TITLE      1

struct TShortCutBar
{
  int iconspacing;
  int mode;
  int iconsize;
  bool grouped;
  int group;
  bool autosort;
  bool lastontop;
};

class CShortCutBar : public CElement
{
  friend CShortCut;
public:
  CShortCutBar();
  ~CShortCutBar();

  TElementKind GetKind();

  void Clear();
  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);

  bool HasShortCut();
  CShortCut* GetFirstShortCut();
  CShortCut* GetLastShortCut();
  bool GetNextShortCut(CShortCut* &shortcut);
  bool GetPrevShortCut(CShortCut* &shortcut);
  int GetShortCutCount();
  int GetShortCutIndex(CShortCut* shortcut);

  bool SetGroup(int group, CShortCut* &shortcut);
  int GetMaxY();

  void SortShortCuts();

  TShortCutBar bar;
  CList* nodes;
  CList* dirs;
  int topindex;

protected:
  wchar_t* m_lasttarget;
  wchar_t* m_lasttitle;
  int m_lastgroup;
  void DoGetShortCutsFromDir(CShortCutDir* dir, wchar_t* dirname, wchar_t* mask, int &idx, bool incsub);
  void UpdateShortCutsFromDir(int group);

};

/************************************************************************/
/* 子插件类                                                             */
/************************************************************************/

class CChildPlugin : public CStdElement
{
public:
  CChildPlugin();
  ~CChildPlugin();

  TElementKind GetKind();

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);

  void ReleasePlugin();

  int GetMaxY();

  wchar_t* clsid;
  IHomePlugin* plugin;

protected:

};

/************************************************************************/
/* 插件参数类                                                           */
/************************************************************************/

struct TPluginParamInfo
{
  int height;
  int paddingleft;
  int paddingright;
  int paddingtop;
  int paddingbottom;
  int borderwidth;
  bool noborder;
};

class CPluginParam : public CPersistent
{
public:
  CPluginParam();
  ~CPluginParam();

  TPluginParamInfo data;
  wchar_t* name;
  CFont* font;
  CFont* selfont;
  CHomeColor* bordercolor;

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
};
