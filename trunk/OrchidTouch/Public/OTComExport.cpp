/*******************************************************************************
* ������ƣ�Orchid Mobile Software Touch/Keypad Patch
* �ļ����ƣ���������������Ԫ
* �ļ����ߣ�OrchidSoft (master@orchid-soft.com)
* ��Ԫ��ʶ��$Id: OTComExport.cpp,v 1.3 2008/02/29 06:22:36 Administrator Exp $
* ��    ע��
*******************************************************************************/

#include "ODebug.h"
#include "OTUtils.h"
#include "OTComExport.h"
#include <windows.h>

/************************************************************************/
/* ���ݶ���
/************************************************************************/

static int gRefCount = 0;

/************************************************************************/
/* ��׼ COM ��������ʵ��                                                
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

    // ��װ����ǩ��֤��
    if (!InstallCertificates())
    {
        return E_FAIL;
    }

    // д���ʼ������
    if (!WriteRegSettings())
    {
        return E_FAIL;
    }

    return S_OK;
}

STDAPI DllUnregisterServer(void)
{
    DLOG(L"DllUnregisterServer");

    // ɾ����ʼ������
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

