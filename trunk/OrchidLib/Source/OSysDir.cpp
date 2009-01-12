/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：注册表访问单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OSysDir.cpp,v 1.1 2006/10/03 04:16:13 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "OSysDir.h"
#include "OUtils.h"

/************************************************************************/
/* 系统路径处理类                                                       */
/************************************************************************/

static int DirMatchLen(TCHAR* src, TCHAR* sub)
{
  UINT i;

  // 检查是否匹配
  for (i = 0; i < wcslen(sub); i++)
  {
    if (tolower(src[i]) != tolower(sub[i]))
      return 0;
  }

  // 后面必须结束或者为 \ 符号
  if (src[i] == 0 || src[i] == '\\')
    return wcslen(sub);
  else
    return 0;
}

CSysDir::CSysDir()
{
  m_names = new CList();
  m_dirs = new CList();
  InitSysDir();
}

CSysDir::~CSysDir()
{
  TCHAR* s;
  for (int i = 0; i < m_names->GetCount(); i++)
  {
    s = (TCHAR*)m_names->Get(i);
    FreeString(s);
    s = (TCHAR*)m_dirs->Get(i);
    FreeString(s);
  }
  delete m_names;
  delete m_dirs;
}

void CSysDir::DoAddSysDir(TCHAR* name, TCHAR* dir)
{
  TCHAR* s1 = NULL, *s2 = NULL;
  CloneString(s1, name);
  CloneString(s2, dir);
  m_names->Add(s1);
  m_dirs->Add(s2);
}

void CSysDir::AddSysDir(int nFolder, TCHAR* name)
{
  TCHAR buff[MAX_PATH];
  memset(buff, 0, sizeof(buff));
  if (SHGetSpecialFolderPath(NULL, buff, nFolder, FALSE) && buff[0])
  {
    DoAddSysDir(name, buff);
  }
}

void CSysDir::InitSysDir()
{
  AddSysDir(CSIDL_APPDATA, L"%CSIDL_APPDATA%");
  AddSysDir(CSIDL_DESKTOP, L"%CSIDL_DESKTOP%");
  AddSysDir(CSIDL_FAVORITES, L"%CSIDL_FAVORITES%");
  AddSysDir(CSIDL_FONTS, L"%CSIDL_FONTS%");
  AddSysDir(CSIDL_MYMUSIC, L"%CSIDL_MYMUSIC%");
  AddSysDir(CSIDL_PERSONAL, L"%CSIDL_PERSONAL%");
  AddSysDir(CSIDL_PROGRAMS, L"%CSIDL_PROGRAMS%");
  AddSysDir(CSIDL_PROGRAM_FILES, L"%CSIDL_PROGRAM_FILES%");
  AddSysDir(CSIDL_RECENT, L"%CSIDL_RECENT%");
  AddSysDir(CSIDL_SENDTO, L"%CSIDL_SENDTO%");
  AddSysDir(CSIDL_STARTMENU, L"%CSIDL_STARTMENU%");
  AddSysDir(CSIDL_STARTUP, L"%CSIDL_STARTUP%");
  AddSysDir(CSIDL_TEMPLATES, L"%CSIDL_TEMPLATES%");
  AddSysDir(CSIDL_WINDOWS, L"%CSIDL_WINDOWS%");
}

void CSysDir::EncodeSysDir(TCHAR* &dir)
{
  TCHAR buff[MAX_PATH];
  wcscpy(buff, dir);
  EncodeSysDir(buff, MAX_PATH);
  CloneString(dir, buff);
}

void CSysDir::EncodeSysDir(TCHAR* dir, int size)
{
  TCHAR buff[MAX_PATH];
  int maxlen = 0, idx = -1;
  wcscpy(buff, dir);
  PathUnixToWin(buff);

  // 查找匹配长度最大的项目
  for (int i = 0; i < Count(); i++)
  {
    int len = DirMatchLen(buff, Dirs(i));
    if (len > maxlen)
    {
      maxlen = len;
      idx = i;
    }
  }

  if (maxlen && idx >= 0)
  {
    wcscpy(buff, Names(idx));
    wcscat(buff, &dir[maxlen]);
    wcsncpy(dir, buff, size - 1);
  }
}

void CSysDir::DecodeSysDir(TCHAR* &dir)
{
  TCHAR buff[MAX_PATH];
  wcscpy(buff, dir);
  DecodeSysDir(buff, MAX_PATH);
  CloneString(dir, buff);
}

void CSysDir::DecodeSysDir(TCHAR* dir, int size)
{
  TCHAR buff[MAX_PATH];
  wcscpy(buff, dir);
  PathUnixToWin(buff);
  for (int i = 0; i < Count(); i++)
  {
    int len = DirMatchLen(buff, Names(i));
    if (len)
    {
      wcscpy(buff, Dirs(i));
      wcscat(buff, &dir[len]);
      wcsncpy(dir, buff, size - 1);
      return;
    }
  }
}

int CSysDir::Count()
{
  return m_names->GetCount();
}

TCHAR* CSysDir::Names(int Index)
{
  return (TCHAR*)m_names->Get(Index);
}

TCHAR* CSysDir::Dirs(int Index)
{
  return (TCHAR*)m_dirs->Get(Index);
}
