/*******************************************************************************
* ������ƣ�Orchid Mobile Software Public Library
* �ļ����ƣ�ע�����ʵ�Ԫ
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OSysDir.h,v 1.1 2006/10/03 04:16:13 Administrator Exp $
* ��    ע��
*******************************************************************************/

#pragma once

#include <windows.h>
#include "OList.h"

/************************************************************************/
/* ϵͳ·��������                                                       */
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
