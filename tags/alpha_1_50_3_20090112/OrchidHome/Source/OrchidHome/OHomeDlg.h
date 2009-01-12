/*******************************************************************************
* ������ƣ�Orchid Mobile Software Home Plugin
* �ļ����ƣ�ϵͳ��Ϣͷ�ļ�
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OHomeDlg.h,v 1.9 2006/06/06 01:24:04 zjy Exp $
* ��    ע��
*******************************************************************************/

#pragma once

#define TITLE_LEN     64

/************************************************************************/
/* �Ի�����                                                           */
/************************************************************************/

#define PAGE_CALENDAR   0
#define PAGE_ALARM      1
#define PAGE_USERDAY    2
#define PAGE_TOOLS      3
#define PAGE_FIRST      0
#define PAGE_LAST       3

void ShowMainDlg(int page = -1);

BOOL CALLBACK Main_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK EditHoliday_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK EditAlarm_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK Alarms_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK Option_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
