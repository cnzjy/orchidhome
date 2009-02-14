/*******************************************************************************
* 软件名称：Orchid 智能手机功能键定义工具
* 文件名称：文件对话框单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OFileDlg.h,v 1.4 2008/02/29 01:11:27 Administrator Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include "stdafx.h"

BOOL ShowOpenFileDlg(HWND hParent, TCHAR* name, int size, bool dirmode);

BOOL CALLBACK OpenFile_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
