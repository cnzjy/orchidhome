/*******************************************************************************
* 软件名称：Orchid 智能手机功能键定义工具
* 文件名称：文件对话框单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OFileDlg.cpp,v 1.4 2008/02/29 01:11:27 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "OFileDlg.h"

#define TITLE_LEN      64
#define IS_DIR(fd)  (((fd)->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)

static TCHAR * sParentDir = L"..";

/*******************************************************************************
* 公共处理过程
*******************************************************************************/

static WNDPROC g_lpfnListView = NULL;

// 主列表子类化消息处理函数
static LRESULT CALLBACK ListViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // 让对话框处理所有按键
    if (msg == WM_GETDLGCODE)
        return DLGC_WANTALLKEYS;
    else if (msg == WM_KEYDOWN || msg == WM_KEYUP)
    {
        if ((wParam >= '0' && wParam <= '9') || wParam == VK_F8 || wParam == VK_F9 || 
            wParam == VK_ESCAPE || wParam == VK_BACK || wParam == VK_RETURN || 
            wParam == VK_LEFT || wParam == VK_RIGHT)
            return SendMessage(GetParent(hwnd), msg, wParam, lParam);
    }

    return CallWindowProc((WNDPROC)g_lpfnListView, hwnd, msg, wParam, lParam);
}

static void HookListView(HWND hwnd)
{
    g_lpfnListView = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (LONG)ListViewWndProc);
}

/*******************************************************************************
* OpenFile 对话框
*******************************************************************************/

static CList* g_files = NULL;
static TCHAR g_dir[MAX_PATH];
static TCHAR g_name[MAX_PATH];
static bool g_dirmode;

static void ClearFiles()
{
    for (int i = 0; i < g_files->GetCount(); i++)
    {
        WIN32_FIND_DATA* fd = (WIN32_FIND_DATA*)g_files->Get(i);
        delete fd;
    }
    g_files->Clear();
}

static int DoFileCompare(void* Item1, void* Item2)
{
    WIN32_FIND_DATA* p1 = (WIN32_FIND_DATA*)Item1;
    WIN32_FIND_DATA* p2 = (WIN32_FIND_DATA*)Item2;

    if (IsSameText(p1->cFileName, sParentDir))
        return -1;
    else if (IsSameText(p2->cFileName, sParentDir))
        return 1;

    if (IS_DIR(p1) && !IS_DIR(p2))
        return -1;
    else if (!IS_DIR(p1) && IS_DIR(p2))
        return 1;

    for (int i = 0; p1->cFileName[i] || p2->cFileName[i]; i++)
        if (p1->cFileName[i] == 0)
            return -1;
        else if (p2->cFileName[i] == 0)
            return 1;
        else if (tolower(p1->cFileName[i]) > tolower(p2->cFileName[i]))
            return 1;
        else if (tolower(p1->cFileName[i]) < tolower(p2->cFileName[i]))
            return -1;

        return 0;
}

static void UpdateFiles(TCHAR* dir)
{
    TCHAR buff[MAX_PATH];
    wcscpy(buff, g_dir);
    if (dir && dir[0])
    {
        if (IsSameText(dir, sParentDir))
        {
            for (int i = wcslen(buff) - 1; i >= 0; i--)
                if (buff[i] == '\\')
                {
                    wcscpy(g_name, &buff[i + 1]);
                    buff[i] = 0;
                    break;
                }
        }
        else
        {
            wcscat(buff, L"\\");
            wcscat(buff, dir);
            g_name[0] = 0;
        }
    }
    wcscpy(g_dir, buff);
    wcscat(buff, L"\\*.*");

    ClearFiles();

    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(buff, &fd);
    bool hasParent = false;
    while (hFind != INVALID_HANDLE_VALUE)
    {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 && fd.cFileName[0] && !IsSameText(fd.cFileName, L"."))
        {
            if (!g_dirmode || IS_DIR(&fd))
            {
                WIN32_FIND_DATA* pfd = new WIN32_FIND_DATA;
                *pfd = fd;
                g_files->Add(pfd);

                if (IsSameText(fd.cFileName,sParentDir))
                    hasParent = true;
            }
        }

        if (!FindNextFile(hFind, &fd))
        {
            FindClose(hFind);
            hFind = INVALID_HANDLE_VALUE;
        }
    }

    if (!hasParent && g_dir[0])
    {
        WIN32_FIND_DATA* pfd = new WIN32_FIND_DATA;
        memset(pfd, 0, sizeof(WIN32_FIND_DATA));
        wcscpy(pfd->cFileName, sParentDir);
        pfd->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
        g_files->Add(pfd);
    }

    if (g_files->GetCount() > 1)
        g_files->Sort(DoFileCompare);
}

BOOL ShowOpenFileDlg(HWND hParent, TCHAR* name, int size, bool dirmode)
{
    BOOL lRes = 0;

    g_dirmode = dirmode;
    g_files = new CList();
    g_dir[0] = 0;

    wcscpy(g_dir, name);
    PathUnixToWin(g_dir);

    // 初始化文件
    g_name[0] = 0;
    if (g_dir[0])
    {
        for (int i = wcslen(g_dir) - 1; i; i--)
        {
            if (g_dir[i] == '\\')
            {
                wcscpy(g_name, &g_dir[i + 1]);
                g_dir[i] = 0;
                break;
            }
        }
    }
    else
    {
        SHGetSpecialFolderPath(NULL, g_dir, CSIDL_STARTMENU, FALSE);
    }

    if (DialogBox(hRes, MAKEINTRESOURCE(IDD_DLGOPENFILE), hParent, OpenFile_DlgProc) == IDOK)
    {
        wsprintf(name, L"%s\\%s", g_dir, g_name);
        lRes = TRUE;
    }
    else
        lRes = FALSE;

    ClearFiles();
    delete g_files;
    g_files = NULL;

    return lRes;
}

static void OpenFileDlg_UpdateFileList(HWND hwnd, TCHAR* dir)
{
    UpdateFiles(dir);

    LV_COLUMN lvColumn;
    SHFILEINFO info;
    TCHAR buff[MAX_PATH];
    memset(&lvColumn, 0, sizeof(LV_COLUMN));
    memset(&info, 0, sizeof(info));
    wcscpy(buff, g_dir);
    if (buff[0] == 0)
        wcscpy(buff, L"\\");
    SHGetFileInfo(buff, 0, &info, sizeof(info), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    lvColumn.mask = LVCF_IMAGE | LVCF_TEXT;
    lvColumn.iImage = info.iIcon;
    lvColumn.pszText = buff;
    lvColumn.cchTextMax = MAX_PATH;
    ListView_SetColumn(hwnd, 0, &lvColumn);

    ListView_SetItemCount(hwnd, g_files->GetCount());
    int idx = 0;
    if (g_name[0])
    {
        for (int i = 0; i < g_files->GetCount(); i++)
            if (IsSameText(g_name, ((WIN32_FIND_DATA*)g_files->Get(i))->cFileName))
            {
                idx = i;
                break;
            }
    }
    SetListViewItemIndex(hwnd, idx);
    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT OpenFileDlg_OnLVNotify(HWND hwnd, LPARAM lParam)
{
    NMHDR* pnmh = (NMHDR*) lParam;
    LRESULT lResult = 0;

    switch(pnmh->code)
    {
    case LVN_GETDISPINFO:
        {
            LV_DISPINFO* pdi = (LV_DISPINFO*)lParam;
            WIN32_FIND_DATA* fd = (WIN32_FIND_DATA*)g_files->Get(pdi->item.iItem);

            if (pdi->item.mask & LVIF_TEXT)
            {
                wcscpy(pdi->item.pszText, fd->cFileName);
            }

            if (pdi->item.mask & LVIF_IMAGE)
            {
                SHFILEINFO info;
                TCHAR buff[MAX_PATH];
                memset(&info, 0, sizeof(info));
                wcscpy(buff, g_dir);
                if (!IsSameText(fd->cFileName, sParentDir))
                {
                    wcscat(buff, L"\\");
                    wcscat(buff, fd->cFileName);
                }

                SHGetFileInfo(buff, 0, &info, sizeof(info), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
                pdi->item.iImage = info.iIcon;
            }
        }
        break;
    }
    return(lResult);
}

// 初始化 ListView 控件
static void OpenFileDlg_InitListView(HWND hwnd, UINT ctrlid)
{
    RECT rc;
    HWND hListView = GetDlgItem(hwnd, ctrlid);
    GetClientRect(hwnd, &rc);
    MoveWindow(hListView, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);

    // 初始化栏头
    LV_COLUMN lvColumn;
    memset(&lvColumn, 0, sizeof(LV_COLUMN));
    lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_IMAGE | LVCF_WIDTH;
    lvColumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE | LVCFMT_COL_HAS_IMAGES;
    lvColumn.pszText = L"";
    lvColumn.cchTextMax = MAX_PATH;
    lvColumn.iImage = -1;
    lvColumn.cx = rc.right - rc.left - GetSystemMetrics(SM_CXVSCROLL);
    ListView_InsertColumn(hListView, 0, &lvColumn);
    ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_NOHSCROLL |
        (GetOSVer() < OS_WM5 ? LVS_EX_GRADIENT : 0));

    // 设置系统图标集
    SHFILEINFO info;
    memset(&info, 0, sizeof(info));
    int hImgLst = SHGetFileInfo(L"\\windows", 0, &info, sizeof(info), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    if (hImgLst)
        ListView_SetImageList(hListView, hImgLst, LVSIL_SMALL);

    HookListView(hListView);

    OpenFileDlg_UpdateFileList(hListView, NULL);
}

// 初始化对话框
static void OpenFileDlg_InitDialog(HWND hwnd)
{
    InitializeDialogEx(hRes, hwnd, NULL, IDR_MENU_OKCANCEL, DI_SCALE | DI_SETFONT | DI_TEXTBOLD | DI_RIDEBACK);

    // 更新标题
    wchar_t title[TITLE_LEN];
    LoadString(hRes, IDS_CAPTION_OPENFILE, title, TITLE_LEN);
    SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)title);

    OpenFileDlg_InitListView(hwnd, IDC_LISTFILES);
}

static void OpenFileDlg_EndDialog(HWND hwnd, int nResult)
{
    EndDialog(hwnd, nResult);
}

// 处理控件事件
static void OpenFileDlg_OnCommand(HWND hwnd, UINT cmd)
{
    HWND hListView = GetDlgItem(hwnd, IDC_LISTFILES);
    int idx = GetListViewItemIndex(hListView);
    WIN32_FIND_DATA* fd;

    switch(cmd)
    {
    case IDCANCEL:
        OpenFileDlg_EndDialog(hwnd, cmd);
        break;

    case IDOK:
    case VK_RETURN:
        if (idx >= 0)
        {
            fd = (WIN32_FIND_DATA*)g_files->Get(idx);
            if (IS_DIR(fd))
            {
                if (g_dirmode && !IsSameText(fd->cFileName, sParentDir) && cmd == IDOK)
                {
                    wcscpy(g_name, fd->cFileName);
                    OpenFileDlg_EndDialog(hwnd, IDOK);
                }
                else
                    OpenFileDlg_UpdateFileList(hListView, fd->cFileName);
            }
            else if (!g_dirmode)
            {
                wcscpy(g_name, fd->cFileName);
                OpenFileDlg_EndDialog(hwnd, IDOK);
            }
        }
        break;

    case VK_BACK:
        if (g_dir[0])
            OpenFileDlg_UpdateFileList(hListView, sParentDir);
        break;
    }
}

// 对话框主过程
BOOL CALLBACK OpenFile_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hListView = GetDlgItem(hwnd, IDC_LISTFILES);

    switch(message)
    {
    case WM_INITDIALOG:
        OpenFileDlg_InitDialog(hwnd);
        return TRUE;

    case WM_KEYDOWN:
        if (wParam == VK_LEFT)
            SendMessage(hListView, WM_KEYDOWN, VK_PRIOR, lParam);
        else if (wParam == VK_RIGHT)
            SendMessage(hListView, WM_KEYDOWN, VK_NEXT, lParam);
        else
            OpenFileDlg_OnCommand(hwnd, wParam);
        return TRUE;

    case WM_COMMAND:
        OpenFileDlg_OnCommand(hwnd, wParam);
        return TRUE;

    case WM_NOTIFY:
        OpenFileDlg_OnLVNotify(hListView, lParam);
        return TRUE;

    case WM_HOTKEY:
        if (VK_TBACK == HIWORD(lParam) && (MOD_KEYUP & LOWORD(lParam)) == 0)
            OpenFileDlg_OnCommand(hwnd, VK_BACK);
        break;
    }
    return FALSE;
}
