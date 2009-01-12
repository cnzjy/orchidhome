/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：公共类文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OClasses.h,v 1.2 2006/09/24 08:44:16 Administrator Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <windows.h>
#include <ocidl.h>

#include "OTypes.h"
#include "OList.h"
#include "OUtils.h"

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
/* 颜色类                                                               */
/************************************************************************/

class CHomeColor : public CPersistent
{
public:
  CHomeColor();
  ~CHomeColor();

  wchar_t* name;
  COLORREF color;

  void SetName(wchar_t* colorname);
  HRESULT LoadFromStream(IStream* pStm);
  HRESULT SaveToStream(IStream* pStm);
  void Clone(CHomeColor* source);
};

// 取得颜色对象总数
int GetHomeColorCount();

// 从列表中取颜色对象
CHomeColor* GetHomeColorItem(int Index);
