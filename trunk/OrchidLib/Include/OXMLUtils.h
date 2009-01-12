/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：XML 处理函数头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OXMLUtils.h,v 1.8 2006/10/03 04:59:05 Administrator Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <windows.h>
#include <memory.h>
#include <msxml.h>

struct EnumDef
{
  int value;
  wchar_t* name;
};

HRESULT XMLCreateDocument(IXMLDOMDocument** XMLDoc);

HRESULT XMLLoadFromFile(IXMLDOMDocument* XMLDoc, wchar_t* filename);

HRESULT XMLSaveToFile(IXMLDOMDocument* XMLDoc, wchar_t* filename);

HRESULT XMLAppendTextNode(IXMLDOMDocument* xml, IXMLDOMElement* element, wchar_t* text);

HRESULT XMLInsertAfter(IXMLDOMNode* pNode, IXMLDOMNode* newNode, IXMLDOMNode *outNew);

HRESULT XMLInsertBefore(IXMLDOMNode* pNode, IXMLDOMNode* newNode, IXMLDOMNode *outNew);

HRESULT XMLInsertTextAfter(IXMLDOMNode* pNode, wchar_t* text);

HRESULT XMLInsertTextBefore(IXMLDOMNode* pNode, wchar_t* text);

HRESULT XMLGetIntAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, int* pVal);

HRESULT XMLGetBoolAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, bool* pVal);

HRESULT XMLGetDoubleAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, double* pVal);

HRESULT XMLGetStrAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, wchar_t* str, int len);

HRESULT XMLGetStrAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, wchar_t** pStr);

HRESULT XMLGetEnumAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, int* pVal, EnumDef def[], int len);

HRESULT XMLSetIntAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, int val);

HRESULT XMLSetBoolAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, bool val, bool strbool = true);

HRESULT XMLSetDoubleAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, double val);

HRESULT XMLSetStrAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, wchar_t* str);

HRESULT XMLSetEnumAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, int val, EnumDef def[], int len);

HRESULT XMLGetNodeName(IXMLDOMNode* pNode, wchar_t* name, int slen);

bool XMLNodeNameIs(IXMLDOMNode* pNode, wchar_t* name);

HRESULT XMLGetNodeText(IXMLDOMNode* pNode, wchar_t* text, int slen);

HRESULT XMLGetNodeValue(IXMLDOMNode* pNode, wchar_t* text, int slen);

HRESULT XMLGetNodeValue(IXMLDOMNode* pNode, wchar_t** ptext);

HRESULT XMLGetChildByName(IXMLDOMNode* pNode, wchar_t* name, IXMLDOMNode** pChild);
