/*******************************************************************************
* ������ƣ�Orchid Mobile Software Home Plugin
* �ļ����ƣ���������ͷ�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OUtils.h,v 1.39 2008/02/28 12:54:22 Administrator Exp $
* ��    ע��
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

// �洢����Ϣ�ṹ
struct STORAGE_INFO 
{
  int load;
  int total;
  int free;
};

// ���������Ա���ĺ�
#define LENOF(a) sizeof(a) / sizeof(a[0])

/*******************************************************************************
* ��̬�ַ���������
*******************************************************************************/

// ��ʼ����̬�ַ���
bool InitString(wchar_t* &s);

// Ϊ��̬�ַ������䳤�ȿռ�
bool AllocString(wchar_t* &s, int len);

// �ͷŶ�̬�ַ���
void FreeString(wchar_t* &s);

// ���ƶ�̬�ַ���
void CloneString(wchar_t* &dst, wchar_t* src);

/*******************************************************************************
* �ַ���������
*******************************************************************************/

// Unicode ת ASCII
int WCharToChar(wchar_t* ws, char* buf, int buflen);

// ASCII ת Unicode
int CharToWChar(char* cs, wchar_t* buf, int buflen);

// �ж������ַ����Ƿ���ȣ������ִ�Сд
bool IsSameText(wchar_t* str1, wchar_t* str2);

// Unix ���·��ת Windows ���
void PathUnixToWin(wchar_t* path);

// ɾ���ļ���չ��
void RemoveFileExt(wchar_t* filename);

// Ϊ���·���ļ�������Ĭ��·��
void AddDefaultPath(wchar_t* &filename, wchar_t* path);

// ����Դ�м����ַ���
void LoadResString(HINSTANCE hInst, UINT uid, wchar_t* &str);

// ����������ȡ���ļ���
void ExtractFileNameFromCmdLine(wchar_t* cmd, wchar_t* &filename);

/*******************************************************************************
* ��ѧ����
*******************************************************************************/

// ��������ȡ��
int round(double v);

// �Ƚ�������������� v1 > v2 ���� 1��v2 < v2 ���� -1��v2 = v2 ���� 0��
int CompareValue(int v1, int v2);

// �Ƚ������ַ�������� s1 > s2 ���� 1��s2 < s2 ���� -1��s2 = s2 ���� 0��
int OU_CompareString(TCHAR * s1, TCHAR * s2, bool IgnoreCase = true);

/*******************************************************************************
* ͼ������
*******************************************************************************/

// ��ָ����ɫ�����Σ�������͸��ɫ
void FillRectColor(HDC hdc, RECT* rect, COLORREF color);

// ��ָ����ɫ�����Σ�������͸��ɫ
void DrawRectColor(HDC hdc, RECT* rect, COLORREF color);

/*******************************************************************************
* �ļ�������
*******************************************************************************/

// �ж��ļ��Ƿ����
bool FileExists(wchar_t* filename);

// �ж�Ŀ¼�Ƿ����
bool DirectoryExists(wchar_t* dir);

// Ϊ�ļ����������ڵĶ༶Ŀ¼
bool CreateDirectoriesForFile(wchar_t* filename);

// �ж��ļ��Ƿ�����ʹ��
bool IsFileInUse(wchar_t* filename);

// �����ļ�дʱ��
bool GetFileWriteTime(wchar_t* filename, FILETIME &lpLastWriteTime);

// �����ļ���С
int OU_GetFileSize(wchar_t* filename);

// ���� Shell ִ��һ���ļ�
void ExecuteFile(wchar_t* target, wchar_t* args);

// ���ļ���װ��ͼ��
HICON LoadIconFromFile(wchar_t* iconfile, int iconindex, int iconsize);

// ע�� COM DLL
bool DoRegDll(TCHAR* filename, bool isReg);

// �ж� COM DLL �Ƿ��ע��
bool DllCanReg(TCHAR* filename);

/*******************************************************************************
* �Ի���ؼ�������
*******************************************************************************/

#define DI_RIDEBACK     1  // ������˼�
#define DI_SCALE        2  // �Զ����ſؼ�
#define DI_SETFONT      4  // ���ÿؼ�����
#define DI_TEXTBOLD     8  // �ı������Զ��Ӵ�
#define DI_LANDSCALE    16 // �����ֻ���Ȳ��Զ��Ŵ�

#define DT_TEXT_NO_FOLD 0x8000 // ���Զ��Ӵֵ��ı��ؼ� ID

// ��ʼ���Ի���
void InitializeDialog(HINSTANCE hInst, HWND hwnd, UINT idCtrl, UINT menuid, bool rideback);
void InitializeDialogEx(HINSTANCE hInst, HWND hwnd, UINT idCtrl, UINT menuid, UINT flag);

// Ϊ�б�򴴽� Spin ��ť�ؼ�
HWND CreateSpinBoxForListBox(HINSTANCE hInst, HWND hParent, HWND hList);

// Ϊ�б���ʼ�� Spin ��ť��ʹ�� idstart �� idend ����Դ�ַ�����ʼ���б�numstart �� numend Ϊ���ӵ������
// ��ʼ����ɺ����õ�ǰѡ����Ϊ idx
HWND InitSpinBox(HINSTANCE hInst, HWND hwnd, UINT idctrl, UINT idstart, UINT idend, int idx);
HWND InitSpinBoxEx(HINSTANCE hInst, HWND hwnd, UINT idctrl, UINT idstart, UINT idend, int numstart, int numend, int idx);

// ȡ���б�ǰ��
int GetListViewItemIndex(HWND hwnd);

// �����б�ǰ��
void SetListViewItemIndex(HWND hwnd, int index);

// ȡ���б�ǰ��
int GetListBoxItemIndex(HWND hwnd);

// �����б�ǰ��
void SetListBoxItemIndex(HWND hwnd, int index);

// ������Դ�ַ�����ʾ�Ի���
int ShowMsgBoxRes(HINSTANCE hInst, HWND hwnd, UINT idTitle, UINT idText, UINT uType);

// ���ÿؼ��ɼ���
void SetControlVisible(HWND hwnd, int id, bool visible);

// ���ÿؼ�������
void SetControlEnabled(HWND hwnd, int id, bool enabled);

// Ϊ Edit �ؼ������ַ���
void SetStrValueToEdit(HWND hwnd, int id, wchar_t* s);

// Ϊ Edit �ؼ���������
void SetIntValueToEdit(HWND hwnd, int id, int v);

// �� Edit �ؼ���ȡ���ַ���
void GetStrValueFromEdit(HWND hwnd, int id, wchar_t* &s);
void GetStrValueFromEdit(HWND hwnd, int id, wchar_t* buf, int buflen);

// �� Edit �ؼ���ȡ����ֵ
int GetIntValueFromEdit(HWND hwnd, int id, int def = -1);

// ȡ����״̬
bool GetCheckBoxChecked(HWND hwnd, int id);

// ���ü���״̬
void SetCheckBoxChecked(HWND hwnd, int id, bool checked);

// ���ҶԻ�������Ĳ˵�
HMENU GetDlgMenuBar(HWND hwnd, UINT idmenu = 0);

/*******************************************************************************
* ϵͳ��غ���
*******************************************************************************/

// ȡ�ô洢��Ϣ
bool GetStorageInfo(wchar_t* dir, STORAGE_INFO* info);

// ȡ����ϵͳ�汾
int GetOSVer();

// ȡ��Ļ���
int GetCXScreen();

// ȡ�Ƿ� QVGA
bool IsQVGA();

// ȡ�Ƿ�����ֻ�
bool IsLandScape();

// ȡ�Ƿ�����ϵͳ
bool IsChinese();

// ȡ��׼�Ի�������
HFONT GetStdDlgFont(bool bold = false);

// ����
void RestartWindows();

// �ػ�
void ShutdownWindows();

// ��ʾ�ȴ����
void ShowWaitCursor();

// ���صȴ����
void HideWaitCursor();

// ȡ��ϵͳʱ�䣬��1999-12-31����ǰ������
int GetSecondStamp(SYSTEMTIME* time);
int GetSecondStamp();
