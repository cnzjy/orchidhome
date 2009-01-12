/*******************************************************************************
* 软件名称：Orchid Mobile Software Public Library
* 文件名称：公共函数
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OUtils.cpp,v 1.46 2008/02/28 12:54:22 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "OUtils.h"

#define STR_LEN  255

static wchar_t* sOpen = L"Open";

/*******************************************************************************
* 动态字符串处理函数
*******************************************************************************/

static wchar_t* _EmptyStr = L"";

// 初始化动态字符串
bool InitString(wchar_t* &s)
{
  s = _EmptyStr;
  return true;
}

// 为动态字符串分配长度空间
bool AllocString(wchar_t* &s, int len)
{
  int size;
  if (len < 0) len = 0;
  size = (len + 1) * sizeof(wchar_t);
  s = (wchar_t*)((s && s != _EmptyStr) ? realloc(s, size) : malloc(size));
  if (s) memset(s, 0, size);
  return s != NULL;
}

// 释放动态字符串
void FreeString(wchar_t* &s)
{
  if (s && s != _EmptyStr)
    free(s);
  s = NULL;
}

// 复制动态字符串
void CloneString(wchar_t* &dst, wchar_t* src)
{
  if (src && src != _EmptyStr)
  {
    AllocString(dst, wcslen(src));
    wcscpy(dst, src);
  }
  else
  {
    FreeString(dst);
    dst = src;
  }
}

/*******************************************************************************
* 字符串处理函数
*******************************************************************************/

// Unicode 转 ASCII
int WCharToChar(wchar_t* ws, char* buf, int buflen)
{
  int len;
  len = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, ws, wcslen(ws), NULL, 0, NULL, NULL);
  if (len > buflen - 1) len = buflen - 1;
  len = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, ws, wcslen(ws), buf, len, NULL, NULL);
  buf[len] = 0;
  return len;
}

// ASCII 转 Unicode
int CharToWChar(char* cs, wchar_t* buf, int buflen)
{
  int len;
  len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cs, strlen(cs), NULL, 0);
  if (len > buflen - 1) len = buflen - 1;
  len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cs, strlen(cs), buf, buflen);
  buf[len] = 0;
  return len;
}

// 判断两个字符串是否相等，不区分大小写
bool IsSameText(wchar_t* str1, wchar_t* str2)
{
  UINT i;
  if (wcslen(str1) != wcslen(str2))
    return false;
  for (i = 0; i < wcslen(str1); i++)
  {
    if (CharUpper((LPTSTR)str1[i]) != CharUpper((LPTSTR)str2[i]))
      return false;
  }

  return true;
}

// Unix 风格路径转 Windows 风格
void PathUnixToWin(wchar_t* path)
{
  if (path)
  {
    for (uint i = 0; i < wcslen(path); i++)
    {
      if (path[i] == '/')
        path[i] = '\\';
    }
  }
}

// 删除文件扩展名
void RemoveFileExt(wchar_t* filename)
{
  if (filename)
  {
    for (int i = wcslen(filename) - 1; i >= 0; i--)
    {
      if (filename[i] == '.')
      {
        filename[i] = 0;
        return;
      }
    }
  }
}

// 无路径的程序默认为 \Windows 路径
void AddDefaultPath(wchar_t* &filename, wchar_t* path)
{
  PathUnixToWin(filename);
  if (filename[0] && filename[0] != '\\' && filename[0] != '%')
  {
    wchar_t buff[MAX_PATH];
    wcscpy(buff, path);
    PathUnixToWin(buff);
    if (buff[wcslen(path)] != '\\')
      wcscat(buff, L"\\");
    wcscat(buff, filename);
    CloneString(filename, buff);
  }
}

// 从资源中加载字符串
void LoadResString(HINSTANCE hInst, UINT uid, wchar_t* &str)
{
  wchar_t buff[1024];
  LoadString(hInst, uid, buff, sizeof(buff));
  CloneString(str, buff);
}

// 从命令行中取出文件名
void ExtractFileNameFromCmdLine(wchar_t* cmd, wchar_t* &filename)
{
  wchar_t buff[MAX_PATH];
  wcsncpy(buff, cmd, MAX_PATH - 1);
  if (buff[0] == '"')
  {
    for (int i = 1; buff[i]; i++)
    {
      if (buff[i] == '"')
      {
        if (buff[i + 1] == '"')
          i++;
        else
        {
          buff[i] = 0;
          break;
        }
      }
    }
    CloneString(filename, &buff[1]);
  }
  else
  {
    for (int i = 0; buff[i]; i++)
    {
      if (buff[i] == ' ')
      {
        buff[i] = 0;
        break;
      }
    }
    CloneString(filename, buff);
  }
}

/*******************************************************************************
* 数学函数
*******************************************************************************/

// 四舍五入取整
int round(double v)
{
  return (int)(v + 0.5);
}

// 比较两个整数，如果 v1 > v2 返回 1，v2 < v2 返回 -1，v2 = v2 返回 0。
int CompareValue(int v1, int v2)
{
  if (v1 > v2)
    return 1;
  else if (v1 < v2)
    return -1;
  else
    return 0;
}

// 比较两个字符串，如果 s1 > s2 返回 1，s2 < s2 返回 -1，s2 = s2 返回 0。
int OU_CompareString(TCHAR * s1, TCHAR * s2, bool IgnoreCase)
{
  int code = CompareString(LOCALE_SYSTEM_DEFAULT, IgnoreCase ? NORM_IGNORECASE : 0, 
    s1, wcslen(s1), s2, wcslen(s2));
  switch(code)
  {
  case CSTR_LESS_THAN:
    return -1;
  case CSTR_GREATER_THAN:
    return 1;
  default:
    return 0;
  }
}

/*******************************************************************************
* 图像处理函数
*******************************************************************************/

// 用指定颜色填充矩形，可以是透明色
void FillRectColor(HDC hdc, RECT* rect, COLORREF color)
{
  HBRUSH br;
  if (color != COLOR_TRANSPARENT)
  {
    br = CreateSolidBrush(color);
    FillRect(hdc, rect, br);
    DeleteObject(br);
  }
}

// 用指定颜色画矩形，可以是透明色
void DrawRectColor(HDC hdc, RECT* rect, COLORREF color)
{
  HPEN hpen, hsave;
  hpen = CreatePen(PS_SOLID, 0, color);
  hsave = (HPEN)SelectObject(hdc, hpen);
  MoveToEx(hdc, rect->left, rect->top, NULL);
  LineTo(hdc, rect->right, rect->top);
  LineTo(hdc, rect->right, rect->bottom);
  LineTo(hdc, rect->left, rect->bottom);
  LineTo(hdc, rect->left, rect->top);
  SelectObject(hdc, hsave);
  DeleteObject(hpen);
}

/*******************************************************************************
* 文件处理函数
*******************************************************************************/

// 判断文件是否存在
bool FileExists(wchar_t* filename)
{
  return GetFileAttributes(filename) != (DWORD)-1;
}

// 判断目录是否存在
bool DirectoryExists(wchar_t* dir)
{
  DWORD Code = GetFileAttributes(dir);
  return (Code != (DWORD)-1) && ((FILE_ATTRIBUTE_DIRECTORY & Code) != 0);
}

// 为文件创建其所在的多级目录
bool CreateDirectoriesForFile(wchar_t* filename)
{
    wchar_t buff[MAX_PATH];
    wcscpy(buff, filename);
    for (int i = wcslen(buff); ; i--)
    {
        if (buff[i] == '\\' || buff[i] == '/' || i == 0)
        {
            buff[i] = 0;
            break;
        }
    }
    if (!buff[0] || DirectoryExists(buff))
        return true;
    else
        return CreateDirectoriesForFile(buff) && CreateDirectory(buff, NULL);
}

// 判断文件是否正在使用
bool IsFileInUse(wchar_t* filename)
{
  HANDLE hFile;
  if (!FileExists(filename))
    return false;
  hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, 
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (hFile == INVALID_HANDLE_VALUE)
    return true;
  else
  {
    CloseHandle(hFile);
    return false;
  }
}

// 返回文件写时间
bool GetFileWriteTime(wchar_t* filename, FILETIME &lpLastWriteTime)
{
  WIN32_FILE_ATTRIBUTE_DATA data;
  memset(&lpLastWriteTime, 0, sizeof(FILETIME));
  if (filename && filename[0] && GetFileAttributesEx(filename, GetFileExInfoStandard, &data))
  {
    lpLastWriteTime = data.ftLastWriteTime;
    return true;
  }
  else
    return false;
}

// 返回文件大小
int OU_GetFileSize(wchar_t* filename)
{
  WIN32_FILE_ATTRIBUTE_DATA data;
  if (filename && filename[0] && GetFileAttributesEx(filename, GetFileExInfoStandard, &data))
    return data.nFileSizeLow;
  else
    return 0;
}

// 调用 Shell 执行一个文件
void ExecuteFile(wchar_t* target, wchar_t* args)
{
  SHELLEXECUTEINFO info;
  memset(&info, 0, sizeof(info));
  info.cbSize = sizeof(info);
  info.lpVerb = sOpen;
  info.lpFile = target;
  info.lpParameters = args;
  info.nShow = SW_SHOW;
  ShellExecuteEx(&info);
}

// 从文件中装载图标
HICON LoadIconFromFile(wchar_t* iconfile, int iconindex, int iconsize)
{
  HICON hIcon = NULL;
  if (iconfile[0])
  {
    HINSTANCE hlib = LoadLibrary(iconfile);
    if (hlib)
    {
      hIcon = (HICON)LoadImage(hlib, MAKEINTRESOURCE(iconindex), IMAGE_ICON, iconsize, iconsize, 0);
      FreeLibrary(hlib);
    }
  }
  return hIcon;
}

static TCHAR* sRegProc = TEXT("DllRegisterServer");
static TCHAR* sUnRegProc = TEXT("DllUnregisterServer");

// 注册 COM DLL
bool DoRegDll(TCHAR* filename, bool isReg)
{
  typedef HRESULT (WINAPI *TRegProc)(void);
  HMODULE hMod = LoadLibrary(filename);
  bool ret = false;
  if (hMod)
  {
    TRegProc proc = (TRegProc)GetProcAddress(hMod, isReg ? sRegProc : sUnRegProc);
    if (proc) ret = SUCCEEDED(proc());
    FreeLibrary(hMod);
  }
  return ret;
}

// 判断 COM DLL 是否可注册
bool DllCanReg(TCHAR* filename)
{
  HMODULE hMod = LoadLibrary(filename);
  bool ret = false;
  if (hMod)
  {
    ret = (GetProcAddress(hMod, sRegProc) != NULL) && (GetProcAddress(hMod, sUnRegProc) != NULL);
    FreeLibrary(hMod);
  }
  return ret;
}

/*******************************************************************************
* 对话框控件处理函数
*******************************************************************************/

static WNDPROC g_lpfnText = NULL;

static LRESULT CALLBACK TextControlProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (msg == WM_SETFOCUS)
    SendMessage(hwnd, EM_SETSEL, 0, -1);

  return CallWindowProc((WNDPROC)g_lpfnText, hwnd, msg, wParam, lParam);
}

// 初始化对话框
void InitializeDialog(HINSTANCE hInst, HWND hwnd, UINT idCtrl, UINT menuid, bool rideback)
{
  UINT flag = DI_SCALE | DI_SETFONT;
  if (rideback)
    flag |= DI_RIDEBACK;
  InitializeDialogEx(hInst, hwnd, idCtrl, menuid, flag);
}

void InitializeDialogEx(HINSTANCE hInst, HWND hwnd, UINT idCtrl, UINT menuid, UINT flag)
{
  RECT rc, rcwin;
  double ratex = 0, ratey = 0;
  GetWindowRect(hwnd, &rcwin);
  SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

  // 计算比例
  if (rcwin.left < rcwin.right)
  {
    ratex = ratey = (double)(rc.right - rc.left) / (double)(rcwin.right - rcwin.left);
    if (IsLandScape())
    {
      ratey = ratey * 240 / 320;
      if (flag & DI_LANDSCALE)
        ratex = ratey;
    }
  }

  // 初始化对话框
  SHINITDLGINFO shidi;
  memset(&shidi, 0, sizeof(SHINITDLGINFO));
  shidi.dwMask = SHIDIM_FLAGS;
  shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
  if (GetOSVer() >= OS_WM6) // WM6 下不会自动创建滚动条
    shidi.dwFlags |= SHIDIF_WANTSCROLLBAR;
  shidi.hDlg = hwnd;
  SHInitDialog(&shidi);

  HWND hChild = GetWindow(hwnd, GW_CHILD);
  HFONT hFont = GetStdDlgFont(false);
  HFONT hFontBold = GetStdDlgFont(true);
  bool isChinese = IsChinese();
  while (hChild)
  {
    // 子类化 Edit 控件，获得焦点时自动全选
    wchar_t sClass[256];
    GetClassName(hChild, sClass, 255);
    if (IsSameText(sClass, L"Edit"))
    {
      g_lpfnText = (WNDPROC)SetWindowLong(hChild, GWL_WNDPROC, (LONG)TextControlProc);
    }

    // 自动缩放控件
    if ((flag & DI_SCALE) && (ratex > 0))
    {
      GetWindowRect(hChild, &rcwin);
      OffsetRect(&rcwin, -rc.left, -rc.top);
      MoveWindow(hChild, (int)((double)rcwin.left * ratex + 0.95), (int)((double)rcwin.top * ratey + 0.95), 
        (int)((double)(rcwin.right-rcwin.left) * ratex + 0.95), 
        (int)((double)(rcwin.bottom-rcwin.top) * ratey + 0.95), TRUE);
    }

    // 为控件设置字体
    if (flag & DI_SETFONT)
    {
      if (!isChinese && (flag & DI_TEXTBOLD) && IsSameText(sClass, L"static") && 
        (GetDlgCtrlID(hChild) != DT_TEXT_NO_FOLD))
        SendMessage(hChild, WM_SETFONT, (WPARAM)hFontBold, FALSE);
      else
        SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, FALSE);
    }

    hChild = GetWindow(hChild, GW_HWNDNEXT);
  }

  // 将主控件最大化显示
  if (idCtrl)
  {
    HWND hCtrl = GetDlgItem(hwnd, idCtrl);
    if (hCtrl)
    {
      GetClientRect(hwnd, &rc);
      MoveWindow(hCtrl, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, TRUE);
    }
  }

  // 创建主菜单
  if (menuid)
  {
    SHMENUBARINFO shmi;
    memset(&shmi, 0, sizeof(SHMENUBARINFO));
    shmi.cbSize = sizeof(SHMENUBARINFO);
    shmi.hwndParent = hwnd;
    shmi.nToolBarId = menuid;
    shmi.hInstRes = hInst;
    //shmi.dwFlags |= SHCMBF_HMENU; WM5下不需要
    SHCreateMenuBar(&shmi);
    if (flag & DI_RIDEBACK)
      SendMessage(shmi.hwndMB, SHCMBM_OVERRIDEKEY, VK_TBACK, 
        MAKELPARAM (SHMBOF_NODEFAULT | SHMBOF_NOTIFY, SHMBOF_NODEFAULT | SHMBOF_NOTIFY));
  }
}

// 为列表框创建 Spin 按钮控件
HWND CreateSpinBoxForListBox(HINSTANCE hInst, HWND hParent, HWND hList)
{
  HWND hUpDown = CreateWindow(UPDOWN_CLASS, NULL, WS_VISIBLE | UDS_HORZ    
    | UDS_ALIGNRIGHT | UDS_ARROWKEYS |  UDS_SETBUDDYINT | UDS_WRAP | UDS_EXPANDABLE, 
    0, 0, 0, 0, hParent, 0, hInst, 0);
  // 修改 ZOrder 到列表后面以避免被其它插件挡住
  SetWindowPos(hUpDown, hList, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  SendMessage(hUpDown, UDM_SETBUDDY, (WPARAM)hList, 0);
  return hUpDown;
}

// 为列表框初始化 Spin 按钮。使用 idstart 到 idend 的资源字符串初始化列表，numstart 到 numend 为附加的数字项。
// 初始化完成后设置当前选择项为 idx
HWND InitSpinBoxEx(HINSTANCE hInst, HWND hwnd, UINT idctrl, UINT idstart, UINT idend, int numstart, int numend, int idx)
{
  HWND hCtrl = GetDlgItem(hwnd, idctrl);
  HWND hUpDown = CreateSpinBoxForListBox(hInst, hwnd, hCtrl);
  if (idstart && idend)
  {
    for (UINT i = idstart; i <= idend; i++)
    {
      wchar_t buff[STR_LEN];
      LoadString(hInst, i, buff, STR_LEN - 1);
      SendMessage(hCtrl, LB_ADDSTRING, 0, (LPARAM)buff);
    }
  }

  if (numstart && numend)
  {
    for (int i = numstart; i <= numend; i++)
    {
      wchar_t buff[STR_LEN];
      wsprintf(buff, L"%d", i);
      SendMessage(hCtrl, LB_ADDSTRING, 0, (LPARAM)buff);
    }
  }

  if (idx >= 0)
    SendMessage(hCtrl, LB_SETCURSEL, idx, 0);

  return hUpDown;
}

HWND InitSpinBox(HINSTANCE hInst, HWND hwnd, UINT idctrl, UINT idstart, UINT idend, int idx)
{
  return InitSpinBoxEx(hInst, hwnd, idctrl, idstart, idend, 0, 0, idx);
}

// 取得列表当前项
int GetListViewItemIndex(HWND hwnd)
{
  return ListView_GetNextItem(hwnd, -1, LVNI_ALL | LVNI_FOCUSED);
}

// 设置列表当前项
void SetListViewItemIndex(HWND hwnd, int index)
{
  ListView_SetItemState(hwnd, index, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
  ListView_EnsureVisible(hwnd, index, false);
}

// 取得列表当前项
int GetListBoxItemIndex(HWND hwnd)
{
  return SendMessage(hwnd, LB_GETCURSEL, 0, 0);
}

// 设置列表当前项
void SetListBoxItemIndex(HWND hwnd, int index)
{
  if (index >= 0)
    SendMessage(hwnd, LB_SETCURSEL, index, 0);
}

// 根据资源字符串显示对话框
int ShowMsgBoxRes(HINSTANCE hInst, HWND hwnd, UINT idTitle, UINT idText, UINT uType)
{
  TCHAR title[STR_LEN], text[STR_LEN];
  title[0] = text[0] = 0;
  LoadString(hInst, idTitle, title, STR_LEN);
  LoadString(hInst, idText, text, STR_LEN);
  return MessageBox(hwnd, text, title, uType);
}

// 设置控件可见性
void SetControlVisible(HWND hwnd, int id, bool visible)
{
  SetWindowPos(GetDlgItem(hwnd, id), 0, 0, 0, 0, 0, SWP_NOACTIVATE |
  SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER |
  (visible ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
}

// 设置控件可用性
void SetControlEnabled(HWND hwnd, int id, bool enabled)
{
  EnableWindow(GetDlgItem(hwnd, id), enabled);
}

// 为 Edit 控件设置字符串
void SetStrValueToEdit(HWND hwnd, int id, wchar_t* s)
{
  SendMessage(GetDlgItem(hwnd, id), WM_SETTEXT, 0, (LPARAM)s);
  SendMessage(GetDlgItem(hwnd, id), EM_SETINPUTMODE, 0, EIM_TEXT);
}

// 为 Edit 控件设置整数
void SetIntValueToEdit(HWND hwnd, int id, int v)
{
  wchar_t buff[32];
  wsprintf(buff, L"%d", v);
  SendMessage(GetDlgItem(hwnd, id), WM_SETTEXT, 0, (LPARAM)buff);
  SendMessage(GetDlgItem(hwnd, id), EM_SETINPUTMODE, 0, EIM_NUMBERS);
}

// 从 Edit 控件中取得字符串
void GetStrValueFromEdit(HWND hwnd, int id, wchar_t* &s)
{
  wchar_t buff[MAX_PATH];
  GetStrValueFromEdit(hwnd, id, buff, LENOF(buff) - 1);
  CloneString(s, buff);
}

void GetStrValueFromEdit(HWND hwnd, int id, wchar_t* buf, int buflen)
{
  SendMessage(GetDlgItem(hwnd, id), WM_GETTEXT, buflen, (LPARAM)buf);
}

// 从 Edit 控件中取得数值
int GetIntValueFromEdit(HWND hwnd, int id, int def)
{
  int ret;
  wchar_t buff[32];
  SendMessage(GetDlgItem(hwnd, id), WM_GETTEXT, LENOF(buff) - 1, (LPARAM)buff);
  if (buff[0])
  {
    ret = _wtoi(buff);
    if (ret == 0 && buff[0] != '0' && buff[1] != 0)
      ret = def;
  }
  else
    ret = def;
  return ret;
}

// 取检查框状态
bool GetCheckBoxChecked(HWND hwnd, int id)
{
  return SendMessage(GetDlgItem(hwnd, id), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

// 设置检查框状态
void SetCheckBoxChecked(HWND hwnd, int id, bool checked)
{
  SendMessage(GetDlgItem(hwnd, id), BM_SETCHECK, checked ? BST_CHECKED : BST_UNCHECKED, 0);
}

// 查找对话框关联的菜单
HMENU GetDlgMenuBar(HWND hwnd, UINT idmenu)
{
  HWND hwndMB = SHFindMenuBar (hwnd);
  if (hwndMB)
    return (HMENU)SendMessage(hwndMB, idmenu ? SHCMBM_GETSUBMENU : SHCMBM_GETMENU, 0, idmenu);
  else
    return NULL;
}

/*******************************************************************************
* 系统相关函数
*******************************************************************************/

// 取得存储信息
bool GetStorageInfo(wchar_t* dir, STORAGE_INFO* info)
{
  ULARGE_INTEGER lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;
  memset(info, 0, sizeof(STORAGE_INFO));
  if (GetDiskFreeSpaceEx(dir, &lpFreeBytesAvailableToCaller, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes))
  {
    info->free = (lpTotalNumberOfFreeBytes.HighPart << 22) | (lpTotalNumberOfFreeBytes.LowPart >> 10);
    info->total = (lpTotalNumberOfBytes.HighPart << 22) | (lpTotalNumberOfBytes.LowPart >> 10);
    if (info->total > 0)
      info->load = (int)((double)(info->total - info->free) * 100 / info->total);
    else
      info->load = 0;
    return true;
  }
  return false;
}

// 取操作系统版本
int GetOSVer()
{
  // 取版本号
  static int os = -1;
  if (os < 0)
  {
    OSVERSIONINFO osver;
    os = OS_SP2003;
    if (GetVersionEx(&osver))
    {
      if (osver.dwMajorVersion == 3)
        os = OS_SP2002;
      else if (osver.dwMajorVersion == 4)
        os = OS_SP2003;
      else if (osver.dwMajorVersion == 5)
      {
        if (osver.dwMinorVersion >= 2)
          os = OS_WM6;
        else
          os = OS_WM5;
      }
      else if (osver.dwMinorVersion > 5) // 大于 5 的按 WM6 处理
        os = OS_WM6;
    }
  }
  return os;
}

// 取屏幕宽度
int GetCXScreen()
{
  static int cx = 0;
  if (cx == 0)
  {
    cx = GetSystemMetrics(SM_CXSCREEN);
  }
  return cx;
}

// 取是否 QVGA
bool IsQVGA()
{
  int cx = GetCXScreen();
  return (cx == 240) || (cx == 320);
}

// 取是否横屏手机
bool IsLandScape()
{
  return GetCXScreen() == 320;
}

// 取是否中文系统
bool IsChinese()
{
  static LANGID lid = 0;
  if (lid == 0)
  {
    lid = GetUserDefaultUILanguage();
  }
  return (lid == 2052) || (lid == 1028);
}

// 取标准对话框字体
HFONT GetStdDlgFont(bool bold)
{
  static HFONT hFont = 0, hFontBold = 0;
  if (hFont == 0)
  {
    HFONT hSysFont = (HFONT)GetStockObject(SYSTEM_FONT);
    LOGFONT logFont, lFont;
    GetObject(hSysFont, sizeof(LOGFONT), &logFont);
    lFont = logFont;
    wcscpy(lFont.lfFaceName, (GetOSVer() >= OS_WM5 && !IsChinese()) ? L"Segoe Condensed" : L"nina");
    lFont.lfHeight = IsQVGA() ? 19 : 14;
    lFont.lfWeight = FW_DONTCARE;
    hFont = CreateFontIndirect(&lFont);
    lFont.lfWeight = FW_BOLD;
    hFontBold = CreateFontIndirect(&lFont);
  }
  return bold ? hFontBold : hFont;
}

extern "C" BOOL ExitWindowsEx(UINT uFlags, DWORD dwReason);

// 重启
void RestartWindows()
{
  ExitWindowsEx(EWX_REBOOT, 0);
}

// 关机
void ShutdownWindows()
{
  ExitWindowsEx(EWX_POWEROFF, 0);
}

void ShowWaitCursor()
{
  static HCURSOR hCursor = NULL;
  if (hCursor == NULL)
  {
    hCursor = LoadCursor(NULL, IDC_WAIT);
  }
  SetCursor(hCursor);
}

void HideWaitCursor()
{
  SetCursor(NULL);
}


// 每个月的日期数（平年和闰年）
static const int MonthDays[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

// 返回某公历是否闰年
static int GetIsLeapYear(int AYear)
{
  return (AYear % 4 == 0) && ((AYear % 100 != 0) || (AYear % 400 == 0)) ? TRUE : FALSE;
}

// 获得天数
static int EncodeDate(int AYear, int AMonth, int ADay, int* ADate)
{
  int i, leap;

  leap = GetIsLeapYear(AYear) ? 1 : 0;

  if ((AYear >= 1) && (AYear <= 9999) && (AMonth >= 1) && (AMonth <= 12) &&
    (ADay >= 1) && (ADay <= MonthDays[leap][AMonth - 1]))
  {
    /* 计算日期是这一年中的第几天 */
    for (i = 1; i < AMonth; i++)
      ADay += MonthDays[leap][i - 1];

    /* 年内日期加上从零到这一年元旦的天数 */
    i = AYear - 1;
    *ADate = i * 365 + i / 4 - i / 100 + i / 400 + ADay;

    return TRUE;
  } else {
    *ADate = 0;
    return FALSE;
  }
}

int GetSecondStamp(SYSTEMTIME* time)
{
  int days1, days2, days;
  if (EncodeDate(time->wYear, time->wMonth, time->wDay, &days1) && EncodeDate(1999, 12, 31, &days2))
  {
    days = days1 - days2;
    return ((days * 24 + time->wHour) * 60 + time->wMinute) * 60 + time->wSecond;
  }
  else
    return 0;
}

int GetSecondStamp()
{
  SYSTEMTIME time;
  GetLocalTime(&time);
  return GetSecondStamp(&time);
}