/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：注册表访问单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: ORegistry.h,v 1.4 2006/10/04 12:16:31 Administrator Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <windows.h>

/************************************************************************/
/* 注册表操作类                                                         */
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
