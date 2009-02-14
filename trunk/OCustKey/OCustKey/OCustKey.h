/*******************************************************************************
* 软件名称：Orchid 智能手机功能键定义工具
* 文件名称：主功能单元
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: OCustKey.h,v 1.7 2008/02/29 01:11:27 Administrator Exp $
* 备    注：
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
