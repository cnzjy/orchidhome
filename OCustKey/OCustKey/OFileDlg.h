/*******************************************************************************
* ������ƣ�Orchid �����ֻ����ܼ����幤��
* �ļ����ƣ��ļ��Ի���Ԫ
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OFileDlg.h,v 1.4 2008/02/29 01:11:27 Administrator Exp $
* ��    ע��
*******************************************************************************/

#pragma once

#include "stdafx.h"

BOOL ShowOpenFileDlg(HWND hParent, TCHAR* name, int size, bool dirmode);

BOOL CALLBACK OpenFile_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
