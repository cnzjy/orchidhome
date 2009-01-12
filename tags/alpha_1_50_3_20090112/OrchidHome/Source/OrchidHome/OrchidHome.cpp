/*******************************************************************************
* ������ƣ�Orchid Mobile Software Home Plugin
* �ļ����ƣ��������Ԫ
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OrchidHome.cpp,v 1.103 2006/10/24 03:58:00 Administrator Exp $
* ��    ע��
*******************************************************************************/

#include "OrchidHome.h"

/************************************************************************/
/* GUID ����                                                            */
/************************************************************************/

#define INITGUID
#include "initguid.h"

DEFINE_GUID(CLSID_ORCHIDPLUG, 0xafc6bae1, 0x194b, 0x487c, 0x85, 0x6c, 0x70, 0xc5, 0x40, 0xef, 0xd1, 0x89);

#define CLSIDTEXT__ORCHIDPLUG TEXT("AFC6BAE1-194B-487c-856C-70C540EFD189")

// define all IID GUIDs
DEFINE_GUID(IID_IHomePlugin, 0x7F0C58E9, 0x4F30, 0x47bb, 0x96, 0x93, 0xd7, 0x78, 0xe9, 0x99, 0xc4, 0x33);
DEFINE_GUID(IID_IHomePluginEnvironment,  0xFF328DC0, 0x22F5, 0x4AC1, 0xA1, 0x03, 0x19, 0xB3, 0x89, 0xC6, 0x33, 0xFA);
DEFINE_GUID(IID_IHomePluginEnvironment2, 0xE01A8A84, 0xDDEF, 0x438e, 0x90, 0x04, 0x3F, 0xB2, 0xA9, 0x08, 0xE4, 0x65);
DEFINE_GUID(IID_IPersistStream, 0x00000109, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
DEFINE_GUID(IID_IPersist, 0x0000010c, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
DEFINE_GUID(IID_IObjectWithSite, 0xFC4801A3, 0x2BA9, 0x11CF, 0xA2, 0x29, 0x00, 0xAA, 0x00, 0x3D, 0x73, 0x52);

#include <COGUID.H>
#undef INITGUID

/************************************************************************/
/* ������������                                                         */
/************************************************************************/

// ��� DLL ���ü���
ULONG g_dwDLLRefCount = 0;

#define DELAY_SHORT       50

// �ַ�������
static wchar_t* sSMSClsID = L"{8486CC1E-025B-4888-9C76-048DF7C1787D}";
static wchar_t* sSysSMSClsID = L"{2F930BF0-6FE9-4a53-9E17-88E9247BAB48}";
static wchar_t* sSMSXml =
  L"<?xml version=\"1.0\"?>"
  L"<home>"
  L"<default font-face=\"%s\" font-size=\"14\" padding-left=\"4\" padding-right=\"4\" padding-top=\"2\" bgcolor=\"transparent\" fgcolor=\"COLOR_HOMETEXT\">"
  L"<format state=\"selected\" fgcolor=\"COLOR_HOMEHIGHLIGHTTEXT\"/>"
  L"</default>"
  L"<plugin clsid=\"{2F930BF0-6FE9-4a53-9E17-88E9247BAB48}\" height=\"20\">"
  L"<label><text><unreadEmail/><unreadSMS/><unreadVMail/></text></label>"
  L"</plugin>"
  L"</home>";

/************************************************************************/
/* ��׼ COM ��������ʵ��                                                */
/************************************************************************/

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppObject)
{
  HRESULT hr = S_OK;
  PluginFactory *pFactory = NULL;

  if(rclsid == CLSID_ORCHIDPLUG)
  {
    pFactory = new PluginFactory;
    if(!pFactory)
    {
      hr = E_OUTOFMEMORY;
      goto Error;
    }

    if(FAILED(pFactory->QueryInterface(riid, ppObject)))
    {
      hr = E_NOINTERFACE;
      goto Error;
    }
  }
  else
  {
    hr = CLASS_E_CLASSNOTAVAILABLE;
    goto Error;
  }

Error:
  if(pFactory)
  {
    pFactory->Release();
  }
  return(hr);
}

STDAPI DllCanUnloadNow()
{
  if(g_dwDLLRefCount)
  {
    return S_FALSE;
  }
  else
  {
    return S_OK;
  }
}

STDAPI DllRegisterServer(void)
{
  HRESULT hr = S_OK;
  HKEY hKeyCLSID = NULL;
  HKEY hKeyInproc32 = NULL;
  DWORD dwDisposition;
  HMODULE hModule;
  TCHAR szName[MAX_PATH+1];

  if(ERROR_SUCCESS != RegCreateKeyEx(HKEY_CLASSES_ROOT,
    TEXT("CLSID\\{") CLSIDTEXT__ORCHIDPLUG TEXT("}"),
    NULL, TEXT(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
    &hKeyCLSID, &dwDisposition))
  {
    hr = E_FAIL;
    goto Error;
  }

  if(ERROR_SUCCESS != RegSetValueEx(hKeyCLSID, TEXT(""), NULL, REG_SZ, (BYTE*) TEXT("OrchidHome Plugin"), sizeof(TEXT("OrchidHome Plugin"))))
  {
    hr = E_FAIL;
    goto Error;
  }

  if(ERROR_SUCCESS != RegCreateKeyEx(hKeyCLSID, TEXT("InprocServer32"),
    NULL, TEXT(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
    &hKeyInproc32, &dwDisposition))
  {
    hr = E_FAIL;
    goto Error;
  }
  hModule = GetModuleHandle(IMAGENAME);
  if(!hModule)
  {
    hr = E_FAIL;
    goto Error;
  }

  if(GetModuleFileName(hModule, szName, sizeof(szName))==0)
  {
    hr = E_FAIL;
    goto Error;
  }
  if(ERROR_SUCCESS != RegSetValueEx(hKeyInproc32, TEXT(""), NULL, REG_SZ, (BYTE*) szName, sizeof(TCHAR)*(lstrlen(szName)+1)))
  {
    hr = E_FAIL;
    goto Error;
  }

Error:
  if(hKeyInproc32)
  {
    RegCloseKey(hKeyInproc32);
  }
  if(hKeyCLSID)
  {
    RegCloseKey(hKeyCLSID);
  }
  return(hr);
}

STDAPI DllUnregisterServer(void)
{
  HRESULT hr = S_OK;

  if(ERROR_SUCCESS != RegDeleteKey(HKEY_CLASSES_ROOT,
    TEXT("CLSID\\{") CLSIDTEXT__ORCHIDPLUG TEXT("}\\InprocServer32")))
  {
    hr = E_FAIL;
    goto Error;
  }
  if(ERROR_SUCCESS != RegDeleteKey(HKEY_CLASSES_ROOT,
    TEXT("CLSID\\{") CLSIDTEXT__ORCHIDPLUG TEXT("}")))
  {
    hr = E_FAIL;
    goto Error;
  }

Error:
  return(hr);
}

/************************************************************************/
/* PluginFactory ʵ��                                                   */
/************************************************************************/

PluginFactory::PluginFactory()
{
  m_cRef = 1;
}

PluginFactory::~PluginFactory()
{
}

HRESULT PluginFactory::QueryInterface(REFIID riid, void** ppObject)
{
  HRESULT hr = S_OK;

  if(riid == IID_IUnknown || riid == IID_IClassFactory)
  {
    *ppObject= (IClassFactory*)this;
  }
  else
  {
    hr = E_NOINTERFACE;
    goto Error;
  }

  AddRef();

Error:
  return(hr);
}

HRESULT PluginFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void** ppObject)
{
  HRESULT hr = S_OK;
  IUnknown* punk = NULL;

  *ppObject = NULL;

  if(pUnkOuter != NULL)
  {
    hr = CLASS_E_NOAGGREGATION;
    goto Error;
  }

  punk = (IUnknown*)(IHomePlugin*) new PluginOrchid;
  if(!punk)
  {
    hr = E_OUTOFMEMORY;
    goto Error;
  }

  if(FAILED(punk->QueryInterface(riid, ppObject)))
  {
    hr = E_NOINTERFACE;
    goto Error;
  }

Error:
  if(punk)
  {
    punk->Release();
  }
  return hr;
}

HRESULT PluginFactory::LockServer(BOOL fLock)
{
  if(fLock)
  {
    g_dwDLLRefCount++;
  }
  else
  {
    g_dwDLLRefCount--;
  }
  return(S_OK);
}

/************************************************************************/
/* PluginOrchid ��ʵ��                                                  */
/************************************************************************/

PluginOrchid::PluginOrchid()
{
  CHomeData::InitInstance();
  m_index = HomeData->AddNotifier(this);

  DEBUGLOG1(TEXT("PluginOrchid create: %d"), m_index);

  g_dwDLLRefCount++;
  m_cRef = 1;
  m_ppe = NULL;
  m_selnode = NULL;
  m_selected = NULL;
  m_mask = DATA_NONE;
  m_smsplugin = NULL;
  m_height = 0;
  m_delayupdate = false;
  m_nexttimer = 0;
  m_param = new CPluginParam;
  m_list = new CList;
}

PluginOrchid::~PluginOrchid()
{
  DEBUGLOG(TEXT("PluginOrchid destroy"));

  if (m_smsplugin)
    delete m_smsplugin;

  delete m_param;
  ClearList();
  delete m_list;
  g_dwDLLRefCount--;

  HomeData->RemoveNotifier(this);
  CHomeData::ReleaseInstance();
}

HRESULT PluginOrchid::QueryInterface(REFIID riid, void** ppObject)
{
  HRESULT hr = S_OK;

  if (IID_IUnknown == riid || IID_IObjectWithSite == riid || IID_IHomePlugin == riid)
  {
    *ppObject = (IHomePlugin*)this;
  }
  else if (IID_IPersist == riid || IID_IPersistStream == riid)
  {
    *ppObject = (IPersistStream*)this;
  }
  else if (IID_IHomePluginEnvironment == riid || IID_IHomePluginEnvironment2 == riid)
  {
    *ppObject = (IHomePluginEnvironment2*)this;
  }
  else
  {
    // �Ӳ�����ܻ���� MS �ڲ��Ľӿڣ�����ת��ϵͳ�ӿ���ȥ����
    hr = m_ppe->QueryInterface(riid, ppObject);
    //hr = E_NOINTERFACE;
    goto Error;
  }
  AddRef();

Error:
  return(hr);
}

// �������ü���
ULONG PluginOrchid::AddRef()
{
  m_cRef++; 
  DEBUGLOG1(TEXT("PluginOrchid AddRef (%d)"), m_cRef);
  return m_cRef;
}

// �������ü���
ULONG PluginOrchid::Release()
{
  m_cRef--; 
  DEBUGLOG1(TEXT("PluginOrchid Release (%d)"), m_cRef);
  if(m_cRef > 0) 
  {
    return(m_cRef);
  }

  delete this;
  return 0;
}

HRESULT PluginOrchid::SetSite(IUnknown* pSite)
{
  HRESULT hr = S_OK;

  DEBUGLOG1(TEXT("PluginOrchid::SetSite called (%x)"), (void*)pSite);

  if(m_ppe)
  {
    m_ppe->Release();
    m_ppe = NULL;
  }

  if(pSite)
  {
    if(FAILED(hr = pSite->QueryInterface(IID_IHomePluginEnvironment2, (void**)&m_ppe)))
    {
      goto Error;
    }
  }
  else
  {
    // ��� pSite Ϊ NULL ˵����Ҫ�ͷŲ����
    for (int i = 0; i < m_list->GetCount(); i++)
    {
      CElement* node = (CElement*)m_list->Get(i);
      if (node->GetKind() == CElement::ChildPlugin)
      {
        CChildPlugin* plugin = (CChildPlugin*)node;
        plugin->ReleasePlugin();
      }
    }
  }

Error:
  return(hr);
}

HRESULT PluginOrchid::GetSite(REFIID riid, void** ppvSite)
{
  HRESULT hr = S_OK;

  if(!m_ppe)
  {
    hr = E_FAIL;
    goto Error;
  }

  if(FAILED(hr = m_ppe->QueryInterface(riid, ppvSite)))
  {
    goto Error;
  }

Error:
  return(hr);
}

// �����ʼ������ Home.exe ���� XML �����ļ������
HRESULT PluginOrchid::Initialize(HPLUGIN hPlugin, IXMLDOMNode* pnodeParams, IXMLDOMNode* pnodeDefault)
{
  m_hPlugin = hPlugin;

  if (m_index == 0)
    HomeData->Initialize();

  return XMLInitPlugin(hPlugin, pnodeParams, pnodeDefault);
}

// ���ز����ʾ�߶�
HRESULT PluginOrchid::GetHeight(int* pdyPlugin)
{
  *pdyPlugin = m_height;

  //DEBUGLOG2(TEXT("PluginOrchid::GetHeight called (%s) (%d)"), m_param->name, *pdyPlugin);
  return S_OK;
}

// ���ز���Ƿ��ѡ��
HRESULT PluginOrchid::GetSelectability(BOOL* pfSelectable)
{
  HRESULT hr = S_OK;

  // ���ӽڵ�֧�ֿ�ݷ�ʽʱ�ſ�ѡ��
  *pfSelectable = FALSE;
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    if (((CElement*)m_list->Get(i))->HasShortCut())
    {
      *pfSelectable = TRUE;
      break;
    }
  }

  //DEBUGLOG2(TEXT("PluginOrchid::GetSelectability called (%s) (%d)"), m_param->name, *pfSelectable);

  return(hr);
}

// ���֪ͨ�¼�
HRESULT PluginOrchid::OnEvent(PluginEvent* ppe)
{
  HRESULT hr = S_OK;
  bool heightchanged = false;

  switch(ppe->idEvent)
  {
  case PE_KEYDOWN:
    // �����¼���ֻ֧�����Ҽ�
    DEBUGLOG2(TEXT("OnEvent PE_KEYDOWN (%s) (%d)"), m_param->name, ppe->p.key.wVKey);
    hr = DoKeyDown(ppe);
    break;

  case PE_ACTION:
    // �û�����ȷ�����¼�
    DEBUGLOG1(TEXT("OnEvent PE_ACTION (%s)"), m_param->name);
    hr = DoAction(ppe);
    break;

  case PE_PAINT:
    // ��������ػ��¼�
    DEBUGLOG1(TEXT("OnEvent PE_PAINT (%s)"), m_param->name);
    STARTTIME();
    hr = DoPaint(ppe);
    LOGTIME(L"DoPaint");
    break;

  case PE_SYSCOLORCHANGE:
    // ϵͳ��ɫ�ı��¼�
    DEBUGLOG(TEXT("OnEvent PE_SYSCOLORCHANGE"));
    UpdateColors();
    DoChildPluginEvent(ppe);
    break;

  case PE_TIMER:
    // ��ʱ���¼�
    // DEBUGLOG1(TEXT("OnEvent PE_TIMER (%s)", m_param->name);

    m_nexttimer = 0;
    if (m_delayupdate)
    {
      // ��ʱˢ��
      m_delayupdate = false;
      Invalidate(false);
      return hr;
    }
    else
    {
      if (m_index == 0)
        HomeData->CheckData(false);
      DoChildPluginEvent(ppe);
    }
    break;

  case PE_DATACHANGE:
    // ϵͳ���ݱ���¼�
    DEBUGLOG(TEXT("OnEvent PE_DATACHANGE"));
    DoChildPluginEvent(ppe);
    break;

  default:
    break;
  }

  // ֻ�е�һ�����ʹ�ö�ʱ��
  if (m_index == 0)
    SetNextTimer(HomeData->interval);

  return(hr);
}

// ������������������ṹ
void PluginOrchid::FillFontData(LOGFONT* lFont, CFont* font)
{
  if (font->fontface[0] != 0)
    wcsncpy(lFont->lfFaceName, font->fontface, LF_FACESIZE - 1);
  if (font->data.height)
    lFont->lfHeight = font->data.height;
  if (font->data.width)
    lFont->lfWidth = font->data.width;
  lFont->lfWeight = font->data.bold ? FW_BOLD : FW_DONTCARE;
  lFont->lfItalic = font->data.italic ? TRUE : FALSE;
  lFont->lfUnderline = font->data.underline ? TRUE : FALSE;
  lFont->lfQuality = font->data.cleartype ? CLEARTYPE_QUALITY : DEFAULT_QUALITY;
}

// ��������ػ�
HRESULT PluginOrchid::DoPaint(PluginEvent* ppe)
{
  HDC hdc = ppe->p.paint.hdc;
  bool selected = ppe->p.paint.fSelected ? true : false;
  RECT rect;
  HFONT saveFont, hFont, hSelFont;
  LOGFONT logFont, lFont, lSelFont;
  int groups[256];
  int groupcount = 0;

  rect = ppe->p.paint.rcDraw;

  // ������
  FillRectColor(hdc, &rect, selected ? m_param->selfont->bgcolor->color : m_param->font->bgcolor->color);

  // ���߿�
  if (m_param->bordercolor->color != COLOR_TRANSPARENT && m_param->data.borderwidth > 0)
  {
    rect = ppe->p.paint.rcDraw;
    rect.top = rect.bottom - m_param->data.borderwidth;
    FillRectColor(hdc, &rect, m_param->bordercolor->color);
  }

  // ����Ĭ�ϵ���ʾ����
  rect = ppe->p.paint.rcDraw;
  rect.left += m_param->data.paddingleft;
  rect.right -= m_param->data.paddingright;
  rect.top += m_param->data.paddingtop;
  rect.bottom -= m_param->data.paddingbottom + m_param->data.borderwidth;

  // ���浱ǰ����
  saveFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
  GetObject(saveFont, sizeof(LOGFONT), &logFont);
  SetBkMode(hdc, TRANSPARENT);

  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CElement* pnode = (CElement*)m_list->Get(i);
    RECT rcnode;

    // ��������
    lFont = logFont;
    FillFontData(&lFont, pnode->font);
    hFont = CreateFontIndirect(&lFont);
    lSelFont = logFont;
    FillFontData(&lSelFont, pnode->selfont);
    hSelFont = CreateFontIndirect(&lSelFont);

    // ������ʾ����
    rcnode = rect;
    rcnode.left += pnode->data.x;
    rcnode.top += pnode->data.y;
    if (pnode->data.w)
      rcnode.right = rcnode.left + pnode->data.w;
    if (pnode->data.h)
      rcnode.bottom = rcnode.top + pnode->data.h;

    switch(pnode->GetKind())
    {
      case CElement::Label:
        CLabel* label;
        label = (CLabel*)pnode;
        // ��������
        if (label->data.group != 0)
        {
          bool exists = false;
          // ����÷�����Ѿ����ڣ�����������
          for (int i = 0; i < groupcount; i++)
            if (groups[i] == label->data.group)
            {
              exists = true;
              break;
            }
          if (exists) break;
        }
        // ����ı�����
        if (DoPaintLabel(label, hdc, &rcnode, selected, hFont, hSelFont))
        {
          // ��¼�¸÷����
          if (label->data.group && groupcount < LENOF(groups) - 1)
            groups[groupcount++] = label->data.group;
        }
        break;
      case CElement::Image:
        // ���ͼ������
        DoPaintImage((CImage*)pnode, hdc, &rcnode, selected, hFont, hSelFont);
        break;
      case CElement::ShortCutBar:
        // ���ƿ�ݷ�ʽ
        DoPaintShortCut((CShortCutBar*)pnode, hdc, &rcnode, selected, hFont, hSelFont);
        break;
      case CElement::ChildPlugin:
        // ���ƿ�ݷ�ʽ
        DoPaintChildPlugin((CChildPlugin*)pnode, hdc, &rcnode, selected, hFont, hSelFont);
        break;
      default:
        break;
    }

    // �ͷ�����
    SelectObject(hdc, saveFont);
    DeleteObject(hFont);
    DeleteObject(hSelFont);
    hFont = NULL;
    hSelFont = NULL;
  }

  return S_OK;
}

// �����ı���ǩ
bool PluginOrchid::DoPaintLabel(CLabel* label, HDC hdc, RECT* rect, bool selected, HFONT hFont, HFONT hSelFont)
{
  wchar_t buff[1024], sz[TEXT_LEN];
  int xsizes[1024];
  int groups[256];
  int groupcount = 0;
  wchar_t* sdot = L"...";
  int dotlen = wcslen(sdot);        
  int dotwidth;
  SIZE size;
  struct {
    int offset;
    int len;
    int line;
    int width;
    COLORREF color;
  } items[128];
  int count = 0, line = 0;
  bool sel = selected && (m_selected == label->shortcut);
  CFont* font = sel ? label->selfont : label->font;

  memset(xsizes, 0, sizeof(xsizes));
  memset(items, 0, sizeof(items));

  // ����ʡ�ԺŵĿ��
  GetTextExtentPoint(hdc, &sdot[0], 1, &size);
  dotwidth = size.cx + font->data.xspace;

  // ȡ��Ҫ���Ƶ��ı�
  buff[0] = 0;
  for (int i = 0; i < label->nodes->GetCount(); i++)
  {
    CTextNode* text = (CTextNode*)label->nodes->Get(i);
    COLORREF textcolor = sel ? text->selcolor->color : text->color->color;
    GetTextNodeValue(text, sz, TEXT_LEN);
    if (sz[0])
    {
      // ��������
      if (text->data.group != 0)
      {
        bool exists = false;
        // ����÷�����Ѿ����ڣ�����������
        for (int i = 0; i < groupcount; i++)
          if (groups[i] == text->data.group)
          {
            exists = true;
            break;
          }
        if (exists) continue;

        // ��¼�¸÷����
        if (groupcount < LENOF(groups) - 1)
          groups[groupcount++] = text->data.group;
      }

      // �����ַ���
      if (sz[0] == '\n' && sz[1] == 0)
      {
        line++;
        continue;
      }

      // ����ɫ�ֶ�
      if (count == 0)
      {
        items[0].offset = 0;
        items[0].len = wcslen(sz);
        items[0].color = textcolor;
        items[0].line = line;
        count++;
      } 
      else if ((items[count - 1].color == textcolor && line == items[count - 1].line) || 
        (count == LENOF(items)))
      {
        items[count - 1].len += wcslen(sz);
      }
      else
      {
        items[count].offset = wcslen(buff);
        items[count].len = wcslen(sz);
        items[count].color = textcolor;
        items[count].line = line;
        count++;
      }

      wcscat(buff, sz);

      // ��黻�з��������ַ�
      for (int j = 0; j < items[count - 1].len; j++)
      {
        if (buff[items[count - 1].offset + j] == '\n')
        {
          if (j == 0)
          {
            // �����ڿ�ʼ
            items[count - 1].line++;
            items[count - 1].offset++;
            items[count - 1].len--;
          }
          else if (j == items[count - 1].len - 1)
          {
            // �����ڽ���ʱ
            line++;
            items[count - 1].len--;
          }
          else if (count < LENOF(items))
          {
            // �������м�ʱ
            line++;
            items[count - 1].len = j;
            items[count].offset = items[count - 1].offset + j + 1;
            items[count].len = items[count - 1].len - j - 1;
            items[count].color = textcolor;
            items[count].line = line;
            count++;
          }
        }
        else if (buff[items[count - 1].offset + j] < ' ')
        {
          // �����ַ�ת�ɿո���ʾ
          buff[items[count - 1].offset + j] = ' ';
        }
      }
    }
  }

  if (buff[0] != 0)
  {
    int bufflen = wcslen(buff);
    int width = 0, height = 0, lh = 0;
    int x, y;

    if (selected && m_selected == label->shortcut)
      SelectObject(hdc, hSelFont);
    else
      SelectObject(hdc, hFont);

    FillRectColor(hdc, rect, font->bgcolor->color);

    // ����ÿ���ַ��Ŀ�Ⱥ͸߶�
    for (int i = 0; i < bufflen; i++)
    {
      GetTextExtentPoint(hdc, &buff[i], 1, &size);
      xsizes[i] = size.cx + ((buff[i + 1] == '\n' || buff[i + 1] == 0) ? 0 : font->data.xspace);
      if (size.cy > height)
        lh = size.cy;
    }

    // ����߶Ⱥʹ�ֱλ��
    height = lh * (line + 1) + font->data.yspace * line;
    if (label->data.valign == DT_BOTTOM)
      y = rect->bottom - height;
    else if (label->data.valign == DT_VCENTER)
      y = (rect->bottom - rect->top - height) / 2 + rect->top;
    else
      y = rect->top;

    for (int lineidx = 0; lineidx <= line; lineidx++)
    {
      // �������ʵ�ʿ��
      width = 0;
      for (int i = 0; i < count; i++)
        if (items[i].line == lineidx)
          for (int j = items[i].offset; j < items[i].offset + items[i].len; j++)
            width += xsizes[j];

      if (label->data.ellipsis && width > rect->right - rect->left)
      {
        // ��������ʾʡ�Ժ�
        width = dotwidth * dotlen - font->data.xspace;
        for (int i = 0; i < count; i++)
        {
          if (items[i].line == lineidx)
          {
            for (int j = items[i].offset; j < items[i].offset + items[i].len; j++)
            {
              if (width + xsizes[j] > rect->right - rect->left)
              {
                wcscpy(&buff[j], sdot);
                items[i].len = j - items[i].offset + dotlen;
                for (int k = j; k < j + dotlen; k++)
                  xsizes[k] = dotwidth; // ʡ�ԺŵĿ��

                // ����������С�ζ�����Ҫ��ʾ
                for (i++; i < count && items[i].line == lineidx; i++)
                  items[i].len = 0;
                i--;
                break;
              }
              else
              {
                width += xsizes[j];
              }
            }
          }
        }
      }

      // ����ÿС�εĿ��
      for (int i = 0; i < count; i++)
        if (items[i].line == lineidx)
        {
          items[i].width = 0;
          for (int j = items[i].offset; j < items[i].offset + items[i].len; j++)
            items[i].width += xsizes[j];
        }

      // ����ˮƽλ��
      if (label->data.align == DT_RIGHT)
        x = rect->right - width;
      else if (label->data.align == DT_CENTER)
        x = (rect->right - rect->left - width) / 2 + rect->left;
      else
        x = rect->left;

      // ������Ӱ
      if (font->data.shadow)
      {
        RECT rcshadow = *rect;
        int start = -1, drawlen = 0;
        OffsetRect(&rcshadow, font->data.shadowx, font->data.shadowy);
        SetTextColor(hdc, font->shadowcolor->color);
        for (int i = 0; i < count; i++)
          if (items[i].line == lineidx && items[i].len > 0)
          {
            if (start < 0) start = items[i].offset;
            drawlen += items[i].len;
          }
        if (start >= 0)
          ExtTextOut(hdc, x + font->data.shadowx, y + font->data.shadowy, ETO_CLIPPED, &rcshadow, &buff[start], drawlen, &xsizes[start]);
      }

      // �����ı�
      for (int i = 0; i < count; i++)
      {
        if (items[i].line == lineidx && items[i].len > 0)
        {
          SetTextColor(hdc, items[i].color);
          ExtTextOut(hdc, x, y, ETO_CLIPPED, rect, &buff[items[i].offset], items[i].len, &xsizes[items[i].offset]);
          x += items[i].width;
        }
      }

      y += lh + font->data.yspace;
    }
    return true;
  }
  else
    return false;
}

// ����ͼ��
void PluginOrchid::DoPaintImage(CImage* img, HDC hdc, RECT* rect, bool selected, HFONT hFont, HFONT hSelFont)
{
  if (img->bmp)
  {
    HDC hdcTemp = CreateCompatibleDC(hdc);
    RECT rcimg = *rect;
    int x, y, w, h;
    bool sel = selected && (m_selected == img->shortcut);

    // ������߿�Ĵ�С
    if (rcimg.right - rcimg.left > img->width + img->info.border * 2)
      rcimg.right = rcimg.left + img->width + img->info.border * 2;
    if (rcimg.bottom - rcimg.top > img->height + img->info.border * 2)
      rcimg.bottom = rcimg.top + img->height + img->info.border * 2;

    // ���߿�
    FillRectColor(hdc, &rcimg, sel ? img->selfont->bgcolor->color : img->font->bgcolor->color);

    SelectObject(hdcTemp, img->bmp);

    x = rcimg.left + img->info.border;
    y = rcimg.top + img->info.border;
    w = rcimg.right - x - img->info.border;
    h = rcimg.bottom - y - img->info.border;
    if (img->info.transparent)
    {
      COLORREF transcolor;
      if (img->transcolor->color == COLOR_TRANSPARENT)
        transcolor = GetPixel(hdcTemp, 0, img->height - 1); // ȡ���½�Ϊ͸��ɫ
      else
        transcolor = img->transcolor->color;
      // ͸������
      TransparentBlt(hdc, x, y, w, h, hdcTemp, 0, 0, w, h, transcolor);
    }
    else
    {
      // ��ͨ����
      BitBlt(hdc, x, y, w, h, hdcTemp, 0, 0, SRCCOPY);
    }
    DeleteDC(hdcTemp);
  }
}

// ���ƿ�ݷ�ʽ
void PluginOrchid::DoPaintShortCut(CShortCutBar* bar, HDC hdc, RECT* rect, bool selected, HFONT hFont, HFONT hSelFont)
{
  int x, y, border;
  int isize;
  int idx;
  
  isize = bar->bar.iconsize == ICON_LARGE ? HomeData->iconsize : HomeData->smiconsize;
  border = (bar->bar.iconspacing + 1) / 2;
  x = rect->left + border;
  y = rect->top;

  if (bar->bar.mode == SM_ICON)
  {
    // ͼ��̫��ʱ�Զ�����
    int maxcnt = (rect->right - rect->left) / (isize + bar->bar.iconspacing);
    int idx = -1;
    int cnt = 0;
    for (int j = 0; j < bar->nodes->GetCount(); j++)
    {
      CShortCut* shortcut = (CShortCut*)bar->nodes->Get(j);
      if (!bar->bar.grouped || bar->bar.group == GROUP_ALL ||  bar->bar.group == shortcut->data.group)
      {
        if (shortcut == m_selected) idx = cnt;
        cnt++;
      }
    }

    // �����������ʾ��ͼ���
    if (idx >= 0 && cnt > maxcnt)
    {
      if (idx - bar->topindex >= maxcnt)
        bar->topindex = idx - maxcnt + 1;
      if (idx < bar->topindex)
        bar->topindex = idx;
    }
  }

  for (int j = idx = 0; j < bar->nodes->GetCount(); j++)
  {
    CShortCut* shortcut;
    RECT rc;
    bool sel;

    // ֻ��ʾ����ƥ�������
    shortcut = (CShortCut*)bar->nodes->Get(j);
    if (!bar->bar.grouped || bar->bar.group == GROUP_ALL || bar->bar.group == shortcut->data.group)
    {
      CFont* font;
      sel = selected && shortcut == m_selected;
      font = sel ? bar->selfont : bar->font;

      if (bar->bar.mode == SM_TITLE)
      {
        SIZE size;

        if (sel)
          SelectObject(hdc, hSelFont);
        else
          SelectObject(hdc, hFont);

        // �����ı���ݷ�ʽ
        GetTextExtentPoint(hdc, shortcut->title, wcslen(shortcut->title), &size);
        SetRect(&rc, x, y, x + size.cx, y + size.cy);
        if (IntersectRect(&rc, &rc, rect))
        {
          FillRectColor(hdc, &rc, font->bgcolor->color);

          // ������Ӱ
          if (font->data.shadow)
          {
            RECT rcshadow = rc;
            OffsetRect(&rcshadow, font->data.shadowx, font->data.shadowy);
            SetTextColor(hdc, font->shadowcolor->color);
            DrawText(hdc, shortcut->title, -1, &rcshadow, 0);
          }

          // �����ı�
          SetTextColor(hdc, font->color->color);
          DrawText(hdc, shortcut->title, -1, &rc, 0);
        }

        x += size.cx + bar->bar.iconspacing;
      }
      else // if (bar->bar.mode == SM_ICON)
      {
        if (idx >= bar->topindex)
        {
          if (x + isize < rect->right)
          {
            SetRect(&rc, x, y, x + isize, y + isize);
            if (IntersectRect(&rc, &rc, rect))
            {
              RECT rcbg = rc;

              // �屳��
              InflateRect(&rcbg, border, border);
              FillRectColor(hdc, &rcbg, sel ? bar->selfont->bgcolor->color : bar->font->bgcolor->color);

              // ����ͼ��
              if (shortcut->UpdateIcon(bar->bar.iconsize) && shortcut->icon)
                DrawIcon(hdc, rc.left, rc.top, shortcut->icon);
            }

            x += isize + bar->bar.iconspacing;
          }
        }
      }
      idx++;
    }

    if (x + border >= rect->right)
      break;
  }
}

// �����Ӳ��
void PluginOrchid::DoPaintChildPlugin(CChildPlugin* plugin, HDC hdc, RECT* rect, bool selected, HFONT hFont, HFONT hSelFont)
{
  if (plugin->plugin)
  {
    PluginEvent ppe;
    memset(&ppe, 0, sizeof(ppe));
    ppe.idEvent = PE_PAINT;
    ppe.p.paint.fSelected = selected && (plugin->shortcut == m_selected);
    ppe.p.paint.hdc = hdc;
    ppe.p.paint.rcDraw = *rect;

#if 0
    int height = rect->bottom - rect->top;
    if (SUCCEEDED(plugin->plugin->GetHeight(&height)))
      if (rect->top + height < rect->bottom)
        ppe.p.paint.rcDraw.bottom = rect->top + height;
#endif

    plugin->plugin->OnEvent(&ppe);
  }
}

// �������¼�
HRESULT PluginOrchid::DoKeyDown(PluginEvent* ppe)
{
  if (ppe->p.key.wVKey == VK_LEFT)
  {
    NavAction(ppe, false);
  }
  else if (ppe->p.key.wVKey == VK_RIGHT)
  {
    NavAction(ppe, true);
  }

  return S_OK;
}

void PluginOrchid::NavAction(PluginEvent* ppe, bool isDown)
{
  int idx;
  CElement* pnode;
  CShortCut* save = m_selected;
  if (m_list->GetCount() > 0)
  {
    idx = m_list->IndexOf(m_selnode);
    if (idx >= 0)
    {
      // �������
      if (!isDown)
      {
        if (!m_selnode->GetPrevShortCut(m_selected))
        {
          m_selnode = NULL;
          m_selected = NULL;
          for (int i = idx - 1; ; i--)
          {
            if (i < 0) i = m_list->GetCount() - 1;
            pnode = (CElement*)m_list->Get(i);
            if (pnode->HasShortCut())
            {
              m_selnode = pnode;
              m_selected = (CShortCut*)m_selnode->GetLastShortCut();
              break;
            }
            else if (i == idx)
            {
              // ����һȦ��û�ҵ�
              break;
            }
          }
        }
      }
      else
      {
        // �����Ҽ�
        if (!m_selnode->GetNextShortCut(m_selected))
        {
          m_selnode = NULL;
          m_selected = NULL;
          for (int i = idx + 1; ; i++)
          {
            if (i >= m_list->GetCount()) i = 0;
            pnode = (CElement*)m_list->Get(i);
            if (pnode->HasShortCut())
            {
              m_selnode = pnode;
              m_selected = (CShortCut*)m_selnode->GetFirstShortCut();
              break;
            }
            else if (i == idx)
            {
              // ����һȦ��û�ҵ�
              break;
            }
          }
        }
      }
    }
    else
    {
      m_selnode = NULL;
      m_selected = NULL;
      for (int i = 0; i < m_list->GetCount(); i++)
      {
        pnode = (CElement*)m_list->Get(i);
        if (pnode->HasShortCut())
        {
          m_selnode = pnode;
          m_selected = (CShortCut*)m_selnode->GetFirstShortCut();
          break;
        }
      }
    }

    SelectedChanged();
  }

  // ���ֻ��һ�������Ӳ���Ŀ�ݷ�ʽ���򽫰��������Ӳ��
  if (m_selected && m_selnode && ppe && (save == m_selected) && (m_selected->data.cmd == CMD_CALLPLUGIN) &&
    (m_selnode->GetKind() == CElement::ChildPlugin))
  {
    CChildPlugin* plugin = (CChildPlugin*)m_selnode;
    plugin->plugin->OnEvent(ppe);
  }
}

// ����ȷ�ϼ�
HRESULT PluginOrchid::DoAction(PluginEvent* ppe)
{
  HRESULT ret = S_OK;
  if (m_selected)
  {
    bool repaint = false;
    ret = m_selected->DoAction(repaint);
    if (repaint)
      Invalidate(false);
  }
  return ret;
}

// ���ز����
HRESULT PluginOrchid::GetClassID(CLSID* pClassID)
{
  *pClassID = CLSID_ORCHIDPLUG;
  return S_OK;
}

/************************************************************************/
/* ������ݳ־û�����                                                   */
/************************************************************************/

HRESULT PluginOrchid::IsDirty(void)
{
  return S_OK;
}

// ������װ�ز������
// Ϊ������������ܣ�Home.exe ֻ����������ʱ���� XML �ļ�����ɺ�Ҫ���������ݱ��浽���У�
// ��������ʱ�����м������ݡ�
HRESULT PluginOrchid::Load(IStream* pStm)
{
  int cnt = 0;
  CElement *pnode = NULL;
  CElement::TElementKind kind;
  wchar_t* stamp = NULL;
  bool hasSms = false;
  bool hasSysSms = false;

  DEBUGLOG(TEXT("PluginOrchid::Load - Called"));

  if (FAILED(CPersistent::LoadFromStream(pStm)))
    return E_FAIL;

  // ʹ�ñ���ʱ���ǩ����֤ÿ�ΰ�װ�°汾����������������
  if (FAILED(DoLoadString(pStm, stamp)))
    return E_FAIL;
  if (wcscmp(stamp, BUILDSTAMP) != 0)
  {
    DEBUGLOG1(L"Stamp error: %s", stamp);
    FreeString(stamp);
    return E_FAIL;
  }
  FreeString(stamp);

  // ������
  if (FAILED(pStm->Read(&m_hPlugin, sizeof(m_hPlugin), 0)))
    return E_FAIL;

  // ���������
  if (FAILED(pStm->Read(&m_index, sizeof(m_index), 0)))
    return E_FAIL;

  // �������
  if (FAILED(m_param->LoadFromStream(pStm)))
    return E_FAIL;

  // ����Ŀ�б�
  ClearList();
  if (FAILED(pStm->Read(&cnt, sizeof(int), 0)))
    return E_FAIL;
  for (; cnt; cnt--)
  {
    if (FAILED(pStm->Read(&kind, sizeof(kind), 0)))
      return E_FAIL;

    switch(kind)
    {
      case CElement::Label:
        pnode = new CLabel;
    	  break;
      case CElement::Image:
        pnode = new CImage;
        break;
      case CElement::ShortCutBar:
        pnode = new CShortCutBar;
        break;
      case CElement::ChildPlugin:
        pnode = new CChildPlugin;
        break;
      default:
        ClearList();
        return E_FAIL;
        break;
    }

    if (FAILED(pnode->LoadFromStream(pStm)))
    {
      delete pnode;
      ClearList();
      return E_FAIL;
    }

    if (kind == CElement::ChildPlugin)
    {
      CChildPlugin* pchild = (CChildPlugin*)pnode;
      if (!LoadChildPlugin(pchild, pStm))
      {
        delete pnode;
        ClearList();
        return E_FAIL;
      }

      // ����Ƿ�������Ų��
      if (IsSameText(pchild->clsid, sSMSClsID))
        hasSms = true;
      else if (IsSameText(pchild->clsid, sSysSMSClsID))
        hasSysSms = true;
    }

    m_list->Add(pnode);
  }

  // ȫ������
  if (m_index == 0)
    if (FAILED(HomeData->LoadFromStream(pStm)))
      return E_FAIL;

  UpdateColors();
  InitData();

  // ��ͨ�Ķ���Ϣ������ܻ������Ϣ֪ͨ���ڴ˴���һ�����õı�׼�ʼ������ȡ��֪ͨ
  if (hasSms && !hasSysSms)
  {
    CreateInternalSmsPlugin();
  }

  HomeData->CheckData(true);

  DEBUGLOG(TEXT("PluginOrchid::Load - Success"));
  return S_OK;
}

// ���������ݵ���
HRESULT PluginOrchid::Save(IStream *pStm, BOOL fClearDirty)
{
  int i, cnt;
  CElement *pnode = NULL;
  CElement::TElementKind kind;

  DEBUGLOG(TEXT("PluginOrchid::Save - Called"));

  if (FAILED(CPersistent::SaveToStream(pStm)))
    return E_FAIL;

  // �������ʱ���ǩ
  if (FAILED(DoSaveString(pStm, BUILDSTAMP)))
    return E_FAIL;

  // ������
  if (FAILED(pStm->Write(&m_hPlugin, sizeof m_hPlugin, 0)))
    return E_FAIL;

  // ���������Ҳ��Ҫ���棬��Ϊֻ��������Ϊ 0 �Ĳ����Ҫ��ʼ��ȫ�����ݡ�
  // �����ǰ����ʹ���˲�������л�������ʱ��Home.exe ���ȴ�������������
  // �ͷžɲ�����������ܵ����²���� m_index ����ȷ��������������ʹ���˸�
  // �����������л�ʱʧ�ܣ�����������������ʱ����������
  if (FAILED(pStm->Write(&m_index, sizeof(m_index), 0)))
    return E_FAIL;

  // �������
  if (FAILED(m_param->SaveToStream(pStm)))
    return E_FAIL;

  // ����Ŀ�б�
  cnt = m_list->GetCount();
  if (FAILED(pStm->Write(&cnt, sizeof(int), 0)))
    return E_FAIL;

  for (i = 0; i < cnt; i++)
  {
    pnode = (CElement*)m_list->Get(i);
    kind = pnode->GetKind();

    if (FAILED(pStm->Write(&kind, sizeof(kind), 0)))
      return E_FAIL;
    if (FAILED(pnode->SaveToStream(pStm)))
      return E_FAIL;

    if (kind == CElement::ChildPlugin)
      if (!SaveChildPlugin((CChildPlugin*)pnode, pStm, fClearDirty))
        return E_FAIL;
  }

  // ȫ������
  if (m_index == 0)
    if (FAILED(HomeData->SaveToStream(pStm)))
      return E_FAIL;

  DEBUGLOG(TEXT("PluginOrchid::Save - Success"));
  return S_OK;
}

HRESULT PluginOrchid::GetSizeMax(ULARGE_INTEGER* pcbSize)
{
  ULONGLONG* pl = (ULONGLONG*)pcbSize;

  *pl = 0;

  return(S_OK);
}

/************************************************************************/
/* IHomePluginEnvironment2 �ӿڼ��Ӳ��������                         */
/************************************************************************/

HRESULT PluginOrchid::InvalidatePlugin(HPLUGIN hPlugin, DWORD dwFlags)
{
  return m_ppe->InvalidatePlugin(m_hPlugin, dwFlags);
}

HRESULT PluginOrchid::SetSingleShotTimer(HPLUGIN hPlugin, UINT cMiliseconds)
{
  return SetNextTimer(cMiliseconds);
}

HRESULT PluginOrchid::GetColor(HPLUGIN hPlugin, BSTR bstrColorName, COLORREF *pcr)
{
  return m_ppe->GetColor(m_hPlugin, bstrColorName, pcr);
}

// �����Ӳ���ӿ�
bool PluginOrchid::CreateChildPlugin(CChildPlugin* plugin)
{
  if (plugin->clsid[0])
  {
    DEBUGLOG1(L"CreateChildPlugin: %s", plugin->clsid);
    CLSID clsid;
    if (SUCCEEDED(CLSIDFromString(plugin->clsid, &clsid)))
    {
      CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        IID_IHomePlugin, (LPVOID *)&plugin->plugin);

      // Ϊ�Ӳ������ Site Ϊ���������
      if (plugin->plugin)
      {
        IUnknown* site;
        QueryInterface(IID_IHomePluginEnvironment2, (void**)&site);
        plugin->plugin->SetSite(site);
        site->Release();
      }

      // ���ÿ�ݷ�ʽ�Ĳ���ӿ�
      plugin->shortcut->plugin = plugin->plugin;
    }
  }

  DEBUGLOG1(L"Plugin: %x", (void*)plugin->plugin);
  return plugin->plugin != NULL;
}

// ����һ����ʹ�õı�׼�ʼ������ȡ��֪ͨ
void PluginOrchid::CreateInternalSmsPlugin()
{
  if (!m_smsplugin)
  {
    DEBUGLOG(L"CreateInternalSmsPlugin");
    m_smsplugin = new CChildPlugin();
    CloneString(m_smsplugin->clsid, sSysSMSClsID);
    CreateChildPlugin(m_smsplugin);

    IXMLDOMDocument* xml = NULL;
    if SUCCEEDED(XMLCreateDocument(&xml))
    {
      IXMLDOMNode* proot = NULL, *pnode = NULL, *pnodeDef = NULL, *pnodeMRU = NULL;
      VARIANT_BOOL succ = 0;
      xml->loadXML(sSMSXml, &succ);
      xml->get_firstChild(&proot);
      while (proot)
      {
        if (XMLNodeNameIs(proot, L"home"))
          break;
        proot->get_nextSibling(&proot);
      }

      if (proot)
      {
        proot->get_firstChild(&pnode);
        while (pnode)
        {
          if (XMLNodeNameIs(pnode, L"default"))
            pnodeDef = pnode;
          if (XMLNodeNameIs(pnode, L"plugin"))
            pnodeMRU = pnode;
          pnode->get_nextSibling(&pnode);
        }
        proot->Release();
      }

      if (pnodeDef && pnodeMRU)
      {
        HRESULT ret = m_smsplugin->plugin->Initialize(m_hPlugin, pnodeMRU, pnodeDef);
        DEBUGLOG1(L"SMS Initialize: %x", ret);
      }

      xml->Release();
    }
  }
}

// װ���Ӳ������
bool PluginOrchid::LoadChildPlugin(CChildPlugin* plugin, IStream* pStm)
{
  DEBUGLOG1(L"LoadChildPlugin: %s", plugin->clsid);

  bool ret = false;
  if (CreateChildPlugin(plugin))
  {

    IPersistStream* persist = NULL;
    if (SUCCEEDED(plugin->plugin->QueryInterface(IID_IPersistStream, (void**)&persist) && persist))
    {
      if (SUCCEEDED(persist->Load(pStm)))
        ret = true;
      persist->Release();
    }
  }

  return ret;
}

// �����Ӳ������
bool PluginOrchid::SaveChildPlugin(CChildPlugin* plugin, IStream* pStm, BOOL fClearDirty)
{
  DEBUGLOG1(L"SaveChildPlugin: %s", plugin->clsid);

  bool ret = false;
  IPersistStream* persist = NULL;
  if (SUCCEEDED(plugin->plugin->QueryInterface(IID_IPersistStream, (void**)&persist) && persist))
  {
    if (SUCCEEDED(persist->Save(pStm, fClearDirty)))
      ret = true;
    persist->Release();
  }

  // ���������ͷ��Ӳ��ʵ�������������Ӳ��������������ӿڣ���������������Զ��ͷ�
  plugin->ReleasePlugin();

  return ret;
}

// �����Ӳ���¼�
void PluginOrchid::DoChildPluginEvent(PluginEvent* ppe)
{
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CElement* node = (CElement*)m_list->Get(i);
    if (node->GetKind() == CElement::ChildPlugin)
    {
      CChildPlugin* plugin = (CChildPlugin*)node;
      if (plugin->plugin)
        plugin->plugin->OnEvent(ppe);
    }
  }
}

/************************************************************************/
/* ���ݸ���                                                             */
/************************************************************************/

// ȡ��ɫֵ
COLORREF PluginOrchid::GetColorValue(wchar_t* name, COLORREF crDef)
{
  COLORREF color = crDef;
  BSTR bstrName = NULL;
  extern wchar_t* sTransparent;
  if (name && name[0])
  {
    if (IsSameText(name, sTransparent))
      color = COLOR_TRANSPARENT;
    else
    {
      bstrName = SysAllocString(name);
      m_ppe->GetColor(m_hPlugin, bstrName, &color);
      SysFreeString(bstrName);
    }
  }
  return color;
}

// ����������ɫֵ
void PluginOrchid::UpdateFontColor(CFont* font)
{
  font->color->color = GetColorValue(font->color->name, COLOR_WINDOWTEXT);
  font->bgcolor->color = GetColorValue(font->bgcolor->name, COLOR_TRANSPARENT);
  font->shadowcolor->color = GetColorValue(font->shadowcolor->name, COLOR_BTNSHADOW);
}

// ����������ɫֵ
void PluginOrchid::UpdateColors()
{
  CElement* pnode;
  m_param->bordercolor->color = GetColorValue(m_param->bordercolor->name, COLOR_TRANSPARENT);
  UpdateFontColor(m_param->font);
  UpdateFontColor(m_param->selfont);
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    pnode = (CElement*)m_list->Get(i);
    UpdateFontColor(pnode->font);
    UpdateFontColor(pnode->selfont);

    if (pnode->GetKind() == CElement::Label)
    {
      // ���� Text ��ǩ��ɫ
      CLabel* label = (CLabel*)pnode;
      for (int j = 0; j < label->nodes->GetCount(); j++)
      {
        CTextNode* text = (CTextNode*)label->nodes->Get(j);
        text->color->color = GetColorValue(text->color->name, pnode->font->color->color);
        text->selcolor->color = GetColorValue(text->selcolor->name, pnode->selfont->color->color);
      }
    }
    else if (pnode->GetKind() == CElement::Image)
    {
      // ���� Image ��ɫ
      CImage* img = (CImage*)pnode;
      img->transcolor->color = GetColorValue(img->transcolor->name, COLOR_TRANSPARENT);
    }
  }
}

// ������һ�ζ�ʱ�����
HRESULT PluginOrchid::SetNextTimer(uint delay)
{
  // ��ʱ�����ܻ��ε��ã�������һ�μ���ܳ����ᶪʧ֮ǰ�����á�
  // ��������Ϊ�������һ�εĵ���
  int tick = GetTickCount();
  if (!m_nexttimer || (tick + delay < m_nexttimer))
  {
    m_nexttimer = tick + delay;
    return m_ppe->SetSingleShotTimer(m_hPlugin, delay);
  }

  return S_OK;
}

void PluginOrchid::Invalidate(bool checkheight)
{
  if (m_ppe)
  {
    int flag = 0;
    if (checkheight && CalcHeight())
      flag = IPF_HEIGHT_CHANGED;
    m_ppe->InvalidatePlugin(m_hPlugin, flag);
  }
}

// ��ʼ���������
void PluginOrchid::InitData()
{
  // �����������
  m_mask = DATA_NONE;
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CElement* pnode = (CElement*)m_list->Get(i);
    if (pnode->GetKind() == CElement::Label)
    {
      CLabel* plabel = (CLabel*)pnode;
      for (int j = 0; j < plabel->nodes->GetCount(); j++)
      {
        CTextNode* pText = (CTextNode*)plabel->nodes->Get(j);

        m_mask |= HomeData->GetMask(pText->data.node);

        if (pText->data.node == NT_RISETIME || pText->data.node == NT_TRANSITTIME || pText->data.node == NT_SETTIME ||
          pText->data.node == NT_LIGHTTIME || pText->data.node == NT_DARKTIME)
          HomeData->AddRiseSet(pText);
      }
    }
  }

  // �������뵽 HomeData
  HomeData->AddMask(m_mask);

  DEBUGLOG2(L"InitData: mask = %X interval = %d", m_mask, HomeData->interval);

  // ��ʼ����ǰѡ��Ŀ�ݷ�ʽ
  InitSelected();

  // �������߶�
  CalcHeight();
}

// ��ʼ����ǰѡ��Ŀ�ݷ�ʽ
void PluginOrchid::InitSelected()
{
  // ���ҵ�һ����Ч�Ŀ�ݷ�ʽ
  m_selnode = NULL;
  m_selected = NULL;
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CElement* pnode = (CElement*)m_list->Get(i);
    if (pnode->HasShortCut())
    {
      m_selnode = pnode;
      m_selected = (CShortCut*)m_selnode->GetFirstShortCut();
      SelectedChanged();
      break;
    }
  }
}

// ���������б�
void PluginOrchid::ClearList()
{
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CElement* pnode = (CElement*)m_list->Get(i);
    if (pnode->GetKind() == CElement::Label)
    {
      CLabel* plabel = (CLabel*)pnode;
      for (int j = 0; j < plabel->nodes->GetCount(); j++)
      {
        CTextNode* pText = (CTextNode*)plabel->nodes->Get(j);
        if (pText->data.node == NT_RISETIME || pText->data.node == NT_TRANSITTIME || pText->data.node == NT_SETTIME
          || pText->data.node == NT_LIGHTTIME || pText->data.node == NT_DARKTIME)
          HomeData->RemoveRiseSet(pText);
      }
    }
    delete pnode;
  }

  m_list->Clear();
}

// ��������̬�߶�
bool PluginOrchid::CalcHeight()
{
  if (m_param->data.height)
  {
    m_height = m_param->data.height;
  }
  else
  {
    int maxh = 0, th = 0;

    // �����ӽڵ����߶�
    for (int i = 0; i < m_list->GetCount(); i++)
    {
      CElement* pnode = (CElement*)m_list->Get(i);
      th = pnode->GetMaxY();
      if (th > maxh)
        maxh = th;
    }

    // ������ʱ�ټ��ϱ߿�߶�
    if (maxh > 0)
      maxh += m_param->data.paddingtop + m_param->data.paddingbottom + m_param->data.borderwidth + 1;

    // ����߶ȱ仯������
    if (maxh != m_height)
    {
      m_height = maxh;
      DEBUGLOG2(L"Height changed: %s, %d", m_param->name, m_height);
      return true;
    }
  }

  return false;
}

// ȡ Text �ڵ�ֵ
void PluginOrchid::GetTextNodeValue(CTextNode* pText, wchar_t* text, int slen)
{
  HomeData->GetTextNodeValue(pText, text, slen);
}

// ��ǰѡ��������
void PluginOrchid::SelectedChanged()
{
  if (m_selected && m_selected->relbar[0])
  {
    HomeData->SetShortCutGroup(m_selected->relbar, m_selected->data.relgroup);
  }

  Invalidate(false);

  HomeData->NotifyPlugins(this, DATA_SHORTCUT);
}

/************************************************************************/
/* CHomeNotifier ���෽��                                               */
/************************************************************************/

// ϵͳ�����ѱ��
void PluginOrchid::DataChanged(void* sender, uint mask)
{
  // ���ݱ�������������ж��Ƿ���Ҫ�ػ���
  if ((mask & m_mask) || (mask & DATA_FORCE))
  {
    DEBUGLOG2(L"DataChanged: %s mask: %x", m_param->name, mask);
    if ((mask & DATA_SHORTCUT) && (sender != this))
    {
      // ���Է��ֵ�һ�����������һ���������ʱ��ϵͳ���ػ����в��
      // Ϊ���ٲ���Ҫ��ˢ�£�����ʹ����ʱˢ�»���
      m_delayupdate = true;
      SetNextTimer(DELAY_SHORT);
    }
    else
    {
      Invalidate(true);
    }
  }
  
  // ���¿�ݷ�ʽ�������
  if  (mask & DATA_DAY)
  {
    bool repaint;
    for (int i = 0; i < m_list->GetCount(); i++)
    {
      CElement* node = (CElement*)m_list->Get(i);
      if (node->GetKind() == CElement::ShortCutBar)
      {
        CShortCutBar* bar = (CShortCutBar*)node;
        for (int j = 0; j < bar->nodes->GetCount(); j++)
        {
          CShortCut* shortcut = (CShortCut*)bar->nodes->Get(j);
          shortcut->CheckClickCount(repaint);
        }
      }
    }

    // �ػ���
    if (repaint)
      Invalidate(false);
  }
}

// ���ÿ�ݷ�ʽ��
void PluginOrchid::SetShortCutGroup(wchar_t* name, int group)
{
  for (int i = 0; i < m_list->GetCount(); i++)
  {
    CElement* pnode = (CElement*)m_list->Get(i);
    if (IsSameText(pnode->name, name))
    {
      CShortCut* shortcut = NULL;
      pnode->SetGroup(group, shortcut);
      if (pnode == m_selnode || m_selnode == NULL)
      {
        if (shortcut)
        {
          m_selnode = pnode;
          m_selected = shortcut;
          SelectedChanged();
        }
        else
        {
          NavAction(NULL, false);
        }
      }
    }
  }
}

// ���ز������
wchar_t* PluginOrchid::GetPluginName()
{
  return m_param->name;
}

// ���ز����ǰѡ��ڵ�
CShortCut* PluginOrchid::GetSelected()
{
  return m_selected;
}
