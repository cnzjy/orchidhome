/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：COM DLL 注册工具
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OrchidRegSvr32.cpp,v 1.5 2006/10/24 03:58:00 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "stdafx.h"
#include "OrchidRegSvr32.h"
#include <windows.h>
#include <commctrl.h>
#include "OUtils.h"

#define STR_LEN 255

HINSTANCE hInst;			// 当前实例

TCHAR sAppPath[MAX_PATH] = {0};

void DoAddPath(HWND hlst, TCHAR* fn)
{
  HANDLE hfind;
  WIN32_FIND_DATA fd;
  TCHAR sz[MAX_PATH];

  wcscpy(sz, sAppPath);
  wcscat(sz, fn);
  hfind = FindFirstFile(sz, &fd);
  if (hfind != INVALID_HANDLE_VALUE)
  {
    do 
    {
      TCHAR buff[MAX_PATH];
      wcscpy(buff, sAppPath);
      wcscat(buff, fd.cFileName);
      if (DllCanReg(buff))
        SendMessage(hlst, LB_ADDSTRING, 0, (LPARAM)fd.cFileName);
    } while(FindNextFile(hfind, &fd));

    FindClose(hfind);
  }
}

void InitFileList(HWND hwndDlg)
{
  HWND listbox;

  listbox = GetDlgItem(hwndDlg, IDC_INFOLIST);
  DoAddPath(listbox, _T("*.dll"));

  if (SendMessage(listbox, LB_GETCOUNT, 0, 0))
  {
    SendMessage(listbox, LB_SETCURSEL, 0, 0);
  }
}

BOOL CALLBACK Info_DlgProc(HWND hwndDlg, UINT message,
                           WPARAM wParam, LPARAM lParam)
{
  TCHAR buff[STR_LEN];
  switch(message){
    case WM_INITDIALOG:
      {
        InitializeDialog(hInst, hwndDlg, IDC_INFOLIST, IDR_MENU, false);

        LoadString(hInst, IDS_APP_TITLE, buff, STR_LEN);
        SendMessage(hwndDlg, WM_SETTEXT, 0, (LPARAM)buff);

        InitFileList(hwndDlg);
        return TRUE;
      }
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
      case IDM_OK:
        EndDialog(hwndDlg, LOWORD(wParam));
        return TRUE;

      case IDM_REG:
      case IDM_UNREG:
        int select;
        TCHAR sz[128];
        select = SendMessage(GetDlgItem(hwndDlg, IDC_INFOLIST), LB_GETCURSEL, 0, 0);
        if (select != LB_ERR)
        {
          if (SendMessage(GetDlgItem(hwndDlg, IDC_INFOLIST), LB_GETTEXT, select, (LPARAM)sz) != LB_ERR)
          {
            wcscpy(buff, sAppPath);
            wcscat(buff, sz);
            if (DoRegDll(buff, LOWORD(wParam) == IDM_REG))
            {
              ShowMsgBoxRes(hInst, hwndDlg, IDS_INFO, LOWORD(wParam) == IDM_REG ? IDS_REGSUCC : IDS_UNREGSUCC, 
                MB_OK | MB_ICONINFORMATION);
            }
            else
            {
              ShowMsgBoxRes(hInst, hwndDlg, IDS_ERROR, IDS_OPFAIL, MB_OK | MB_ICONERROR);          
            }
          }
        }
        return TRUE;
      }
      return FALSE;
  }
  return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
  hInst = hInstance;

  GetModuleFileName(NULL, sAppPath, MAX_PATH);
  for (int i = wcslen(sAppPath); i; i--)
  {
    if ((sAppPath[i] == '\\') || (sAppPath[i] == '/'))
    {
      sAppPath[i + 1] = 0;
      break;
    }
  }

  DialogBox(hInst, MAKEINTRESOURCE(IDD_INFODLG), NULL, Info_DlgProc);

  return 0;
}
