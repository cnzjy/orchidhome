/*******************************************************************************
* ������ƣ�Orchid �����ֻ����ܼ����幤��
* �ļ����ƣ������ܵ�Ԫ
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OCustKey.h,v 1.7 2008/02/29 01:11:27 Administrator Exp $
* ��    ע��
*******************************************************************************/

#pragma once

#include "stdafx.h"
#include <msxml.h>

class CKeyItem
{
public:
    CKeyItem();
    ~CKeyItem();

    HRESULT LoadFromXML(IXMLDOMNode* pNode);
    void DeleteShortcut();
    void SetDestFile(TCHAR * filename);
    void ResetDestFile();
    void UpdateDestFile();
    bool ApplyDestFile();

    TCHAR * Name;
    TCHAR * Shortcut;
    TCHAR * DestFile;
    TCHAR * DispName;
    HIMAGELIST hImglst;
    int iIcon;
    bool Modified;

private:
    void UpdateDispName();
    bool UpdateIcon();
    void FreeIcon();
};

class CPhoneItem
{
public:
    CPhoneItem();
    ~CPhoneItem();

    CKeyItem * Get(int Index);
    int Count();

    void UpdateFiles();
    bool LoadFromXML(IXMLDOMDocument* xml);
    bool LoadFromFile(TCHAR * filename);

    TCHAR * IdStr;
    TCHAR * Name;

private:
    HRESULT DoLoadFromXML(IXMLDOMNode* pNode);
    CList * m_list;
};

class CPhoneList
{
public:
    CPhoneList();
    ~CPhoneList();

    CPhoneItem * Get(int Index);
    int Count();

    bool LoadFromDir(TCHAR * dir);

private:
    CList * m_list;
};
