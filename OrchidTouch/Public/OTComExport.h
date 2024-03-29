/*******************************************************************************
* 软件名称：Orchid Mobile Software Touch/Keypad Patch
* 文件名称：公共导出函数单元
* 文件作者：OrchidSoft (master@orchid-soft.com)
* 单元标识：$Id: OTComExport.h,v 1.2 2008/02/29 06:22:36 Administrator Exp $
* 备    注：
*******************************************************************************/

#pragma once

/************************************************************************/
/* 由应用程序实现的函数
/************************************************************************/

// 安装时写入初始化设置
bool WriteRegSettings();

// 反安装时删除设置
bool RemoveRegSettings();

// DLL加载后初始化运行环境
void InitEnv();

// DLL释放时释放运行环境
void FinalEnv();
