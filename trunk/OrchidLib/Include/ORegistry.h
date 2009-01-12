/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ�ע�����ʵ�Ԫ
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: ORegistry.h,v 1.4 2006/10/04 12:16:31 Administrator Exp $
* ��    ע��
*******************************************************************************/

#pragma once

#include <windows.h>

/************************************************************************/
/* ע��������                                                         */
/************************************************************************/

class CHomeRegistry
{
protected:
  wchar_t* m_regbase;

public:
  CHomeRegistry(wchar_t* regbase);
  virtual ~CHomeRegistry();

  bool ReadInt(wchar_t* path, wchar_t* name, int &value);
  bool WriteInt(wchar_t* path, wchar_t* name, int value);

  bool ReadBool(wchar_t* path, wchar_t* name, bool &value);
  bool WriteBool(wchar_t* path, wchar_t* name, bool value);

  bool ReadString(wchar_t* path, wchar_t* name, wchar_t* &value);
  bool ReadString(wchar_t* path, wchar_t* name, wchar_t* value, int size);
  bool WriteString(wchar_t* path, wchar_t* name, wchar_t* value);

  bool ReadBlock(wchar_t* path, wchar_t* name, void* value, int size);
  bool WriteBlock(wchar_t* path, wchar_t* name, void* value, int size);

protected:
  bool OpenKey(wchar_t* path, wchar_t* name, HKEY &hkey);
};
