/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：插件主单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OrchidHome.h,v 1.57 2006/10/24 03:58:00 Administrator Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <windows.h>
#include <memory.h>
#include <ocidl.h>
#include <home.h>

#include "OHomeData.h"
#include "OXMLUtils.h"
#include "OSysInfo.h"
#include "resource.h"

#define IMAGENAME TEXT("OrchidHome.dll")

/************************************************************************/
/* 数据结构                                                             */
/************************************************************************/

/************************************************************************/
/* 标准 COM 类工厂                                                      */
/************************************************************************/

class PluginFactory : public IClassFactory
{
  int m_cRef;

public:
  PluginFactory();
  virtual ~PluginFactory();

  // IUnknown interface
  STDMETHODIMP QueryInterface(REFIID riid, void** ppObject);
  STDMETHODIMP_(ULONG) AddRef() { m_cRef++; return m_cRef; }
  STDMETHODIMP_(ULONG) Release() {m_cRef--; if(m_cRef > 0) { return(m_cRef); } delete this; return 0; }

  STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void** ppObject);
  STDMETHODIMP LockServer(BOOL fLock);
};

/************************************************************************/
/* Home Screen 插件类                                                   */
/************************************************************************/

class PluginOrchid : 
  public CHomeNotifier,
  public IHomePlugin,
  public IPersistStream,
  public IHomePluginEnvironment2
{
  int m_cRef;
  IHomePluginEnvironment2* m_ppe;
  HPLUGIN m_hPlugin;

  int m_index;
  int m_height;
  uint m_mask;
  bool m_delayupdate;
  uint m_nexttimer;

  CPluginParam* m_param;
  CList* m_list;

  CElement* m_selnode;
  CShortCut* m_selected;

  CChildPlugin* m_smsplugin;

public:
  PluginOrchid();
  virtual ~PluginOrchid();

  // IUnknown interface
  STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);
  STDMETHODIMP_(ULONG) AddRef();
  STDMETHODIMP_(ULONG) Release();

  // IObjectWithSite methods
  virtual STDMETHODIMP SetSite(IUnknown* pSite);
  virtual STDMETHODIMP GetSite(REFIID riid, void** ppvSite);

  // IHomePlugin interface (see phonehome.idl for documentation)
  STDMETHODIMP Initialize(HPLUGIN hPlugin, IXMLDOMNode* pnodeParams, IXMLDOMNode* pnodeDefault);
  STDMETHODIMP GetHeight(int* pdyPlugin);
  STDMETHODIMP GetSelectability(BOOL* pfSelectable);
  STDMETHODIMP OnEvent(PluginEvent* ppe);

  // IPersist interface
  virtual STDMETHODIMP GetClassID(CLSID* pClassID);

  // IPersistStream interface
  virtual STDMETHODIMP IsDirty(void);
  virtual STDMETHODIMP Load(IStream* pStm);
  virtual STDMETHODIMP Save(IStream *pStm, BOOL fClearDirty);
  virtual STDMETHODIMP GetSizeMax(ULARGE_INTEGER* pcbSize);

  // IHomePluginEnvironment2
  HRESULT STDMETHODCALLTYPE InvalidatePlugin(HPLUGIN hPlugin, DWORD dwFlags);
  HRESULT STDMETHODCALLTYPE SetSingleShotTimer(HPLUGIN hPlugin, UINT cMiliseconds);
  HRESULT STDMETHODCALLTYPE GetColor(HPLUGIN hPlugin, BSTR bstrColorName, COLORREF *pcr);

  // 数据已更新，需要重绘
  void DataChanged(void* sender, uint mask);

  // 更新快捷方式组
  void SetShortCutGroup(wchar_t* name, int group);

  // 返回插件名称
  virtual wchar_t* GetPluginName();

  // 返回选择状态
  virtual CShortCut* GetSelected();

private:
  COLORREF GetColorValue(wchar_t* name, COLORREF crDef);
  void UpdateFontColor(CFont* font);
  void UpdateColors();

  HRESULT SetNextTimer(uint delay);
  void Invalidate(bool checkheight);

  bool CreateChildPlugin(CChildPlugin* plugin);
  bool LoadChildPlugin(CChildPlugin* plugin, IStream* pStm);
  bool SaveChildPlugin(CChildPlugin* plugin, IStream* pStm, BOOL fClearDirty);
  void DoChildPluginEvent(PluginEvent* ppe);
  void CreateInternalSmsPlugin();

  HRESULT XMLInitPlugin(HPLUGIN hPlugin, IXMLDOMNode* pnodeParams, IXMLDOMNode* pnodeDefault);
  HRESULT XMLCheckLangAttr(IXMLDOMNode* pNode);
  HRESULT XMLGetFontFromNodeMap(IXMLDOMNamedNodeMap* pNodeMap, CFont* font);
  HRESULT XMLGetSelectedFontFromNode(IXMLDOMNode* pNode, CFont* font);
  HRESULT XMLGetDefaultParamFromNode(IXMLDOMNode* pNode);
  HRESULT XMLGetParamFromNode(IXMLDOMNode* pNode);
  HRESULT XMLGetElementFromNode(IXMLDOMNode* pNode, CElement* element);
  HRESULT XMLGetStdElementFromNode(IXMLDOMNode* pNode, CStdElement* element);
  HRESULT XMLGetLabelFromNode(IXMLDOMNode* pNode, CLabel* label);
  HRESULT XMLGetTextFromNode(IXMLDOMNode* pNode, CTextNode* text);
  HRESULT XMLGetImageFromNode(IXMLDOMNode* pNode, CImage* image);
  HRESULT XMLGetShortCutBarFromNode(IXMLDOMNode* pNode, CShortCutBar* bar);
  HRESULT XMLGetShortCutFromNode(IXMLDOMNode* pNode, CShortCut* shortcut);
  HRESULT XMLGetShortCutDirFromNode(IXMLDOMNode* pNode, CShortCutDir* dir);
  HRESULT XMLGetChildPluginFromNode(HPLUGIN hPlugin, IXMLDOMNode* pnodeDefault, IXMLDOMNode* pNode, CChildPlugin* plugin);
  HRESULT XMLGetDownCounterFromNode(IXMLDOMNode* pNode, CDownCounter* counter);

  void InitData();
  void InitSelected();
  void ClearList();
  bool CalcHeight();
  void GetTextNodeValue(CTextNode* pText, wchar_t* text, int slen);

  void FillFontData(LOGFONT* lFont, CFont* font);
  HRESULT DoKeyDown(PluginEvent* ppe);
  HRESULT DoAction(PluginEvent* ppe);
  HRESULT DoPaint(PluginEvent* ppe);

  bool DoPaintLabel(CLabel* label, HDC hdc, RECT* rect, bool selected, HFONT hFont, HFONT hSelFont);
  void DoPaintImage(CImage* img, HDC hdc, RECT* rect, bool selected, HFONT hFont, HFONT hSelFont);
  void DoPaintShortCut(CShortCutBar* bar, HDC hdc, RECT* rect, bool selected, HFONT hFont, HFONT hSelFont);
  void DoPaintChildPlugin(CChildPlugin* plugin, HDC hdc, RECT* rect, bool selected, HFONT hFont, HFONT hSelFont);

  void NavAction(PluginEvent* ppe, bool isDown);
  void SelectedChanged();

};
