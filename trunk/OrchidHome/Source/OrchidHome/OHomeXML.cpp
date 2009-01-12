/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：插件 XML 解析单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OHomeXML.cpp,v 1.33 2006/06/08 05:38:38 zjy Exp $
* 备    注：
*******************************************************************************/

#include "OrchidHome.h"

/************************************************************************/
/* 常量变量定义                                                         */
/************************************************************************/

// 公共常量
wchar_t* sTransparent = L"transparent";

// 文本标签名
static wchar_t* NodeTypes[NT_END - NT_START + 1] = {
  L"str",                 // NT_TEXT
  L"space",               // NT_SPACE
  L"br",                  // NT_BR
  L"year",                // NT_YEAR
  L"month",               // NT_MONTH
  L"day",                 // NT_DAY
  L"cweek",               // NT_CWEEK
  L"hour",                // NT_HOUR
  L"minute",              // NT_MINUTE
  L"second",              // NT_SECOND
  L"lyear",               // NT_LYEAR
  L"lmonth",              // NT_LMONTH
  L"lleapmonth",          // NT_LLEAPMONTH
  L"lshortmonth",         // NT_LSHORTMONTH
  L"lmonthlen",           // NT_LMONTHLEN
  L"lday",                // NT_LDAY
  L"gzyear",              // NT_GZYEAR
  L"gzmonth",             // NT_GZMONTH
  L"gzday",               // NT_GZDAY
  L"gzhour",              // NT_GZHOUR
  L"jieqi",               // NT_JIEQI
  L"xingzuo",             // NT_XINGZUO
  L"shengxiao",           // NT_SHENGXIAO
  L"holiday",             // NT_HOLIDAY
  L"shortholiday",        // NT_SHORTHOLIDAY
  L"downtext",            // NT_DOWNTEXT
  L"downday",             // NT_DOWNDAY
  L"downhour",            // NT_DOWNHOUR
  L"downminute",          // NT_DOWNMINUTE
  L"downsecond",          // NT_DOWNSECOND
  L"lighttime",           // NT_LIGHTTIME
  L"risetime",            // NT_RISETIME
  L"transittime",         // NT_TRANSITTIME
  L"settime",             // NT_SETTIME
  L"darktime",            // NT_DARKTIME
  L"memoryload",          // NT_MEMORYLOAD
  L"totalmemory",         // NT_TOTALMEMORY
  L"freememory",          // NT_FREEMEMORY
  L"storeload",           // NT_STORELOAD
  L"totalstore",          // NT_TOTALSTORE
  L"freestore",           // NT_FREESTORE
  L"storageload",         // NT_STORAGELOAD
  L"totalstorage",        // NT_TOTALSTORAGE
  L"freestorage",         // NT_FREESTORAGE
  L"storagecardload",     // NT_STORAGECARDLOAD
  L"totalstoragecard",    // NT_TOTALSTORAGECARD
  L"freestoragecard",     // NT_FREESTORAGECARD
  L"battery",             // NT_BATTERY
  L"temperature",         // NT_TEMPERATURE
  L"voltage",             // NT_VOLTAGE
  L"current",             // NT_CURRENT
  L"avercurrent",         // NT_AVERCURRENT
  L"shortcuttitle",       // NT_SHORTCUTNAME
  L"shortcutdesc",        // NT_SHORTCUTDESC
  L"shortcutcount",       // NT_SHORTCUTCOUNT
  L"shortcutindex",       // NT_SHORTCUTINDEX
  L"signal"               // NT_SIGNAL
};

// 字体属性名
static wchar_t* sFontFace = L"font-face";
static wchar_t* sFontSize = L"font-size";
static wchar_t* sFontWidth = L"font-width";
static wchar_t* sFontBold = L"font-bold";
static wchar_t* sFontItalic = L"font-italic";
static wchar_t* sFontUnderline = L"font-underline";
static wchar_t* sClearType = L"cleartype";
static wchar_t* sXSpace = L"xspace";
static wchar_t* sYSpace = L"yspace";
static wchar_t* sShadow = L"shadow";
static wchar_t* sShadowX = L"shadowx";
static wchar_t* sShadowY = L"shadowy";
static wchar_t* sBGColor = L"bgcolor";
static wchar_t* sFGColor = L"fgcolor";
static wchar_t* sColor = L"color";
static wchar_t* sShadowColor = L"shadowcolor";
static wchar_t* sSelColor = L"selcolor";

// 插件属性名
static wchar_t* sDefault = L"default";
static wchar_t* sName = L"name";
static wchar_t* sHeight = L"height";
static wchar_t* sPaddingLeft = L"padding-left";
static wchar_t* sPaddingRight = L"padding-right";
static wchar_t* sPaddingTop = L"padding-top";
static wchar_t* sPaddingBottom = L"padding-bottom";
static wchar_t* sBorderColor = L"b-border-color";
static wchar_t* sBorderWidth = L"b-border-width";
static wchar_t* sNoBorder = L"noborder";
static wchar_t* sBackground = L"background";
static wchar_t* sFormat = L"format";
static wchar_t* sState = L"state";
static wchar_t* sSelected = L"selected";
static wchar_t* sLang = L"lang";

// 标签属性名
static wchar_t* sLabel = L"label";
static wchar_t* sX = L"x";
static wchar_t* sY = L"y";
static wchar_t* sW = L"w";
static wchar_t* sH = L"h";
static wchar_t* sEllipsis = L"ellipsis";
static wchar_t* sAlign = L"align";
static wchar_t* sVAlign = L"valign";

static EnumDef defAlign[] = {{DT_LEFT, L"left"}, {DT_CENTER, L"center"}, {DT_RIGHT, L"right"}, {DT_ALIGNSIDE, L"side"}};
static EnumDef defVAlign[] = {{DT_TOP, L"top"}, {DT_VCENTER, L"center"}, {DT_VCENTER, L"vcenter"}, {DT_BOTTOM, L"bottom"}};

// 文本属性名
static wchar_t* sText = L"text";
static wchar_t* sLongitude = L"longitude";
static wchar_t* sLatitude = L"latitude";
static wchar_t* sZoneTime = L"zonetime";
static wchar_t* sLen = L"len";
static wchar_t* sLS = L"ls";
static wchar_t* sRS = L"rs";
static wchar_t* sDecDig = L"decdig";
static wchar_t* sLeadZero = L"leadzero";
static wchar_t* sOperator = L"op";
static wchar_t* sOpNum = L"opnum";
static wchar_t* sValid = L"valid";
static wchar_t* sInvalid = L"invalid";
static wchar_t* sPluginName = L"pluginname";
static wchar_t* sReplace = L"replace";
static wchar_t* sFullMode = L"fullmode";
static wchar_t* sLevel = L"level";
static wchar_t* sIncDate = L"incdate";
static wchar_t* sSDay = L"sday";
static wchar_t* sEDay = L"eday";
static wchar_t* sShowSolar = L"showsolar";
static wchar_t* sShowLunar = L"showlunar";
static wchar_t* sShowWeek = L"showweek";
static wchar_t* sShowSolarBirth = L"showsolarbirth";
static wchar_t* sShowLunarBirth = L"showlunarbirth";
static wchar_t* sShowSolarDay = L"showsolarday";
static wchar_t* sShowLunarDay = L"showlunarday";
static wchar_t* sRelName = L"relname";
static wchar_t* sIsEnd = L"isend";

// DownCounter 属性名
static wchar_t* sDownCounter = L"downcounter";
static wchar_t* sYear = L"year";
static wchar_t* sMonth = L"month";
static wchar_t* sDay = L"day";
static wchar_t* sHour = L"hour";
static wchar_t* sMinute = L"minute";
static wchar_t* sSecond = L"second";

static EnumDef defOperator[] = {
  {OP_ADD, L"+x"}, {OP_ADD, L"x+"}, {OP_SUBX, L"-x"}, {OP_XSUB, L"x-"},
  {OP_MUL, L"x*"}, {OP_MUL, L"*x"}, {OP_DIVX, L"/x"}, {OP_XDIV, L"x/"}
};

// 图像属性名
static wchar_t* sImage = L"image";
static wchar_t* sBitmap = L"bitmap";
static wchar_t* sFileName = L"filename";
static wchar_t* sTransColor = L"transcolor";
static wchar_t* sBitmapTrans = L"transparent";
static wchar_t* sBorder = L"border";

// 快捷方式组
static wchar_t* sShortCutBar = L"shortcutbar";
static wchar_t* sIconSpacing = L"icon-spacing";
static wchar_t* sIconSize = L"icon-size";
static wchar_t* sIconMode = L"mode";
static wchar_t* sGrouped = L"grouped";
static wchar_t* sGroup = L"group";
static wchar_t* sSubDir = L"subdir";
static wchar_t* sAutoSort = L"autosort";
static wchar_t* sLastOnTop = L"lastontop";

static EnumDef defIconSize[] = {{ICON_SMALL, L"small"}, {ICON_LARGE, L"large"}};
static EnumDef defIconMode[] = {{SM_ICON, L"icon"}, {SM_TITLE, L"title"}};

// 快捷方式项
static wchar_t* sShortCut = L"shortcut";
static wchar_t* sTitle = L"title";
static wchar_t* sDesc = L"desc";
static wchar_t* sTarget = L"target";
static wchar_t* sArgs = L"args";
static wchar_t* sIconFile = L"iconfile";
static wchar_t* sIconIndex = L"iconindex";
static wchar_t* sRelBar = L"relbar";
static wchar_t* sRelGroup = L"relgroup";
static wchar_t* sCmd = L"cmd";
static wchar_t* sConfirm = L"confirm";

static EnumDef defCmd[] =
{
  {CMD_OPEN, L"Open"}, {CMD_CALLPLUGIN, L"CallPlugin"},
  {CMD_SHOWMAIN, L"ShowMain"}, {CMD_SHOWMAIN, L"ShowInfo"}, {CMD_SHOWCALENDAR, L"ShowCalendar"}, 
  {CMD_SHOWALARMS, L"ShowAlarms"}, {CMD_SHOWUSERHOLIDAY, L"ShowUserHoliday"}, {CMD_SHOWTOOLS, L"ShowTools"}, 
  {CMD_SHOWCALINFO, L"ShowCalendarInfo"}, {CMD_SHOWHOLIDAYINFO, L"ShowHolidayInfo"},
  {CMD_SHOWPOWERINFO, L"ShowPowerInfo"}, {CMD_SHOWMEMORYINFO, L"ShowMemoryInfo"},
  {CMD_SHOWOPTION, L"ShowOption"}, {CMD_SHOWABOUT, L"ShowAbout"},
  {CMD_RESTART, L"Restart"}, {CMD_SHUTDOWN, L"Shutdown"}
};

// 快捷方式目录
static wchar_t* sShortCutDir = L"shortcutdir";
static wchar_t* sDir = L"dir";
static wchar_t* sMask = L"mask";

// 子插件
static wchar_t* sChildPlugin = L"childplugin";
static wchar_t* sPlugin = L"plugin";
static wchar_t* sClsId = L"clsid";

// 其它字符串
static wchar_t* sMaskLnk = L"*.lnk";

/************************************************************************/
/* XML 参数解析                                                         */
/************************************************************************/

HRESULT PluginOrchid::XMLInitPlugin(HPLUGIN hPlugin, IXMLDOMNode* pnodeParams, IXMLDOMNode* pnodeDefault)
{
  HRESULT hr = S_OK;
  IXMLDOMNode* pNode = NULL;

  DEBUGLOG1(TEXT("PluginOrchid::Initialize called (%x)"), hPlugin);

  // 初始化配置参数
  m_param->data.height = 0;
  m_param->data.paddingleft = 4;
  m_param->data.paddingright = 4;
  m_param->data.paddingtop = 2;
  m_param->data.paddingbottom = 2;
  m_param->data.borderwidth = 1;
  m_param->font->data.height = 14;

  // 取得配置参数
  if (FAILED(XMLGetParamFromNode(pnodeDefault)))
    return E_FAIL;
  if (FAILED(XMLGetParamFromNode(pnodeParams)))
    return E_FAIL;

  // 取背景参数
  pnodeParams->get_firstChild(&pNode);
  while (pNode)
  {
    if (XMLNodeNameIs(pNode, sBackground))
      if (FAILED(XMLGetParamFromNode(pNode)))
        return E_FAIL;
    pNode->get_nextSibling(&pNode);
  }

  // 从 default 区取默认值
  pnodeParams->get_firstChild(&pNode);
  while (pNode)
  {
    if (XMLNodeNameIs(pNode, sDefault))
      if (FAILED(XMLGetParamFromNode(pNode)))
        return E_FAIL;
    pNode->get_nextSibling(&pNode);
  }

  // 取选中字体设置
  m_param->selfont->Clone(m_param->font);
  if (FAILED(XMLGetSelectedFontFromNode(pnodeDefault, m_param->selfont)))
    return E_FAIL;
  if (FAILED(XMLGetSelectedFontFromNode(pnodeParams, m_param->selfont)))
    return E_FAIL;

  // 无边界
  if (m_param->data.noborder)
  {
    m_param->data.borderwidth = 0;
    m_param->data.paddingleft = 0;
    m_param->data.paddingright = 0;
    m_param->data.paddingtop = 0;
    m_param->data.paddingbottom = 0;
  }

  // 分析子节点
  ClearList();
  pnodeParams->get_firstChild(&pNode);
  while (pNode)
  {
    if (XMLNodeNameIs(pNode, sLabel))
    {
      // Label 标签
      CLabel *plabel = new CLabel;
      XMLGetLabelFromNode(pNode, plabel);
      if (plabel->nodes->GetCount() > 0)
        m_list->Add(plabel);
      else
        delete plabel;
    }
    else if (XMLNodeNameIs(pNode, sImage))
    {
      // Image 标签
      CImage *pimg = new CImage;
      XMLGetImageFromNode(pNode, pimg);
      if (pimg->filename[0])
        m_list->Add(pimg);
      else
        delete pimg;
    }
    else if (XMLNodeNameIs(pNode, sShortCutBar))
    {
      // ShortCutBar 标签
      CShortCutBar *pbar = new CShortCutBar;
      XMLGetShortCutBarFromNode(pNode, pbar);
      if (pbar->nodes->GetCount() > 0 || pbar->dirs->GetCount() > 0)
        m_list->Add(pbar);
      else
        delete pbar;
    }
    else if (XMLNodeNameIs(pNode, sChildPlugin))
    {
      // ChildPlugin 标签
      CChildPlugin *plugin = new CChildPlugin;

      // 使用插件本身的句柄，否则可能导致部分插件初始化错误
      XMLGetChildPluginFromNode(m_hPlugin, pnodeDefault, pNode, plugin);
      if (plugin->plugin)
        m_list->Add(plugin);
      else
        delete plugin;
    }
    else if (XMLNodeNameIs(pNode, sDownCounter))
    {
      // DownCounter 标签
      CDownCounter *counter = new CDownCounter;
      XMLGetDownCounterFromNode(pNode, counter);
      if (counter->name && counter->name[0] && counter->text && counter->text[0] &&
        GetDateIsValid(counter->date.year, counter->date.month, counter->date.day) && 
        GetTimeIsValid(counter->time.hour, counter->time.minute, counter->time.second))
        HomeData->counters->Add(counter);
      else
        delete counter;
    }
    pNode->get_nextSibling(&pNode);
  }

  return(hr);
}

// 判断语言 ID 是否匹配
HRESULT PluginOrchid::XMLCheckLangAttr(IXMLDOMNode* pNode)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;
  wchar_t langstr[TEXT_LEN];
  int lang = 0;
  langstr[0] = 0;

  if (FAILED(pNode->get_attributes(&pNodeMap)))
    return E_FAIL;

  if (pNodeMap)
  {
    if (FAILED(XMLGetStrAttribute(pNodeMap, sLang, langstr, TEXT_LEN)))
      return E_FAIL;
    if (langstr[0]) 
      lang = _wtoi(langstr);

    pNodeMap->Release();
  }

  if (!langstr[0] || lang == HomeData->lid)
    return S_OK;
  else
    return S_FALSE;
}

// 从节点属性中取字体设置
HRESULT PluginOrchid::XMLGetFontFromNodeMap(IXMLDOMNamedNodeMap* pNodeMap, CFont* font)
{
  if (FAILED(XMLGetStrAttribute(pNodeMap, sFontFace, &font->fontface)))
    return E_FAIL;
  if (FAILED(XMLGetIntAttribute(pNodeMap, sFontSize, &font->data.height)))
    return E_FAIL;
  if (FAILED(XMLGetIntAttribute(pNodeMap, sFontWidth, &font->data.width)))
    return E_FAIL;
  if (FAILED(XMLGetBoolAttribute(pNodeMap, sFontBold, &font->data.bold)))
    return E_FAIL;
  if (FAILED(XMLGetBoolAttribute(pNodeMap, sFontItalic, &font->data.italic)))
    return E_FAIL;
  if (FAILED(XMLGetBoolAttribute(pNodeMap, sFontUnderline, &font->data.underline)))
    return E_FAIL;
  if (FAILED(XMLGetBoolAttribute(pNodeMap, sClearType, &font->data.cleartype)))
    return E_FAIL;
  if (FAILED(XMLGetIntAttribute(pNodeMap, sXSpace, &font->data.xspace)))
    return E_FAIL;
  if (FAILED(XMLGetIntAttribute(pNodeMap, sYSpace, &font->data.yspace)))
    return E_FAIL;
  if (FAILED(XMLGetBoolAttribute(pNodeMap, sShadow, &font->data.shadow)))
    return E_FAIL;
  if (FAILED(XMLGetIntAttribute(pNodeMap, sShadowX, &font->data.shadowx)))
    return E_FAIL;
  if (FAILED(XMLGetIntAttribute(pNodeMap, sShadowY, &font->data.shadowy)))
    return E_FAIL;
  if (FAILED(XMLGetStrAttribute(pNodeMap, sBGColor, &font->bgcolor->name)))
    return E_FAIL;
  if (FAILED(XMLGetStrAttribute(pNodeMap, sFGColor, &font->color->name)))
    return E_FAIL;
  if (FAILED(XMLGetStrAttribute(pNodeMap, sColor, &font->color->name)))
    return E_FAIL;
  if (FAILED(XMLGetStrAttribute(pNodeMap, sShadowColor, &font->shadowcolor->name)))
    return E_FAIL;

  return S_OK;
}

// 取选中字体设置
HRESULT PluginOrchid::XMLGetSelectedFontFromNode(IXMLDOMNode* pNode, CFont* font)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;
  IXMLDOMNode* pChild;
  wchar_t buff[NAME_LEN];

  pNode->get_firstChild(&pChild);
  while (pChild)
  {
    if (XMLNodeNameIs(pChild, sBackground))
    {
      if (FAILED(XMLGetSelectedFontFromNode(pChild, font)))
        return E_FAIL;
    }
    else if (XMLNodeNameIs(pChild, sFormat))
    {
      if (FAILED(pChild->get_attributes(&pNodeMap)))
        return E_FAIL;

      if (FAILED(XMLGetStrAttribute(pNodeMap, sState, buff, NAME_LEN)))
        return E_FAIL;

      if (IsSameText(buff, sSelected))
        XMLGetFontFromNodeMap(pNodeMap, font);

      pNodeMap->Release();
      pNodeMap = NULL;
    }
    pChild->get_nextSibling(&pChild);
  }

  return S_OK;
}

// 取插件默认参数值
HRESULT PluginOrchid::XMLGetDefaultParamFromNode(IXMLDOMNode* pNode)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;

  if (FAILED(pNode->get_attributes(&pNodeMap)))
    return E_FAIL;

  if (pNodeMap)
  {
    if (FAILED(XMLGetFontFromNodeMap(pNodeMap, m_param->font)))
      return E_FAIL;

    pNodeMap->Release();
  }

  return S_OK;
}

// 取插件参数值
HRESULT PluginOrchid::XMLGetParamFromNode(IXMLDOMNode* pNode)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;

  if (FAILED(pNode->get_attributes(&pNodeMap)))
    return E_FAIL;

  if (pNodeMap)
  {
    if (FAILED(XMLGetStrAttribute(pNodeMap, sName, &m_param->name)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sHeight, &m_param->data.height)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sPaddingLeft, &m_param->data.paddingleft)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sPaddingRight, &m_param->data.paddingright)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sPaddingTop, &m_param->data.paddingtop)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sPaddingBottom, &m_param->data.paddingbottom)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sBorderColor, &m_param->bordercolor->name)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sBorderWidth, &m_param->data.borderwidth)))
      return E_FAIL;
    if (FAILED(XMLGetBoolAttribute(pNodeMap, sNoBorder, &m_param->data.noborder)))
      return E_FAIL;

    pNodeMap->Release();
  }

  if (FAILED(XMLGetDefaultParamFromNode(pNode)))
    return E_FAIL;

  return S_OK;
}

// 取节点属性参数
HRESULT PluginOrchid::XMLGetElementFromNode(IXMLDOMNode* pNode, CElement* element)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;

  if (FAILED(pNode->get_attributes(&pNodeMap)))
    return E_FAIL;

  element->data.x = 0;
  element->data.y = 0;
  element->data.w = 0;
  element->data.h = 0;
  element->data.align = DT_LEFT;
  element->data.valign = DT_TOP;
  element->font->Clone(m_param->font);

  if (pNodeMap)
  {
    if (FAILED(XMLGetStrAttribute(pNodeMap, sName, &element->name)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sX, &element->data.x)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sY, &element->data.y)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sW, &element->data.w)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sH, &element->data.h)))
      return E_FAIL;
    if (FAILED(XMLGetBoolAttribute(pNodeMap, sEllipsis, &element->data.ellipsis)))
      return E_FAIL;
    if (FAILED(XMLGetEnumAttribute(pNodeMap, sAlign, &element->data.align, defAlign, LENOF(defAlign))))
      return E_FAIL;
    if (FAILED(XMLGetEnumAttribute(pNodeMap, sVAlign, &element->data.valign, defVAlign, LENOF(defVAlign))))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sGroup, &element->data.group)))
      return E_FAIL;

    if (FAILED(XMLGetFontFromNodeMap(pNodeMap, element->font)))
      return E_FAIL;

    pNodeMap->Release();
  }

  element->selfont->Clone(m_param->selfont);
  element->selfont->data.height = element->font->data.height;
  element->selfont->data.width = element->font->data.width;
  element->selfont->data.cleartype = element->font->data.cleartype;
  element->selfont->data.shadow = element->font->data.shadow;
  element->selfont->data.shadowx = element->font->data.shadowx;
  element->selfont->data.shadowy = element->font->data.shadowy;
  element->selfont->shadowcolor->Clone(element->font->shadowcolor);
  if (FAILED(XMLGetSelectedFontFromNode(pNode, element->selfont)))
    return E_FAIL;

  return S_OK;
}

// 取节点属性参数
HRESULT PluginOrchid::XMLGetStdElementFromNode(IXMLDOMNode* pNode, CStdElement* element)
{
  IXMLDOMNode *pChild = NULL;
  if (FAILED(XMLGetElementFromNode(pNode, element)))
    return E_FAIL;

  // 读取 ShortCut 信息
  pNode->get_firstChild(&pChild);
  while (pChild)
  {
    if ((XMLCheckLangAttr(pChild) == S_OK) && XMLNodeNameIs(pChild, sShortCut))
    {
      XMLGetShortCutFromNode(pChild, element->shortcut);
      break;
    }
    pChild->get_nextSibling(&pChild);
  }

  if (pChild)
    pChild->Release();

  return S_OK;
}

// 处理 Label 标签参数
HRESULT PluginOrchid::XMLGetLabelFromNode(IXMLDOMNode* pNode, CLabel* label)
{
  IXMLDOMNode *pText = NULL, *pChild = NULL;

  if (FAILED(XMLGetStdElementFromNode(pNode, label)))
    return E_FAIL;

  // 读取 Text 列表
  pNode->get_firstChild(&pText);
  while (pText)
  {
    if ((XMLCheckLangAttr(pText) == S_OK) && XMLNodeNameIs(pText, sText))
    {
      int nodeType;
      DOMNodeType nType;
      CTextNode* textNode;

      pText->get_firstChild(&pChild);
      while (pChild)
      {
        if (SUCCEEDED(pChild->get_nodeType(&nType)))
        {
          nodeType = -1;
          if (nType == NODE_TEXT)
          {
            nodeType = NT_TEXT;
          }
          else if (nType == NODE_ELEMENT)
          {
            for (int i = NT_START; i <= NT_END; i++)
            {
              if (XMLNodeNameIs(pChild, NodeTypes[i - NT_START]))
              {
                nodeType = i;
                break;
              }
            }
          }

          if (nodeType >= 0)
          {
            textNode = new CTextNode;
            textNode->data.node = nodeType;
            if (nType == NODE_TEXT)
            {
              if (FAILED(XMLGetNodeValue(pChild, &textNode->text)))
              {
                delete textNode;
                return E_FAIL;
              }
            }
            else if (FAILED(XMLGetTextFromNode(pChild, textNode)))
            {
              delete textNode;
              return E_FAIL;
            }
            label->nodes->Add(textNode);
          }
        }
        pChild->get_nextSibling(&pChild);
      }

      if (pChild)
      {
        pChild->Release();
        pChild = NULL;
      }

      break;
    }
    pText->get_nextSibling(&pText);
  }

  if (pText)
    pText->Release();

  return S_OK;
}

// 处理 Text 子标签参数
HRESULT PluginOrchid::XMLGetTextFromNode(IXMLDOMNode* pNode, CTextNode* text)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;
  if (FAILED(pNode->get_attributes(&pNodeMap)))
    return E_FAIL;
  if (pNodeMap)
  {
    if (text->data.node == NT_RISETIME || text->data.node == NT_TRANSITTIME || text->data.node == NT_SETTIME ||
      text->data.node == NT_LIGHTTIME || text->data.node == NT_DARKTIME)
    {
      // 日出日落时间
      text->data.geo = HomeData->geo;
      if (FAILED(XMLGetDoubleAttribute(pNodeMap, sLongitude, &text->data.geo.longitude)))
        return E_FAIL;
      if (FAILED(XMLGetDoubleAttribute(pNodeMap, sLatitude, &text->data.geo.latitude)))
        return E_FAIL;
      if (FAILED(XMLGetIntAttribute(pNodeMap, sZoneTime, &text->data.geo.zonetime)))
        return E_FAIL;
    }
    else if (text->data.node == NT_HOLIDAY || text->data.node == NT_SHORTHOLIDAY)
    {
      // 节假日属性
      bool isshow;
      text->data.level = -1;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sFullMode, &text->data.fullmode)))
        return E_FAIL;
      if (FAILED(XMLGetIntAttribute(pNodeMap, sLevel, &text->data.level)))
        return E_FAIL;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sIncDate, &text->data.incdate)))
        return E_FAIL;
      if (FAILED(XMLGetIntAttribute(pNodeMap, sSDay, &text->data.sday)))
        return E_FAIL;
      text->data.eday = text->data.sday;
      if (FAILED(XMLGetIntAttribute(pNodeMap, sEDay, &text->data.eday)))
        return E_FAIL;
      isshow = true;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sShowSolar, &isshow)))
        return E_FAIL;
      if (isshow) text->data.showmask |= (1 << htSolar);
      isshow = true;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sShowLunar, &isshow)))
        return E_FAIL;
      if (isshow) text->data.showmask |= (1 << htLunar);
      isshow = true;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sShowWeek, &isshow)))
        return E_FAIL;
      if (isshow) text->data.showmask |= (1 << htWeek);
      isshow = true;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sShowSolarBirth, &isshow)))
        return E_FAIL;
      if (isshow) text->data.showmask |= (1 << htSolarBirth);
      isshow = true;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sShowLunarBirth, &isshow)))
        return E_FAIL;
      if (isshow) text->data.showmask |= (1 << htLunarBirth);
      isshow = true;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sShowSolarDay, &isshow)))
        return E_FAIL;
      if (isshow) text->data.showmask |= (1 << htSolarDay);
      isshow = true;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sShowLunarDay, &isshow)))
        return E_FAIL;
      if (isshow) text->data.showmask |= (1 << htLunarDay);
    }
    else
    {
      if (text->data.node == NT_MINUTE || text->data.node == NT_SECOND)
      {
        text->data.len = 2;
        text->data.leadzero = true;
      }
      else if (text->data.node == NT_SPACE)
        text->data.len = 1;
      text->data.op = OP_NONE;
      text->data.valid = DEF_VALID;
      text->data.invalid = DEF_VALID;
      CloneString(text->pluginname, m_param->name);
      if (FAILED(XMLGetIntAttribute(pNodeMap, sLen, &text->data.len)))
        return E_FAIL;
      if (FAILED(XMLGetIntAttribute(pNodeMap, sDecDig, &text->data.decdig)))
        return E_FAIL;
      if (FAILED(XMLGetIntAttribute(pNodeMap, sValid, &text->data.valid)))
        return E_FAIL;
      if (FAILED(XMLGetIntAttribute(pNodeMap, sInvalid, &text->data.invalid)))
        return E_FAIL;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sLeadZero, &text->data.leadzero)))
        return E_FAIL;
      if (FAILED(XMLGetEnumAttribute(pNodeMap, sOperator, &text->data.op, defOperator, LENOF(defOperator))))
        return E_FAIL;
      if (FAILED(XMLGetDoubleAttribute(pNodeMap, sOpNum, &text->data.opnum)))
        return E_FAIL;
      if (FAILED(XMLGetBoolAttribute(pNodeMap, sIsEnd, &text->data.isend)))
        return E_FAIL;
      if (FAILED(XMLGetStrAttribute(pNodeMap, sRelName, &text->relname)))
        return E_FAIL;
      if (text->data.len < 0) text->data.len = 0;
      if (text->data.decdig < 0) text->data.decdig = 0;
    }

    if (FAILED(XMLGetStrAttribute(pNodeMap, sLS, &text->ls)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sRS, &text->rs)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sPluginName, &text->pluginname)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sReplace, &text->replace)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sColor, &text->color->name)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sSelColor, &text->selcolor->name)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sText, &text->text)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sGroup, &text->data.group)))
      return E_FAIL;

    pNodeMap->Release();
  }

  return S_OK;
}

// 处理 Image 标签参数
HRESULT PluginOrchid::XMLGetImageFromNode(IXMLDOMNode* pNode, CImage* image)
{
  IXMLDOMNode *pImg = NULL;

  if (FAILED(XMLGetStdElementFromNode(pNode, image)))
    return E_FAIL;

  pNode->get_firstChild(&pImg);
  while (pImg)
  {
    // 检查语言 ID
    if (XMLCheckLangAttr(pImg) == S_OK)
    {
      // 读 Bitmap 标签
      if (XMLNodeNameIs(pImg, sBitmap))
      {
        IXMLDOMNamedNodeMap *pNodeMap = NULL;
        if (FAILED(pImg->get_attributes(&pNodeMap)))
          return E_FAIL;

        if (pNodeMap)
        {
          if (FAILED(XMLGetStrAttribute(pNodeMap, sFileName, &image->filename)))
            return E_FAIL;
          if (FAILED(XMLGetBoolAttribute(pNodeMap, sBitmapTrans, &image->info.transparent)))
            return E_FAIL;
          if (FAILED(XMLGetStrAttribute(pNodeMap, sTransColor, &image->transcolor->name)))
            return E_FAIL;
          if (FAILED(XMLGetIntAttribute(pNodeMap, sBorder, &image->info.border)))
            return E_FAIL;

          HomeData->GetFullPathFileName(image->filename);

          pNodeMap->Release();
        }
        break;
      }
    }
    pImg->get_nextSibling(&pImg);
  }

  if (pImg)
    pImg->Release();

  return S_OK;
}

// 处理 ShortCutBar 标签参数
HRESULT PluginOrchid::XMLGetShortCutBarFromNode(IXMLDOMNode* pNode, CShortCutBar* bar)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;
  IXMLDOMNode *pShortCut = NULL, *pChild = NULL;

  if (FAILED(XMLGetElementFromNode(pNode, bar)))
    return E_FAIL;

  bar->bar.iconspacing = 2;
  bar->bar.iconsize = ICON_SMALL;
  bar->bar.mode = SM_ICON;
  bar->bar.grouped = false;
  bar->bar.group = 0;

  if (FAILED(pNode->get_attributes(&pNodeMap)))
    return E_FAIL;

  if (pNodeMap)
  {
    if (FAILED(XMLGetIntAttribute(pNodeMap, sIconSpacing, &bar->bar.iconspacing)))
      return E_FAIL;
    if (FAILED(XMLGetEnumAttribute(pNodeMap, sIconSize, &bar->bar.iconsize, defIconSize, LENOF(defIconSize))))
      return E_FAIL;
    if (FAILED(XMLGetEnumAttribute(pNodeMap, sIconMode, &bar->bar.mode, defIconMode, LENOF(defIconMode))))
      return E_FAIL;
    if (FAILED(XMLGetBoolAttribute(pNodeMap, sGrouped, &bar->bar.grouped)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sGroup, &bar->bar.group)))
      return E_FAIL;
    if (FAILED(XMLGetBoolAttribute(pNodeMap, sAutoSort, &bar->bar.autosort)))
      return E_FAIL;
    if (FAILED(XMLGetBoolAttribute(pNodeMap, sLastOnTop, &bar->bar.lastontop)))
      return E_FAIL;

    pNodeMap->Release();
  }

  // 处理 ShortCut 子节点
  int idx = 0;
  pNode->get_firstChild(&pShortCut);
  while (pShortCut)
  {
    if (XMLCheckLangAttr(pShortCut) == S_OK)
    {
      if (XMLNodeNameIs(pShortCut, sShortCut))
      {
        CShortCut* shortcut;

        shortcut = new CShortCut(bar);
        if (FAILED(XMLGetShortCutFromNode(pShortCut, shortcut)))
        {
          delete shortcut;
          return E_FAIL;
        }

        if (shortcut->data.exist)
        {
          shortcut->data.index = idx++;
          bar->nodes->Add(shortcut);
        }
        else
          delete shortcut;
      }
      else if (XMLNodeNameIs(pShortCut, sShortCutDir))
      {
        CShortCutDir* dir;

        dir = new CShortCutDir;
        if (FAILED(XMLGetShortCutDirFromNode(pShortCut, dir)))
        {
          delete dir;
          return E_FAIL;
        }

        // 必须是以 \ 开头 或 %CE23% 这样的绝对路径才支持
        if (dir->dir[0] == '\\' || dir->dir[0] == '%')
        {
          dir->data.index = idx;
          bar->dirs->Add(dir);

          // 为了保证从目录中读出来的快捷方式序号唯一，这里加上一个大数
          idx += 1000;
        }
        else
          delete dir;
      }
    }
    pShortCut->get_nextSibling(&pShortCut);
  }

  return S_OK;
}

// 处理 ShortCut 标签参数
HRESULT PluginOrchid::XMLGetShortCutFromNode(IXMLDOMNode* pNode, CShortCut* shortcut)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;

  if (FAILED(pNode->get_attributes(&pNodeMap)))
    return E_FAIL;

  if (pNodeMap)
  {
    if (FAILED(XMLGetStrAttribute(pNodeMap, sTitle, &shortcut->title)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sDesc, &shortcut->desc)))
      return E_FAIL;
    if (FAILED(XMLGetEnumAttribute(pNodeMap, sCmd, &shortcut->data.cmd, defCmd, LENOF(defCmd))))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sTarget, &shortcut->target)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sArgs, &shortcut->args)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sIconFile, &shortcut->iconfile)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sIconIndex, &shortcut->data.iconindex)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sGroup, &shortcut->data.group)))
      return E_FAIL;
    if (FAILED(XMLGetBoolAttribute(pNodeMap, sConfirm, &shortcut->data.confirm)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sRelBar, &shortcut->relbar)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sRelGroup, &shortcut->data.relgroup)))
      return E_FAIL;

    HomeData->GetFullPathFileName(shortcut->target);

    shortcut->data.exist = true;

    pNodeMap->Release();
  }

  return S_OK;
}

// 处理 ShortCutDir 标签参数
HRESULT PluginOrchid::XMLGetShortCutDirFromNode(IXMLDOMNode* pNode, CShortCutDir* dir)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;

  if (FAILED(pNode->get_attributes(&pNodeMap)))
    return E_FAIL;

  if (pNodeMap)
  {
    if (FAILED(XMLGetIntAttribute(pNodeMap, sGroup, &dir->data.group)))
      return E_FAIL;
    if (FAILED(XMLGetBoolAttribute(pNodeMap, sSubDir, &dir->data.subdir)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sDir, &dir->dir)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sMask, &dir->mask)))
      return E_FAIL;

    PathUnixToWin(dir->dir);
    int len = wcslen(dir->dir);
    if (dir->dir[len - 1] == '\\')
      dir->dir[len - 1] = 0;
    if (!dir->mask[0])
      CloneString(dir->mask, sMaskLnk);

    pNodeMap->Release();
  }

  return S_OK;
}

// 处理 ChildPlugin 标签参数
HRESULT PluginOrchid::XMLGetChildPluginFromNode(HPLUGIN hPlugin, IXMLDOMNode* pnodeDefault,
                                                IXMLDOMNode* pNode, CChildPlugin* plugin)
{
  IXMLDOMNode *pChild = NULL;

  if (FAILED(XMLGetStdElementFromNode(pNode, plugin)))
    return E_FAIL;

  // 处理 plugin 子节点
  int idx = 0;
  pNode->get_firstChild(&pChild);
  while (pChild)
  {
    if (XMLNodeNameIs(pChild, sPlugin))
    {
      IXMLDOMNamedNodeMap *pNodeMap = NULL;
      if (FAILED(pChild->get_attributes(&pNodeMap)))
        return E_FAIL;

      if (pNodeMap)
      {
        if (FAILED(XMLGetStrAttribute(pNodeMap, sClsId, &plugin->clsid)))
          return E_FAIL;

        pNodeMap->Release();

        if (CreateChildPlugin(plugin))
        {
          HRESULT hr = plugin->plugin->Initialize(hPlugin, pChild, pnodeDefault);
          if (FAILED(hr))
          {
            DEBUGLOG1(L"Child plugin initialize fail: %x", hr);
            plugin->ReleasePlugin();
          }
        }

        break;
      }
    }
    pChild->get_nextSibling(&pChild);
  }

  if (pChild)
    pChild->Release();

  return S_OK;
}

// 处理 DownCounter 标签
HRESULT PluginOrchid::XMLGetDownCounterFromNode(IXMLDOMNode* pNode, CDownCounter* counter)
{
  IXMLDOMNamedNodeMap *pNodeMap = NULL;

  if (FAILED(pNode->get_attributes(&pNodeMap)))
    return E_FAIL;

  if (pNodeMap)
  {
    if (FAILED(XMLGetStrAttribute(pNodeMap, sName, &counter->name)))
      return E_FAIL;
    if (FAILED(XMLGetStrAttribute(pNodeMap, sText, &counter->text)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sYear, &counter->date.year)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sMonth, &counter->date.month)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sDay, &counter->date.day)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sHour, &counter->time.hour)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sMinute, &counter->time.minute)))
      return E_FAIL;
    if (FAILED(XMLGetIntAttribute(pNodeMap, sSecond, &counter->time.second)))
      return E_FAIL;

    pNodeMap->Release();
  }

  return S_OK;
}
