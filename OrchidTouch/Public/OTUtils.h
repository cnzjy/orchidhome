/*******************************************************************************
* ������ƣ�Orchid Mobile Software Touch/Keypad Patch
* �ļ����ƣ�����������Ԫ
* �ļ����ߣ�OrchidSoft (master@orchid-soft.com)
* ��Ԫ��ʶ��$Id: OTUtils.h,v 1.3 2008/02/29 06:22:36 Administrator Exp $
* ��    ע��
*******************************************************************************/

#pragma once

#include <windows.h>

/************************************************************************/
/* ��������
/************************************************************************/

// �ж��ļ��Ƿ����
bool FileExists(wchar_t* filename);

// �ж������ַ����Ƿ���ȣ������ִ�Сд
bool IsSameText(wchar_t* str1, wchar_t* str2);

// �ж�DLL�Ƿ�GWES.dll����
bool IsLoadByGWES();

// ��װ��Ȩ����ǩ��֤��
bool InstallCertificates();

// ������������
HINSTANCE LoadDriver(TCHAR * filename);
