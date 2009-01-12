/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：公共函数头文件
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OUtils.h,v 1.39 2008/02/28 12:54:22 Administrator Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <windows.h>
#include <aygshell.h>
#include <memory.h>

#define COLOR_TRANSPARENT 0xFFFFFFFF
#define COLOR_BLACK       0x000000
#define COLOR_MAROON      0x000080
#define COLOR_GREEN       0x008000
#define COLOR_OLIVE       0x008080
#define COLOR_NAVY        0x800000
#define COLOR_PURPLE      0x800080
#define COLOR_TEAL        0x808000
#define COLOR_GRAY        0x808080
#define COLOR_SILVER      0xC0C0C0
#define COLOR_RED         0x0000FF
#define COLOR_LIME        0x00FF00
#define COLOR_YELLOW      0x00FFFF
#define COLOR_BLUE        0xFF0000
#define COLOR_FUCHSIA     0xFF00FF
#define COLOR_AQUA        0xFFFF00
#define COLOR_LTGRAY      0xC0C0C0
#define COLOR_DKGRAY      0x808080
#define COLOR_WHITE       0xFFFFFF

#define OS_SP2002    0
#define OS_SP2003    1
#define OS_WM5       2
#define OS_WM6       3

// 存储器信息结构
struct STORAGE_INFO 
{
  int load;
  int total;
  int free;
};

// 计算数组成员数的宏
#define LENOF(a) sizeof(a) / sizeof(a[0])

/*******************************************************************************
* 动态字符串处理函数
*******************************************************************************/

// 初始化动态字符串
bool InitString(wchar_t* &s);

// 为动态字符串分配长度空间
bool AllocString(wchar_t* &s, int len);

// 释放动态字符串
void FreeString(wchar_t* &s);

// 复制动态字符串
void CloneString(wchar_t* &dst, wchar_t* src);

/*******************************************************************************
* 字符串处理函数
*******************************************************************************/

// Unicode 转 ASCII
int WCharToChar(wchar_t* ws, char* buf, int buflen);

// ASCII 转 Unicode
int CharToWChar(char* cs, wchar_t* buf, int buflen);

// 判断两个字符串是否相等，不区分大小写
bool IsSameText(wchar_t* str1, wchar_t* str2);

// Unix 风格路径转 Windows 风格
void PathUnixToWin(wchar_t* path);

// 删除文件扩展名
void RemoveFileExt(wchar_t* filename);

// 为相对路径文件名增加默认路径
void AddDefaultPath(wchar_t* &filename, wchar_t* path);

// 从资源中加载字符串
void LoadResString(HINSTANCE hInst, UINT uid, wchar_t* &str);

// 从命令行中取出文件名
void ExtractFileNameFromCmdLine(wchar_t* cmd, wchar_t* &filename);

/*******************************************************************************
* 数学函数
*******************************************************************************/

// 四舍五入取整
int round(double v);

// 比较两个整数，如果 v1 > v2 返回 1，v2 < v2 返回 -1，v2 = v2 返回 0。
int CompareValue(int v1, int v2);

// 比较两个字符串，如果 s1 > s2 返回 1，s2 < s2 返回 -1，s2 = s2 返回 0。
int OU_CompareString(TCHAR * s1, TCHAR * s2, bool IgnoreCase = true);

/*******************************************************************************
* 图像处理函数
*******************************************************************************/

// 用指定颜色填充矩形，可以是透明色
void FillRectColor(HDC hdc, RECT* rect, COLORREF color);

// 用指定颜色画矩形，可以是透明色
void DrawRectColor(HDC hdc, RECT* rect, COLORREF color);

/*******************************************************************************
* 文件处理函数
*******************************************************************************/

// 判断文件是否存在
bool FileExists(wchar_t* filename);

// 判断目录是否存在
bool DirectoryExists(wchar_t* dir);

// 为文件创建其所在的多级目录
bool CreateDirectoriesForFile(wchar_t* filename);

// 判断文件是否正在使用
bool IsFileInUse(wchar_t* filename);

// 返回文件写时间
bool GetFileWriteTime(wchar_t* filename, FILETIME &lpLastWriteTime);

// 返回文件大小
int OU_GetFileSize(wchar_t* filename);

// 调用 Shell 执行一个文件
void ExecuteFile(wchar_t* target, wchar_t* args);

// 从文件中装载图标
HICON LoadIconFromFile(wchar_t* iconfile, int iconindex, int iconsize);

// 注册 COM DLL
bool DoRegDll(TCHAR* filename, bool isReg);

// 判断 COM DLL 是否可注册
bool DllCanReg(TCHAR* filename);

/*******************************************************************************
* 对话框控件处理函数
*******************************************************************************/

#define DI_RIDEBACK     1  // 处理后退键
#define DI_SCALE        2  // 自动缩放控件
#define DI_SETFONT      4  // 设置控件字体
#define DI_TEXTBOLD     8  // 文本字体自动加粗
#define DI_LANDSCALE    16 // 横屏手机宽度不自动放大

#define DT_TEXT_NO_FOLD 0x8000 // 不自动加粗的文本控件 ID

// 初始化对话框
void InitializeDialog(HINSTANCE hInst, HWND hwnd, UINT idCtrl, UINT menuid, bool rideback);
void InitializeDialogEx(HINSTANCE hInst, HWND hwnd, UINT idCtrl, UINT menuid, UINT flag);

// 为列表框创建 Spin 按钮控件
HWND CreateSpinBoxForListBox(HINSTANCE hInst, HWND hParent, HWND hList);

// 为列表框初始化 Spin 按钮。使用 idstart 到 idend 的资源字符串初始化列表，numstart 到 numend 为附加的数字项。
// 初始化完成后设置当前选择项为 idx
HWND InitSpinBox(HINSTANCE hInst, HWND hwnd, UINT idctrl, UINT idstart, UINT idend, int idx);
HWND InitSpinBoxEx(HINSTANCE hInst, HWND hwnd, UINT idctrl, UINT idstart, UINT idend, int numstart, int numend, int idx);

// 取得列表当前项
int GetListViewItemIndex(HWND hwnd);

// 设置列表当前项
void SetListViewItemIndex(HWND hwnd, int index);

// 取得列表当前项
int GetListBoxItemIndex(HWND hwnd);

// 设置列表当前项
void SetListBoxItemIndex(HWND hwnd, int index);

// 根据资源字符串显示对话框
int ShowMsgBoxRes(HINSTANCE hInst, HWND hwnd, UINT idTitle, UINT idText, UINT uType);

// 设置控件可见性
void SetControlVisible(HWND hwnd, int id, bool visible);

// 设置控件可用性
void SetControlEnabled(HWND hwnd, int id, bool enabled);

// 为 Edit 控件设置字符串
void SetStrValueToEdit(HWND hwnd, int id, wchar_t* s);

// 为 Edit 控件设置整数
void SetIntValueToEdit(HWND hwnd, int id, int v);

// 从 Edit 控件中取得字符串
void GetStrValueFromEdit(HWND hwnd, int id, wchar_t* &s);
void GetStrValueFromEdit(HWND hwnd, int id, wchar_t* buf, int buflen);

// 从 Edit 控件中取得数值
int GetIntValueFromEdit(HWND hwnd, int id, int def = -1);

// 取检查框状态
bool GetCheckBoxChecked(HWND hwnd, int id);

// 设置检查框状态
void SetCheckBoxChecked(HWND hwnd, int id, bool checked);

// 查找对话框关联的菜单
HMENU GetDlgMenuBar(HWND hwnd, UINT idmenu = 0);

/*******************************************************************************
* 系统相关函数
*******************************************************************************/

// 取得存储信息
bool GetStorageInfo(wchar_t* dir, STORAGE_INFO* info);

// 取操作系统版本
int GetOSVer();

// 取屏幕宽度
int GetCXScreen();

// 取是否 QVGA
bool IsQVGA();

// 取是否横屏手机
bool IsLandScape();

// 取是否中文系统
bool IsChinese();

// 取标准对话框字体
HFONT GetStdDlgFont(bool bold = false);

// 重启
void RestartWindows();

// 关机
void ShutdownWindows();

// 显示等待光标
void ShowWaitCursor();

// 隐藏等待光标
void HideWaitCursor();

// 取得系统时间，从1999-12-31到当前的秒数
int GetSecondStamp(SYSTEMTIME* time);
int GetSecondStamp();
