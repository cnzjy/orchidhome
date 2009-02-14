/*******************************************************************************
* 软件名称：Orchid 智能手机功能键定义工具
* 文件名称：主功能单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OCustKey.cpp,v 1.17 2008/02/29 01:11:27 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "stdafx.h"
#include "OCustKey.h"
#include "OFileDlg.h"
#include "OUtils.h"
#include "ORegistry.h"
#include "OList.h"

#define MAX_LOADSTRING 100
#define TITLE_LEN      64
#define IS_DIR(fd)  (((fd)->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)

HINSTANCE	hInst;
HINSTANCE	hRes;

CPhoneList * g_PhoneList = NULL;
CPhoneItem * g_Phone = NULL;
int g_PhoneIdx = -1;
TCHAR * g_IdStr = NULL;
CHomeRegistry * g_Reg = NULL;

static TCHAR * sRegPath = L"\\Software\\Orchid\\OCustKey";
static TCHAR * sRegIdStr = L"PhoneIdStr";
static TCHAR * sCfgExt = L".okey.xml";
static TCHAR * sLnkExt = L".lnk";
static TCHAR * sXmlPhone = L"Phone";
static TCHAR * sXmlItem = L"Item";
static TCHAR * sXmlName = L"Name";
static TCHAR * sXmlShortcut = L"Shortcut";
static TCHAR * sParentDir = L"..";

static TCHAR sCfgDir[MAX_PATH];

CKeyItem::CKeyItem()
{
    InitString(Name);
    InitString(Shortcut);
    InitString(DestFile);
    InitString(DispName);
    hImglst = NULL;
    iIcon = -1;
    Modified = false;
}

CKeyItem::~CKeyItem()
{
    FreeString(Name);
    FreeString(Shortcut);
    FreeString(DestFile);
    FreeString(DispName);
    FreeIcon();
}

void CKeyItem::FreeIcon()
{
    hImglst = NULL;
    iIcon = -1;
}

HRESULT CKeyItem::LoadFromXML(IXMLDOMNode* pNode)
{
    IXMLDOMNamedNodeMap *pNodeMap = NULL;

    if (FAILED(pNode->get_attributes(&pNodeMap)))
        return E_FAIL;

    if (pNodeMap)
    {
        if (FAILED(XMLGetStrAttribute(pNodeMap, sXmlName, &Name)))
            return E_FAIL;
        if (FAILED(XMLGetStrAttribute(pNodeMap, sXmlShortcut, &Shortcut)))
            return E_FAIL;

        pNodeMap->Release();
    }

    return S_OK;
}

void CKeyItem::UpdateDispName()
{
    CloneString(DispName, DestFile);
    if (DestFile && DestFile[0])
    {
        SHFILEINFO info;
        memset(&info, 0, sizeof(info));
        if (SHGetFileInfo(DestFile, 0, &info, sizeof(info), SHGFI_DISPLAYNAME))
        {
            TCHAR buff[MAX_PATH];
            buff[0] = 0;
            wcscpy(buff, info.szDisplayName);
            for (int i = wcslen(buff) - 1; i; i--)
            {
                if (buff[i] == '"')
                    buff[i] = 0;
                else if (buff[i] == '.')
                {
                    buff[i] = 0;
                    break;
                }
            }
            CloneString(DispName, buff);
        }
        else
        {
            for (int i = wcslen(DestFile) - 1; i >= 0; i--)
            {
                if ((DestFile[i] == '\\') || (DestFile[i] == '/'))
                {
                    CloneString(DispName, &DestFile[i + 1]);
                    break;
                }
            }
        }
    }
    else
    {
        TCHAR buff[TITLE_LEN];
        LoadString(hRes, IDS_NONE, buff, TITLE_LEN);
        CloneString(DispName, buff);
    }
}

bool CKeyItem::UpdateIcon()
{
    TCHAR * fn = Modified ? DestFile : Shortcut;
    FreeIcon();
    if (FileExists(fn))
    {
        SHFILEINFO info;
        hImglst = (HIMAGELIST)SHGetFileInfo(fn, 0, &info, sizeof(info), SHGFI_SYSICONINDEX | 
            SHGFI_SMALLICON);
        if (hImglst)
        {
            iIcon = info.iIcon;
            return true;
        }
    }
    return false;
}

void CKeyItem::DeleteShortcut()
{
    Modified = true;
    FreeString(DestFile);
    UpdateDispName();
    UpdateIcon();
}

void CKeyItem::SetDestFile(TCHAR * filename)
{
    Modified = true;
    CloneString(DestFile, filename);
    UpdateDispName();
    UpdateIcon();
}

void CKeyItem::ResetDestFile()
{
    Modified = false;
    UpdateDestFile();
}

void CKeyItem::UpdateDestFile()
{
    TCHAR buff[MAX_PATH];
    if (FileExists(Shortcut) && SHGetShortcutTarget(Shortcut, buff, MAX_PATH))
    {
        CloneString(DestFile, buff);
        UpdateDispName();
        UpdateIcon();
    }
    else
    {
        FreeString(DestFile);
        UpdateDispName();
        UpdateIcon();
    }
}

bool CKeyItem::ApplyDestFile()
{
    if (Modified)
    {
        // 判断目标是否存在
        if (FileExists(DestFile))
        {
            // 创建目录
            if (!CreateDirectoriesForFile(Shortcut))
                return false;

            // 判断目标是否已经是快捷方式
            if (IsSameText(&DestFile[wcslen(DestFile) - wcslen(sLnkExt)], sLnkExt))
            {
                return CopyFile(DestFile, Shortcut, false) ? true : false;
            }
            else
            {
                TCHAR buff[MAX_PATH];
                wsprintf(buff, L"\"%s\"", DestFile);
                DeleteFile(Shortcut);
                return SHCreateShortcut(Shortcut, buff) ? true : false;
            }
        }
        else
        {
            DeleteFile(Shortcut);
            return true;
        }
    }
    else
        return true;
}

CPhoneItem::CPhoneItem()
{
    InitString(Name);
    InitString(IdStr);
    m_list = new CList();
}

CPhoneItem::~CPhoneItem()
{
    FreeString(Name);
    FreeString(IdStr);
    while (Count() > 0)
    {
        CKeyItem * item = Get(0);
        delete item;
        m_list->Delete(0);
    }
    delete m_list;
}

CKeyItem * CPhoneItem::Get(int Index)
{
    return (CKeyItem*)m_list->Get(Index);
}

int CPhoneItem::Count()
{
    return m_list->GetCount();
}

void CPhoneItem::UpdateFiles()
{
    for (int i = 0; i < Count(); i++)
    {
        Get(i)->UpdateDestFile();
    }
}

HRESULT CPhoneItem::DoLoadFromXML(IXMLDOMNode* pNode)
{
    IXMLDOMNamedNodeMap *pNodeMap = NULL;

    if (FAILED(pNode->get_attributes(&pNodeMap)))
        return E_FAIL;

    if (pNodeMap)
    {
        if (FAILED(XMLGetStrAttribute(pNodeMap, sXmlName, &Name)))
            return E_FAIL;
        pNodeMap->Release();
    }

    IXMLDOMNode *pChild = NULL;
    DOMNodeType nType;
    CKeyItem* item;

    pNode->get_firstChild(&pChild);
    while (pChild)
    {
        if (SUCCEEDED(pChild->get_nodeType(&nType)) && (nType == NODE_ELEMENT))
        {
            if (XMLNodeNameIs(pChild, sXmlItem))
            {
                item = new CKeyItem();
                if (FAILED(item->LoadFromXML(pChild)))
                {
                    delete item;
                    return E_FAIL;
                }
                m_list->Add(item);
            }
        }
        pChild->get_nextSibling(&pChild);
    }

    UpdateFiles();

    return S_OK;
}

bool CPhoneItem::LoadFromXML(IXMLDOMDocument* xml)
{
    IXMLDOMNode* root = NULL;
    IXMLDOMNode* node = NULL;

    xml->get_firstChild(&root);
    while (root)
    {
        if (XMLNodeNameIs(root, sXmlPhone))
        {
            if (FAILED(DoLoadFromXML(root)))
                return false;
            break;
        }
        root->get_nextSibling(&root);
    }

    if (root) root->Release();

    return Count() > 0;
}

bool CPhoneItem::LoadFromFile(TCHAR * filename)
{
    bool succ = false;
    IXMLDOMDocument* xml = NULL;
    if (SUCCEEDED(XMLCreateDocument(&xml)))
    {
        if (SUCCEEDED(XMLLoadFromFile(xml, filename)))
            succ = LoadFromXML(xml);
    }
    if (xml) xml->Release();

    return Count() > 0;
}

CPhoneList::CPhoneList()
{
    m_list = new CList();
}

CPhoneList::~CPhoneList()
{
    while (Count() > 0)
    {
        CPhoneItem * item = Get(0);
        delete item;
        m_list->Delete(0);
    }
    delete m_list;
}

CPhoneItem * CPhoneList::Get(int Index)
{
    return (CPhoneItem*)m_list->Get(Index);
}

int CPhoneList::Count()
{
    return m_list->GetCount();
}

static int DoPhoneCompare(void* Item1, void* Item2)
{
    CPhoneItem* p1 = (CPhoneItem*)Item1;
    CPhoneItem* p2 = (CPhoneItem*)Item2;

    return OU_CompareString(p1->IdStr, p2->IdStr);
}

bool CPhoneList::LoadFromDir(TCHAR * dir)
{
    TCHAR buff[MAX_PATH];
    WIN32_FIND_DATA fd;
    HANDLE hFind;

    wsprintf(buff, L"%s\\*%s", dir, sCfgExt);
    hFind = FindFirstFile(buff, &fd);
    while (hFind != INVALID_HANDLE_VALUE)
    {
        if ((fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN| FILE_ATTRIBUTE_DIRECTORY)) == 0 && fd.cFileName[0])
        {
            TCHAR fn[MAX_PATH];
            CPhoneItem * item = new CPhoneItem();

            wcscpy(fn, fd.cFileName);
            fn[wcslen(fn) - wcslen(sCfgExt)] = 0;
            CloneString(item->IdStr, fn);

            wsprintf(fn, L"%s\\%s", dir, fd.cFileName);
            if (!item->LoadFromFile(fn))
            {
                delete item;
                return false;
            }
            m_list->Add(item);
        }

        if (!FindNextFile(hFind, &fd))
        {
            FindClose(hFind);
            hFind = INVALID_HANDLE_VALUE;
        }
    }
    m_list->Sort(DoPhoneCompare);

    return Count() > 0;
}

static bool AutoSelPhone()
{
    int cnt, maxcnt = -1, i, j;
    FreeString(g_IdStr);
    g_PhoneIdx = -1;
    g_Phone = NULL;
    for (i = 0; i < g_PhoneList->Count(); i++)
    {
        CPhoneItem * item = g_PhoneList->Get(i);
        cnt = 0;
        for (j = 0; j < item->Count(); j++)
        {
            if (item->Get(j)->DestFile)
            {
                cnt++;
            }
        }
        if (cnt > maxcnt)
        {
            g_PhoneIdx = i;
            g_Phone = item;
            maxcnt = cnt;
        }
    }
    return g_Phone != NULL;
}

static bool InitEnv()
{
    GetModuleFileName(NULL, sCfgDir, MAX_PATH);
    for (int i = wcslen(sCfgDir); i; i--)
    {
        if (sCfgDir[i] == '\\' || sCfgDir[i] == '/')
        {
            sCfgDir[i] = 0;
            break;
        }
    }

    InitString(g_IdStr);
    g_Reg = new CHomeRegistry(sRegPath);
    g_Reg->ReadString(L"", sRegIdStr, g_IdStr);

    g_PhoneList = new CPhoneList();
    if (g_PhoneList->LoadFromDir(sCfgDir))
    {
        g_PhoneIdx = 0;
        for (int i = 0; i < g_PhoneList->Count(); i++)
        {
            if (IsSameText(g_PhoneList->Get(i)->IdStr, g_IdStr))
            {
                g_PhoneIdx = i;
                g_Phone = g_PhoneList->Get(g_PhoneIdx);
                return true;
            }
        }

        return AutoSelPhone();
    }
    return false;
}

static void FinalEnv()
{
    g_Reg->WriteString(L"", sRegIdStr, g_IdStr);
    FreeString(g_IdStr);

    delete g_PhoneList;
    delete g_Reg;
}

/*******************************************************************************
* 主对话框
*******************************************************************************/

LRESULT MainDlg_OnLVNotify(HWND hwnd, LPARAM lParam)
{
    NMHDR* pnmh = (NMHDR*) lParam;
    LRESULT lResult = 0;

    switch(pnmh->code)
    {
    case LVN_GETDISPINFO:
        {
            LV_DISPINFO* pdi = (LV_DISPINFO*)lParam;
            CKeyItem * item = g_Phone->Get(pdi->item.iItem);

            if (pdi->item.mask & LVIF_TEXT)
            {
                switch(pdi->item.iSubItem)
                {
                case 0:
                    wcscpy(pdi->item.pszText, item->Name);
                	break;
                case 1:
                    wcscpy(pdi->item.pszText, item->DispName);
                    break;
                }
            }

            if (pdi->item.mask & LVIF_IMAGE)
            {
                pdi->item.iImage = item->iIcon;
            }
        }
        break;
    }
    return(lResult);
}

// 初始化 ListView 控件
static void MainDlg_InitListView(HWND hwnd, UINT ctrlid)
{
    RECT rc, rclv, rcwin;
    HWND hListView = GetDlgItem(hwnd, ctrlid);
    GetClientRect(hwnd, &rc);
    GetWindowRect(hwnd, &rcwin);
    GetWindowRect(hListView, &rclv);
    MoveWindow(hListView, rc.left, rclv.top - rcwin.top, rc.right - rc.left, rc.bottom - (rclv.top - rcwin.top), FALSE);

    // 初始化栏头
    TCHAR buff[TITLE_LEN];
    for (int i = 0; i < 2; i++)
    {
        LV_COLUMN lvColumn;
        memset(&lvColumn, 0, sizeof(LV_COLUMN));

        lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.iSubItem = i;
        lvColumn.cx = (rc.right - rc.left - GetSystemMetrics(SM_CXVSCROLL)) * 2 / 3;
        LoadString(hRes, IDS_LV_SHNO + i, buff, TITLE_LEN - 1);
        lvColumn.pszText = buff;
        lvColumn.cchTextMax = TITLE_LEN;
        ListView_InsertColumn(hListView, i, &lvColumn);
    }

    ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT |
        (GetOSVer() < OS_WM5 ? LVS_EX_GRADIENT : 0));

    // 设置系统图标集
    SHFILEINFO info;
    memset(&info, 0, sizeof(info));
    int hImgLst = SHGetFileInfo(L"\\windows", 0, &info, sizeof(info), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    if (hImgLst)
        ListView_SetImageList(hListView, hImgLst, LVSIL_SMALL);

    ListView_SetItemCount(hListView, g_Phone->Count());
    if (g_Phone->Count() > 0)
        SetListViewItemIndex(hListView, 0);
    InvalidateRect(hListView, NULL, TRUE);
}

static void MainDlg_UpdateMenu(HWND hwnd)
{
    bool isauto = g_IdStr && g_IdStr[0] ? false : true;
    HMENU hMenu = GetDlgMenuBar(hwnd, ID_ACTION);
    hMenu = GetSubMenu(hMenu, ID_INDEX_SELPHONE);
    CheckMenuRadioItem(hMenu, 0, g_PhoneList->Count() + 2, isauto ? 0 : g_PhoneIdx + 2, MF_BYPOSITION);

    TCHAR buff[MAX_LOADSTRING], text[MAX_LOADSTRING];
    LoadString(hRes, isauto ? IDS_CFG_TEXT_AUTO : IDS_CFG_TEXT, buff, MAX_LOADSTRING);
    wsprintf(text, buff, g_Phone->Name);
    SetWindowText(GetDlgItem(hwnd, IDC_STATIC_TITLE), text);
}

static void MainDlg_InitMenu(HWND hwnd)
{
    HMENU hMenu = GetDlgMenuBar(hwnd, ID_ACTION);
    hMenu = GetSubMenu(hMenu, ID_INDEX_SELPHONE);
    for (int i = 0; i < g_PhoneList->Count(); i++)
    {
        InsertMenu(hMenu, i + 2, MF_STRING | MF_BYPOSITION, ID_SUB_PHONE + i, g_PhoneList->Get(i)->Name);
    }
    MainDlg_UpdateMenu(hwnd);
}

// 初始化对话框
static void MainDlg_InitDialog(HWND hwnd)
{
    InitializeDialogEx(hRes, hwnd, NULL, IDR_MENU_CONFIG, DI_RIDEBACK | DI_SCALE | DI_SETFONT);

    // 更新标题
    wchar_t title[MAX_LOADSTRING];
    LoadString(hRes, IDS_CONFIG_TITLE, title, MAX_LOADSTRING);
    SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)title);

    // 设置列表框位置
    RECT rcdlg, rclist;
    HWND hlist = GetDlgItem(hwnd, IDC_LIST);
    int border;
    GetWindowRect(hwnd, &rcdlg);
    GetWindowRect(hlist, &rclist);
    OffsetRect(&rclist, -rcdlg.left, -rcdlg.top);
    border = rclist.left;
    MoveWindow(hlist, rclist.left, rclist.top, rcdlg.right - 2 * border, 
        rcdlg.bottom - rclist.top - border - GetSystemMetrics(SM_CYMENU), false);

    MainDlg_InitMenu(hwnd);
    MainDlg_InitListView(hwnd, IDC_LIST);
}

static void MainDlg_EndDialog(HWND hwnd, int nResult)
{
    EndDialog(hwnd, nResult);
}

// 处理控件事件
static bool MainDlg_OnCommand(HWND hwnd, UINT cmd)
{
    HWND hListView = GetDlgItem(hwnd, IDC_LIST);
    TCHAR buff[MAX_PATH], text[MAX_PATH], names[MAX_PATH];
    int i, idx = GetListViewItemIndex(hListView);
    CKeyItem * item = idx >= 0 ? g_Phone->Get(idx) : NULL;
    bool succ = true;

    switch(cmd)
    {
    case IDCANCEL:
        MainDlg_EndDialog(hwnd, cmd);
        break;

    case ID_MENU_ABOUT:
        ShowMsgBoxRes(hInst, hwnd, NULL, IDS_ABOUT, MB_OK | MB_ICONINFORMATION);
        break;

    case ID_MENU_APPLY:
        MainDlg_EndDialog(hwnd, cmd);

        buff[0] = text[0] = names[0] = 0;
        for (i = 0; i < g_Phone->Count(); i++)
        {
            if (!g_Phone->Get(i)->ApplyDestFile())
            {
                if (names[0])
                {
                    wcscat(names, L", ");
                }
                wcscat(names, g_Phone->Get(i)->Name);
                succ = false;
            }
        }
        if (succ)
        {
            ShowMsgBoxRes(hInst, hwnd, NULL, IDS_APPLYSUCC, MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            LoadString(hRes, IDS_APPLYFAIL, text, MAX_PATH);
            wsprintf(buff, text, names);
            MessageBox(hwnd, buff, NULL, MB_OK | MB_ICONERROR);
        }
        break;

    case IDOK:
    case ID_ACTION_EDIT:
        if (item)
        {
            buff[0] = 0;
            if (ShowOpenFileDlg(hwnd, buff, MAX_PATH, false))
            {
                item->SetDestFile(buff);
                InvalidateRect(hListView, NULL, TRUE);
            }
        }
        break;

    case ID_ACTION_DELETE:
        if (item)
        {
            item->DeleteShortcut();
            InvalidateRect(hListView, NULL, TRUE);
        }
        break;

    case ID_ACTION_DELETE_ALL:
        for (i = 0; i < g_Phone->Count(); i++)
        {
            g_Phone->Get(i)->DeleteShortcut();
        }
        InvalidateRect(hListView, NULL, TRUE);
        break;

    case ID_ACTION_DEFAULT:
        if (idx >= 0)
        {
            item->ResetDestFile();
            InvalidateRect(hListView, NULL, TRUE);
        }
        break;

    case ID_ACTION_DEFAULT_ALL:
        for (i = 0; i < g_Phone->Count(); i++)
        {
            g_Phone->Get(i)->ResetDestFile();
        }
        InvalidateRect(hListView, NULL, TRUE);
        break;

    case ID_SEL_AUTO:
        AutoSelPhone();

        ListView_SetItemCount(hListView, g_Phone->Count());
        if (g_Phone->Count() > 0)
            SetListViewItemIndex(hListView, 0);
        InvalidateRect(hListView, NULL, TRUE);
        MainDlg_UpdateMenu(hwnd);
        break;

    default:
        i = cmd - ID_SUB_PHONE;
        if (i >= 0 && i < g_PhoneList->Count())
        {
            g_PhoneIdx = i;
            g_Phone = g_PhoneList->Get(i);
            CloneString(g_IdStr, g_Phone->IdStr);
            ListView_SetItemCount(hListView, g_Phone->Count());
            if (g_Phone->Count() > 0)
                SetListViewItemIndex(hListView, 0);
            InvalidateRect(hListView, NULL, TRUE);
            MainDlg_UpdateMenu(hwnd);
            return true;
        }
        return false;
    }

    return true;
}

// 处理按键
static void MainDlg_OnKeyDown(HWND hwnd, UINT key)
{
    MainDlg_OnCommand(hwnd, key);
}

// 对话框主过程
BOOL CALLBACK Main_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hListView = GetDlgItem(hwnd, IDC_LIST);

    switch(message)
    {
    case WM_INITDIALOG:
        MainDlg_InitDialog(hwnd);
        return TRUE;

    case WM_KEYDOWN:
        MainDlg_OnKeyDown(hwnd, wParam);
        return TRUE;

    case WM_COMMAND:
        if (MainDlg_OnCommand(hwnd, LOWORD(wParam)))
            return TRUE;
        break;

    case WM_NOTIFY:
        MainDlg_OnLVNotify(hListView, lParam);
        return TRUE;

    case WM_HOTKEY:
        if (VK_TBACK == HIWORD(lParam) && (0 != (MOD_KEYUP & LOWORD(lParam))))
        {
            MainDlg_EndDialog(hwnd, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
    TCHAR szTitle[MAX_LOADSTRING];		// 标题栏文本
    int nResult = 0;

    hInst = hInstance;
    hRes = hInst;

    LoadString(hRes, IDS_CONFIG_TITLE, szTitle, MAX_LOADSTRING); 

    HANDLE hMutex = CreateMutex(0, 0, _T("_OCUSTKEY_MUTEX_"));

    if (NULL != hMutex)
    {
        if (ERROR_ALREADY_EXISTS != GetLastError())
        {
            if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
            {
                CloseHandle(hMutex);
                return -1;
            }

            if (InitEnv())
            {
                nResult = DialogBox(hRes, MAKEINTRESOURCE(IDD_DLGCONFIG), NULL, Main_DlgProc);
            }
            else
            {
                ShowMsgBoxRes(hInst, 0, NULL, IDS_DATAERROR, MB_OK | MB_ICONERROR);
                nResult = 0;
            }

            FinalEnv();

            CoUninitialize();
        }
        else
        {
            HWND hWndExistingInstance = FindWindow(_T("Dialog"), szTitle);

            if (NULL != hWndExistingInstance)
            {
                SetForegroundWindow((HWND)((ULONG)hWndExistingInstance | 0x00000001));
            }
        }

        CloseHandle(hMutex);
    }

    return ( nResult >= 0 );
}
