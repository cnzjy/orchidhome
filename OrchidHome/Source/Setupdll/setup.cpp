/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：插件安装DLL
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: setup.cpp,v 1.4 2006/05/10 11:30:28 zjy Exp $
* 备    注：
*******************************************************************************/

#include <windows.h>
#include <shellapi.h>
#include <aygshell.h>
#include <cfgmgrapi.h>
#include <ce_setup.h>

#define SETUP_API extern "C"

#define LENOF(a) sizeof(a) / sizeof(a[0])

HINSTANCE hInst = 0;
bool g_PrevInstalled = false;

// OS 值
#define OS_SP2002    0
#define OS_SP2003    1
#define OS_WM5       2

// OS 字符串
wchar_t* OSStr[3] = {L"2003", L"2003", L"WM5"};

// 屏幕大小
#define SCREEN_NORMAL  0
#define SCREEN_QVGA    1

// 屏幕字符串
wchar_t* ScreenStr[2] = {L"Normal", L"QVGA"};

// 主题文件名
wchar_t* NameStr[] = {L"Orchid.home.xml", L"Orchid_full.home.xml"};

BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
  if (!hInst)
    hInst = (HINSTANCE)hinstDLL;
  return TRUE;
}

SETUP_API codeINSTALL_INIT Install_Init(HWND hwndParent, BOOL fFirstCall, BOOL fPreviouslyInstalled,
                                        LPCTSTR pszInstallDir)
{
  g_PrevInstalled = fPreviouslyInstalled ? true : false;
  return codeINSTALL_INIT_CONTINUE;
}

// 文件复制完成后根据操作系统复制主题文件到主题目录
SETUP_API codeINSTALL_EXIT Install_Exit(HWND hwndParent, LPCTSTR pszInstallDir, WORD cFailedDirs,
                                        WORD cFailedFiles, WORD cFailedRegKeys, WORD cFailedRegVals,
                                        WORD cFailedShortcuts)
{
  OSVERSIONINFO osver;
  int os = OS_SP2003;
  int scr = SCREEN_NORMAL;

  // 取版本号
  if (GetVersionEx(&osver))
  {
    if (osver.dwMajorVersion == 3)
      os = OS_SP2002;
    else if (osver.dwMajorVersion == 4)
      os = OS_SP2003;
    else if (osver.dwMajorVersion == 5)
      os = OS_WM5;
  }

  // 取屏幕大小
  int xsize = GetSystemMetrics(SM_CXSCREEN);
  if (xsize == 176)
    scr = SCREEN_NORMAL;
  else if (xsize == 240)
    scr = SCREEN_QVGA;

  // 主题前缀名
  wchar_t sPrefix[32];
  wsprintf(sPrefix, L"%s_%s_", OSStr[os], ScreenStr[scr]);

  // 主题路径
  wchar_t sHomePath[MAX_PATH];
  SHGetSpecialFolderPath(0, sHomePath, CSIDL_APPDATA, false);
  wcscat(sHomePath, L"\\Home\\");

  // 复制匹配的主题文件到主题目录
  for (int i = 0; i < LENOF(NameStr); i++)
  {
    wchar_t src[MAX_PATH], dst[MAX_PATH];
    wsprintf(src, L"%s\\Theme\\%s%s", pszInstallDir, sPrefix, NameStr[i]);
    wsprintf(dst, L"%s%s", sHomePath, NameStr[i]);
    CopyFile(src, dst, false);
  }

  // 删除临时的主题文件目录
  SHFILEOPSTRUCT shop;
  wchar_t sz[MAX_PATH];
  memset(sz, 0, MAX_PATH);
  wcscpy(sz, pszInstallDir);
  wcscat(sz, L"\\Theme");
  memset(&shop, 0, sizeof(shop));
  shop.hwnd = hwndParent;
  shop.wFunc = FO_DELETE;
  shop.pFrom = sz;
  shop.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
  SHFileOperation(&shop);

  // 删除多余的文件
  wsprintf(sz, L"%s\\Setup.dll", pszInstallDir);
  DeleteFile(sz);

  return codeINSTALL_EXIT_DONE;
}

SETUP_API codeUNINSTALL_INIT Uninstall_Init(HWND hwndParent, LPCTSTR pszInstallDir)
{
  return codeUNINSTALL_INIT_CONTINUE;
}

SETUP_API codeUNINSTALL_EXIT Uninstall_Exit(HWND hwndParent)
{
  // 主题路径
  wchar_t sHomePath[MAX_PATH];
  SHGetSpecialFolderPath(0, sHomePath, CSIDL_APPDATA, false);
  wcscat(sHomePath, L"\\Home\\");

  // 删除主题文件
  for (int i = 0; i < LENOF(NameStr); i++)
  {
    wchar_t fn[MAX_PATH];
    wsprintf(fn, L"%s%s", sHomePath, NameStr[i]);
    DeleteFile(fn);
  }
  return codeUNINSTALL_EXIT_DONE;
}
