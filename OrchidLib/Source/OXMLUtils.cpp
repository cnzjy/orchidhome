/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：XML 处理函数
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OXMLUtils.cpp,v 1.10 2006/10/03 04:59:05 Administrator Exp $
* 备    注：
*******************************************************************************/

#include <objsafe.h>
#include "OXMLUtils.h"
#include "OUtils.h"

HRESULT XMLCreateDocument(IXMLDOMDocument** XMLDoc)
{
  HRESULT hr;
  IXMLDOMDocument *doc = NULL;

  hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
                        IID_IXMLDOMDocument, (LPVOID *)&doc);
  if(doc)
  {
    doc->put_async(VARIANT_FALSE);

    // Smartphone workaround:
    // Remove document safety options
    IObjectSafety *pSafety;
    DWORD dwSupported, dwEnabled;

    if (SUCCEEDED(doc->QueryInterface(IID_IObjectSafety, (void**)&pSafety)))
    {
      pSafety->GetInterfaceSafetyOptions(IID_IXMLDOMDocument, &dwSupported, &dwEnabled);
      pSafety->SetInterfaceSafetyOptions(IID_IXMLDOMDocument, dwSupported, 0);
    }
  }

  *XMLDoc = doc;
  return hr;
}

static HRESULT _XMLDoLoadSave(IXMLDOMDocument* XMLDoc, wchar_t* filename, bool isSave)
{
  VARIANT_BOOL isSucc;
  VARIANT varName;
  BSTR bstrName;
  HRESULT hr = S_FALSE;

  bstrName = SysAllocString(filename);
  VariantInit(&varName);
  varName.vt = VT_BSTR;
  varName.bstrVal = bstrName;
  if (isSave)
  {
    hr = XMLDoc->save(varName);
  }
  else
  {
    if (SUCCEEDED(XMLDoc->load(varName, &isSucc) && isSucc))
      hr = S_OK;
  }
  SysFreeString(bstrName);
  varName.bstrVal = NULL;
  VariantClear(&varName);

  return hr;
}

HRESULT XMLLoadFromFile(IXMLDOMDocument* XMLDoc, wchar_t* filename)
{
  return _XMLDoLoadSave(XMLDoc, filename, false);
}

HRESULT XMLSaveToFile(IXMLDOMDocument* XMLDoc, wchar_t* filename)
{
  DeleteFile(filename);
  return _XMLDoLoadSave(XMLDoc, filename, true);
}

HRESULT XMLAppendTextNode(IXMLDOMDocument* xml, IXMLDOMElement* element, wchar_t* text)
{
  IXMLDOMText* node = NULL;
  xml->createTextNode(text, &node);
  if (node)
    return element->appendChild(node, NULL);
  else
    return S_FALSE;
}

HRESULT XMLInsertAfter(IXMLDOMNode* pNode, IXMLDOMNode* newNode, IXMLDOMNode *outNew)
{
  IXMLDOMNode* parent;
  HRESULT hr = S_FALSE;
  if (FAILED(hr = pNode->get_parentNode(&parent)))
    return hr;
  if (FAILED(hr = pNode->get_nextSibling(&pNode)))
    return hr;

  if (parent)
  {
    if (pNode)
    {
      VARIANT varRef;
      VariantInit(&varRef);
      varRef.vt = VT_UNKNOWN;
      varRef.punkVal = pNode;
      hr = parent->insertBefore(newNode, varRef, &outNew);
    }
    else
    {
      hr = parent->appendChild(newNode, &outNew);
    }
  }

  return hr;
}

HRESULT XMLInsertBefore(IXMLDOMNode* pNode, IXMLDOMNode* newNode, IXMLDOMNode *outNew)
{
  IXMLDOMNode* parent;
  HRESULT hr = S_FALSE;
  if (FAILED(hr = pNode->get_parentNode(&parent)))
    return hr;

  if (parent)
  {
    VARIANT varRef;
    VariantInit(&varRef);
    varRef.vt = VT_UNKNOWN;
    varRef.punkVal = pNode;
    hr = parent->insertBefore(newNode, varRef, &outNew);
  }

  return hr;
}

HRESULT XMLInsertTextAfter(IXMLDOMNode* pNode, wchar_t* text)
{
  IXMLDOMDocument* xml;
  IXMLDOMText* node = NULL;
  HRESULT hr = E_FAIL;
  if (FAILED(hr = pNode->get_ownerDocument(&xml)))
    return hr;
  xml->createTextNode(text, &node);
  return XMLInsertAfter(pNode, node, NULL);
}

HRESULT XMLInsertTextBefore(IXMLDOMNode* pNode, wchar_t* text)
{
  IXMLDOMDocument* xml;
  IXMLDOMText* node = NULL;
  HRESULT hr = E_FAIL;
  if (FAILED(hr = pNode->get_ownerDocument(&xml)))
    return hr;
  xml->createTextNode(text, &node);
  return XMLInsertBefore(pNode, node, NULL);
}

static HRESULT _XMLGetAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, VARTYPE vt, void* pVal, int len)
{
  BSTR bstrName = NULL;
  VARIANT val, varRet;
  IXMLDOMNode* pNode = NULL;

  if (!pAttr || !name || name[0] == 0 || !pVal)
    return S_OK;

  bstrName = SysAllocString(name);
  if (bstrName == NULL)
    return E_OUTOFMEMORY;

  VariantInit(&val);
  VariantInit(&varRet);

  if (SUCCEEDED(pAttr->getNamedItem(bstrName, &pNode)) && pNode)
  {
    if (SUCCEEDED(pNode->get_nodeValue(&val)))
    {
      if (SUCCEEDED(VariantChangeType(&varRet, &val, 0, vt)))
      {
        if (vt == VT_INT)
          *(int*)pVal = varRet.intVal;
        else if (vt == VT_BOOL)
          *(bool*)pVal = varRet.boolVal ? true : false;
        else if (vt == VT_R8)
          *(double*)pVal = varRet.dblVal;
        else if (vt == VT_BSTR)
        {
          if (len > 0)
          {
            wcsncpy((wchar_t*)pVal, varRet.bstrVal, len - 1);
          }
          else
          {
            AllocString(*(wchar_t**)pVal, wcslen(varRet.bstrVal));
            wcscpy(*(wchar_t**)pVal, varRet.bstrVal);
          }
        }
      }
    }
  }

  if (pNode)
    pNode->Release();

  VariantClear(&val);
  VariantClear(&varRet);

  SysFreeString(bstrName);

  return S_OK;
}

HRESULT XMLGetIntAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, int* pVal)
{
  return _XMLGetAttribute(pAttr, name, VT_INT, pVal, 0);
}

HRESULT XMLGetBoolAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, bool* pVal)
{
  return _XMLGetAttribute(pAttr, name, VT_BOOL, pVal, 0);
}

HRESULT XMLGetDoubleAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, double* pVal)
{
  return _XMLGetAttribute(pAttr, name, VT_R8, pVal, 0);
}

HRESULT XMLGetStrAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, wchar_t* str, int len)
{
  return _XMLGetAttribute(pAttr, name, VT_BSTR, str, len);
}

HRESULT XMLGetStrAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, wchar_t** pStr)
{
  return _XMLGetAttribute(pAttr, name, VT_BSTR, pStr, 0);
}

HRESULT XMLGetEnumAttribute(IXMLDOMNamedNodeMap* pAttr, wchar_t* name, int* pVal, EnumDef def[], int len)
{
  wchar_t buff[256];

  buff[0] = 0;
  if (FAILED(XMLGetStrAttribute(pAttr, name, buff, 256)))
  {
    return E_FAIL;
  }
  else
  {
    for (int i = 0; i < len; i++)
    {
      if (IsSameText(buff, def[i].name))
      {
        *pVal = def[i].value;
        return S_OK;
      }
    }
  }

  return S_OK;
}

static HRESULT _XMLSetAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, VARTYPE vt, void* pVal)
{
  BSTR bstrName = NULL;
  BSTR bstrValue = NULL;
  VARIANT val;
  IXMLDOMAttribute* pNode = NULL;

  if (!pAttr || !name || name[0] == 0 || !pVal)
    return S_OK;

  bstrName = SysAllocString(name);
  if (bstrName == NULL)
    return E_OUTOFMEMORY;

  VariantInit(&val);

  val.vt = vt;
  if (vt == VT_INT)
    val.intVal = *(int*)pVal;
  else if (vt == VT_BOOL)
    val.boolVal = *(bool*)pVal ? true : false;
  else if (vt == VT_R8)
    val.dblVal = *(double*)pVal;
  else if (vt == VT_BSTR)
  {
    bstrValue = SysAllocString((wchar_t*)pVal);
    if (bstrValue == NULL)
      return E_OUTOFMEMORY;
    val.bstrVal = bstrValue;
  }

  pDoc->createAttribute(bstrName, &pNode);
  if (pNode)
  {
    pNode->put_nodeValue(val);
    pAttr->setNamedItem(pNode, NULL);
    pNode->Release();
  }
  else
    return E_OUTOFMEMORY;

  SysFreeString(bstrName);

  if (bstrValue)
  {
    SysFreeString(bstrValue);
    val.bstrVal = NULL;
  }

  VariantClear(&val);

  return S_OK;
}

HRESULT XMLSetIntAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, int val)
{
  return _XMLSetAttribute(pDoc, pAttr, name, VT_INT, &val);
}

HRESULT XMLSetBoolAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, bool val, bool strbool)
{
  if (strbool)
    return _XMLSetAttribute(pDoc, pAttr, name, VT_BSTR, val ? L"true" : L"false");
  else
    return _XMLSetAttribute(pDoc, pAttr, name, VT_BOOL, &val);
}

HRESULT XMLSetDoubleAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, double val)
{
  return _XMLSetAttribute(pDoc, pAttr, name, VT_R8, &val);
}

HRESULT XMLSetStrAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, wchar_t* str)
{
  return _XMLSetAttribute(pDoc, pAttr, name, VT_BSTR, str);
}

HRESULT XMLSetEnumAttribute(IXMLDOMDocument* pDoc, IXMLDOMNamedNodeMap* pAttr, wchar_t* name, int val, EnumDef def[], int len)
{
  for (int i = 0; i < len; i++)
  {
    if (def[i].value == val)
      return XMLSetStrAttribute(pDoc, pAttr, name, def[i].name);
  }
  return S_FALSE;
}

HRESULT XMLGetNodeName(IXMLDOMNode* pNode, wchar_t* name, int slen)
{
  BSTR bstrName = NULL;

  if (!pNode || !name)
    return S_OK;

  name[0] = 0;
  if (SUCCEEDED(pNode->get_nodeName(&bstrName)))
    wcsncpy(name, bstrName, slen - 1);

  SysFreeString(bstrName);

  return S_OK;
}

bool XMLNodeNameIs(IXMLDOMNode* pNode, wchar_t* name)
{
  bool ret = false;
  BSTR bstrName = NULL;

  if (!pNode)
    return ret;

  if (SUCCEEDED(pNode->get_nodeName(&bstrName)))
    ret = IsSameText(name, bstrName);

  SysFreeString(bstrName);

  return ret;
}

HRESULT XMLGetNodeText(IXMLDOMNode* pNode, wchar_t* text, int slen)
{
  BSTR bstrText = NULL;

  if (!pNode || !text)
    return S_OK;

  text[0] = 0;
  if (SUCCEEDED(pNode->get_text(&bstrText)))
    wcsncpy(text, bstrText, slen - 1);

  return S_OK;
}

HRESULT XMLGetNodeValue(IXMLDOMNode* pNode, wchar_t* text, int slen)
{
  VARIANT val, varRet;

  VariantInit(&val);
  VariantInit(&varRet);

  if (SUCCEEDED(pNode->get_nodeValue(&val)))
  {
    if (SUCCEEDED(VariantChangeType(&varRet, &val, 0, VT_BSTR)))
      wcsncpy(text, varRet.bstrVal, slen - 1);
  }

  VariantClear(&val);
  VariantClear(&varRet);

  return S_OK;
}

HRESULT XMLGetNodeValue(IXMLDOMNode* pNode, wchar_t** ptext)
{
  VARIANT val, varRet;

  VariantInit(&val);
  VariantInit(&varRet);

  if (SUCCEEDED(pNode->get_nodeValue(&val)))
  {
    if (SUCCEEDED(VariantChangeType(&varRet, &val, 0, VT_BSTR)))
    {
      AllocString(*ptext, wcslen(varRet.bstrVal));
      wcscpy(*ptext, varRet.bstrVal);
    }
  }

  VariantClear(&val);
  VariantClear(&varRet);

  return S_OK;
}

HRESULT XMLGetChildByName(IXMLDOMNode* pNode, wchar_t* name, IXMLDOMNode** pChild)
{
  pNode->get_firstChild(pChild);
  while (*pChild)
  {
    if (XMLNodeNameIs(*pChild, name))
      return S_OK;
    (*pChild)->get_nextSibling(pChild);
  }

  *pChild = NULL;
  return S_OK;
}
