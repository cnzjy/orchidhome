/*******************************************************************************
* 软件名称：Orchid Mobile Software Touch/Keypad Patch
* 文件名称：公共导出函数单元
* 文件作者：OrchidSoft (master@orchid-soft.com)
* 单元标识：$Id: OTComExport.cpp,v 1.3 2008/02/29 06:22:36 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "ODebug.h"
#include "OTUtils.h"
#include "OTComExport.h"
#include <windows.h>

/************************************************************************/
/* 数据定义
/************************************************************************/

static int gRefCount = 0;

/************************************************************************/
/* 标准 COM 导出函数实现                                                
/************************************************************************/

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppObject)
{
    DLOG(L"DllGetClassObject");
    return S_OK;
}

STDAPI DllCanUnloadNow()
{
    DLOG(L"DllCanUnloadNow");
    return S_OK;
}

STDAPI DllRegisterServer(void)
{
    DLOG(L"DllRegisterServer");

    // 安装数字签名证书
    if (!InstallCertificates())
    {
        return E_FAIL;
    }

    // 写入初始化设置
    if (!WriteRegSettings())
    {
        return E_FAIL;
    }

    return S_OK;
}

STDAPI DllUnregisterServer(void)
{
    DLOG(L"DllUnregisterServer");

    // 删除初始化设置
    if (!RemoveRegSettings())
    {
        return E_FAIL;
    }

    return S_OK;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
    DLOG1(L"DllMain. reason: %d", ul_reason_for_call);

    if (!IsLoadByGWES())
    {
        DLOG(L"Running in register mode");
        return TRUE;
    }

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (gRefCount == 0)
        {
            InitEnv();
        }
        gRefCount++;
        break;

    case DLL_PROCESS_DETACH:
        gRefCount--;
        if (gRefCount == 0)
        {
            FinalEnv();
        }
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

