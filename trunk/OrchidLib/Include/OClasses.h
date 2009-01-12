/*******************************************************************************
* ������ƣ�Orchid Mobile Software Home Plugin
* �ļ����ƣ��������ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OClasses.h,v 1.2 2006/09/24 08:44:16 Administrator Exp $
* ��    ע��
*******************************************************************************/

#pragma once

#include <windows.h>
#include <ocidl.h>

#include "OTypes.h"
#include "OList.h"
#include "OUtils.h"

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
/* ��ɫ��                                                               */
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

// ȡ����ɫ��������
int GetHomeColorCount();

// ���б���ȡ��ɫ����
CHomeColor* GetHomeColorItem(int Index);
