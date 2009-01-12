/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：注册表访问单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: ORegistry.cpp,v 1.6 2008/09/12 00:40:39 JingYu Exp $
* 备    注：
*******************************************************************************/

#include "ORegistry.h"
#include "OUtils.h"

/************************************************************************/
/* 注册表操作类                                                         */
/************************************************************************/

CHomeRegistry::CHomeRegistry(wchar_t* regbase)
{
  m_regbase = NULL;
  CloneString(m_regbase, regbase);
}

CHomeRegistry::~CHomeRegistry()
{
  FreeString(m_regbase);
}

bool CHomeRegistry::OpenKey(wchar_t* path, wchar_t* name, HKEY &hkey)
{
  wchar_t buff[MAX_PATH];
  DWORD disp;
  wsprintf(buff, m_regbase);
  wcscat(buff, path);
  return (RegCreateKeyEx(HKEY_CURRENT_USER, buff, 0, NULL, 0, NULL, NULL, &hkey, &disp)) == ERROR_SUCCESS;
}

bool CHomeRegistry::ReadInt(wchar_t* path, wchar_t* name, int &value)
{
  HKEY hkey;
  bool ret = false;
  if (OpenKey(path, name, hkey))
  {
    DWORD type, dw, dwsize;
    dwsize = sizeof(DWORD);
    if (RegQueryValueEx(hkey, name, NULL, &type, (LPBYTE)&dw, &dwsize) == ERROR_SUCCESS)
    {
      if (type == REG_DWORD)
      {
        value = (int)dw;
        ret = true;
      }
    }
    RegCloseKey(hkey);
  }

  return ret;
}

bool CHomeRegistry::WriteInt(wchar_t* path, wchar_t* name, int value)
{
  HKEY hkey;
  bool ret = false;
  if (OpenKey(path, name, hkey))
  {
    DWORD dw = (DWORD)value;
    if (RegSetValueEx(hkey, name, NULL, REG_DWORD, (LPBYTE)&dw, sizeof(DWORD)) == ERROR_SUCCESS)
    {
      ret = true;
    }
    RegCloseKey(hkey);
  }

  return ret;
}

bool CHomeRegistry::ReadBool(wchar_t* path, wchar_t* name, bool &value)
{
  int dw;
  if (ReadInt(path, name, dw))
  {
    value = dw ? true : false;
    return true;
  }

  return false;
}

bool CHomeRegistry::WriteBool(wchar_t* path, wchar_t* name, bool value)
{
  return WriteInt(path, name, value);
}

bool CHomeRegistry::ReadString(wchar_t* path, wchar_t* name, wchar_t* value, int size)
{
  wchar_t* buff = NULL;
  bool ret = ReadString(path, name, buff);
  if (ret && buff && buff[0])
    wcsncpy(value, buff, size - 1);
  else
    value[0] = 0;
  FreeString(buff);
  return ret;
}

bool CHomeRegistry::ReadString(wchar_t* path, wchar_t* name, wchar_t* &value)
{
  HKEY hkey;
  bool ret = false;
  if (OpenKey(path, name, hkey))
  {
    DWORD type, dwsize;
    if (RegQueryValueEx(hkey, name, NULL, &type, (LPBYTE)NULL, &dwsize) == ERROR_SUCCESS)
    {
      if (type == REG_SZ && dwsize > 0)
      {
        AllocString(value, dwsize / sizeof(wchar_t));
        if (RegQueryValueEx(hkey, name, NULL, NULL, (LPBYTE)value, &dwsize) == ERROR_SUCCESS)
          ret = true;
      }
    }
    RegCloseKey(hkey);
  }

  return ret;
}

bool CHomeRegistry::WriteString(wchar_t* path, wchar_t* name, wchar_t* value)
{
  HKEY hkey;
  bool ret = false;
  if (!value)
    value = L"";
  if (OpenKey(path, name, hkey))
  {
    if (RegSetValueEx(hkey, name, NULL, REG_SZ, (LPBYTE)value, wcslen(value) * sizeof(wchar_t)) == ERROR_SUCCESS)
    {
      ret = true;
    }
    RegCloseKey(hkey);
  }

  return ret;
}

bool CHomeRegistry::ReadBlock(wchar_t* path, wchar_t* name, void* value, int size)
{
  HKEY hkey;
  bool ret = false;
  if (OpenKey(path, name, hkey))
  {
    DWORD type, dwsize;
    if (RegQueryValueEx(hkey, name, NULL, &type, (LPBYTE)NULL, &dwsize) == ERROR_SUCCESS)
    {
      if (type == REG_BINARY && dwsize == size)
      {
        if (RegQueryValueEx(hkey, name, NULL, NULL, (LPBYTE)value, &dwsize) == ERROR_SUCCESS)
          ret = true;
      }
    }
    RegCloseKey(hkey);
  }

  return ret;
}

bool CHomeRegistry::WriteBlock(wchar_t* path, wchar_t* name, void* value, int size)
{
  HKEY hkey;
  bool ret = false;
  if (OpenKey(path, name, hkey))
  {
    if (RegSetValueEx(hkey, name, NULL, REG_BINARY, (LPBYTE)value, size) == ERROR_SUCCESS)
    {
      ret = true;
    }
    RegCloseKey(hkey);
  }

  return ret;
}
