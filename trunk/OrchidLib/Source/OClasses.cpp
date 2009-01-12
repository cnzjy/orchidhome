/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：公共类文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OClasses.cpp,v 1.2 2006/09/24 08:44:16 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "OClasses.h"

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
/* 颜色类                                                               */
/************************************************************************/

CList* m_HomeColors = NULL;

int GetHomeColorCount()
{
  if (m_HomeColors)
    return m_HomeColors->GetCount();
  else
    return 0;
}

CHomeColor* GetHomeColorItem(int Index)
{
  if (m_HomeColors)
    return (CHomeColor*)m_HomeColors->Get(Index);
  else
    return NULL;
}

CHomeColor::CHomeColor()
{
  if (!m_HomeColors)
    m_HomeColors = new CList;
  m_HomeColors->Add(this);

  InitString(name);
  color = COLOR_TRANSPARENT;
}

CHomeColor::~CHomeColor()
{
  FreeString(name);

  if (m_HomeColors)
    m_HomeColors->Remove(this);
  if (m_HomeColors->GetCount() == 0)
  {
    delete m_HomeColors;
    m_HomeColors = NULL;
  }
}

void CHomeColor::SetName(wchar_t* colorname)
{
  CloneString(name, colorname);
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
