/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：注册表访问单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OSysDir.h,v 1.1 2006/10/03 04:16:13 Administrator Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <windows.h>
#include "OList.h"

/************************************************************************/
/* 系统路径处理类                                                       */
/************************************************************************/

class CSysDir
{
protected:
  CList* m_names;
  CList* m_dirs;

  void DoAddSysDir(TCHAR* name, TCHAR* dir);
  void AddSysDir(int nFolder, TCHAR* name);
  void InitSysDir();

public:
  CSysDir();
  ~CSysDir();

  void EncodeSysDir(TCHAR* &dir);
  void EncodeSysDir(TCHAR* dir, int size);

  void DecodeSysDir(TCHAR* &dir);
  void DecodeSysDir(TCHAR* dir, int size);

  int Count();
  TCHAR* Names(int Index);
  TCHAR* Dirs(int Index);
};
