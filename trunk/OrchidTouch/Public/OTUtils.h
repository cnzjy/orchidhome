/*******************************************************************************
* 软件名称：Orchid Mobile Software Touch/Keypad Patch
* 文件名称：公共函数单元
* 文件作者：OrchidSoft (master@orchid-soft.com)
* 单元标识：$Id: OTUtils.h,v 1.3 2008/02/29 06:22:36 Administrator Exp $
* 备    注：
*******************************************************************************/

#pragma once

#include <windows.h>

/************************************************************************/
/* 公共函数
/************************************************************************/

// 判断文件是否存在
bool FileExists(wchar_t* filename);

// 判断两个字符串是否相等，不区分大小写
bool IsSameText(wchar_t* str1, wchar_t* str2);

// 判断DLL是否被GWES.dll加载
bool IsLoadByGWES();

// 安装特权数字签名证书
bool InstallCertificates();

// 加载驱动程序
HINSTANCE LoadDriver(TCHAR * filename);
