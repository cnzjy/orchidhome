/*******************************************************************************
* 软件名称：Orchid Mobile Software Touch/Keypad Patch
* 文件名称：配置单元
* 文件作者：OrchidSoft (master@orchid-soft.com)
* 单元标识：$Id: OTConfig.h,v 1.3 2008/02/29 06:22:35 Administrator Exp $
* 备    注：
*******************************************************************************/

#pragma once

/************************************************************************/
/* 配置参数
/************************************************************************/

#define DEF_Interval 150
#define DEF_Delay    150
#define DEF_AreaX    50
#define DEF_AreaY    50

// 两次点击最小间隔
extern UINT g_Interval;

// 第二次点击最小延时
extern INT g_Delay;

// 相邻两次点击最小距离
extern INT g_AreaX;
extern INT g_AreaY;

// 旧驱动名称
extern TCHAR g_OldDriverName[MAX_PATH];

// 加载配置参数
void LoadSettings();
