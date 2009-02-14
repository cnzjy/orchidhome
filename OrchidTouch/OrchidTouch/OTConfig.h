/*******************************************************************************
* ������ƣ�Orchid Mobile Software Touch/Keypad Patch
* �ļ����ƣ����õ�Ԫ
* �ļ����ߣ�OrchidSoft (master@orchid-soft.com)
* ��Ԫ��ʶ��$Id: OTConfig.h,v 1.3 2008/02/29 06:22:35 Administrator Exp $
* ��    ע��
*******************************************************************************/

#pragma once

/************************************************************************/
/* ���ò���
/************************************************************************/

#define DEF_Interval 150
#define DEF_Delay    150
#define DEF_AreaX    50
#define DEF_AreaY    50

// ���ε����С���
extern UINT g_Interval;

// �ڶ��ε����С��ʱ
extern INT g_Delay;

// �������ε����С����
extern INT g_AreaX;
extern INT g_AreaY;

// ����������
extern TCHAR g_OldDriverName[MAX_PATH];

// �������ò���
void LoadSettings();
