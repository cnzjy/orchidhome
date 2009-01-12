/*******************************************************************************
* ������ƣ�Orchid Mobile Software Home Plugin
* �ļ����ƣ�������������൥Ԫ
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OHomeClasses.h,v 1.30 2006/06/08 05:38:38 zjy Exp $
* ��    ע��
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
/* �ṹ����                                                             */
/************************************************************************/

// ��γ����Ϣ�ṹ
struct GEO_INFO
{
  double longitude;  // ����
  double latitude;   // γ��
  int zonetime;      // ʱ��
};

/************************************************************************/
/* ���־û���                                                           */
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
/* ����������                                                             */
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
/* ��ɫ��                                                               */
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
/* ������                                                               */
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
/* ��ݷ�ʽ��                                                           */
/************************************************************************/

// ��ݷ�ʽ����
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
/* ��ʾԪ�ػ���                                                         */
/************************************************************************/

// ���˶��룬��δʵ��
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
  bool ellipsis; // ����ʱ��ʾʡ�Ժ�
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
/* ��ͨԪ�ػ���                                                         */
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
/* �ı��ڵ���                                                           */
/************************************************************************/

// �������������
#define OP_NONE      0
#define OP_ADD       1
#define OP_SUBX      2
#define OP_XSUB      3
#define OP_MUL       4
#define OP_DIVX      5
#define OP_XDIV      6

// Ĭ�ϵ���Чֵ
#define DEF_VALID    0x5A5AA5A5

struct TTextNodeInfo
{
  int node;  // �ڵ�����
  int group; // �����
  union {
    struct
    {
      int len;        // �ı����Ȼ�����λ��
      int decdig;     // С��λ��
      bool leadzero;  // ǰ���0
      int op;         // ������
      double opnum;   // ������
      int valid;      // ��Чֵ
      int invalid;    // ��Чֵ
      bool isend;     // �Ƿ����
    };
    struct
    {
      // �ڼ�������
      bool fullmode;
      int level;
      bool incdate;
      int sday;
      int eday;
      int showmask;
    };
    struct
    {
      // �ճ���������
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
  wchar_t* ls;         // ������ı�
  wchar_t* rs;         // ������ı�
  wchar_t* pluginname; // ���������
  wchar_t* relname;    // ����������
  wchar_t* text;       // �ı�
  wchar_t* replace;    // �滻�ı��б�
  CHomeColor* color;   // �ı���ɫ
  CHomeColor* selcolor;// ѡ�к���ı���ɫ

  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
};

/************************************************************************/
/* ��ǩ��                                                               */
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
/* ͼ����                                                               */
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
/* ��ݷ�ʽĿ¼��                                                       */
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
/* ��ݷ�ʽ��                                                           */
/************************************************************************/

// ͼ���С iconsize
#define ICON_SMALL    0
#define ICON_LARGE    1

// ͼ��ģʽ mode
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
/* �Ӳ����                                                             */
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
/* ���������                                                           */
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
