/*******************************************************************************
* ������ƣ�Orchid Mobile Software Touch/Keypad Patch
* �ļ����ƣ����õ�Ԫ
* �ļ����ߣ�OrchidSoft (master@orchid-soft.com)
* ��Ԫ��ʶ��$Id: OTConfig.cpp,v 1.7 2008/02/29 06:22:35 Administrator Exp $
* ��    ע��
*******************************************************************************/

#include "ODebug.h"
#include "OTConfig.h"
#include "OTUtils.h"
#include "OTComExport.h"
#include <windows.h>

/************************************************************************/
/* ���ò���
/************************************************************************/

// ���ε����С���
UINT g_Interval = DEF_Interval;

// �ڶ��ε����С��ʱ
INT  g_Delay = DEF_Delay;

// �������ε����С����
INT  g_AreaX = DEF_AreaX;
INT  g_AreaY = DEF_AreaY;

// ����������
TCHAR g_OldDriverName[MAX_PATH];

TCHAR * szDriverDllName = L"OrchidTouch.dll";
TCHAR * szRegTouchPath = L"HARDWARE\\DEVICEMAP\\TOUCH";
TCHAR * szRegDriverKey = L"DriverName";
TCHAR * szRegOldDriverKey = L"Och_DriverName";
TCHAR * szRegIntervalKey = L"Och_Interval";
TCHAR * szRegDelayKey = L"Och_Delay";
TCHAR * szRegAreaXKey = L"Och_AreaX";
TCHAR * szRegAreaYKey = L"Och_AreaY";
TCHAR * szDefDriverNames[] = {
    L"hwTouch.dll",  // ȫ����д����
    L"touch.dll",    // ϵͳԭʼ����
    NULL
};

/************************************************************************/
/* ��װ/����װ���� 
/************************************************************************/

#define REG_CHECK(fn)  if (fn != ERROR_SUCCESS) ret = false;
#define WSTR_SIZE(str) (wcslen(str) * sizeof(wchar_t))
#define REG_WRITE_INT(key, value) RegSetValueEx(hkey, key, NULL, REG_DWORD, (LPBYTE)&value, sizeof(DWORD));

// д���ʼ������
bool WriteRegSettings()
{
    DWORD disp;
    HKEY hkey;
    bool ret = true;

    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szRegTouchPath, 0, NULL, 0, NULL, NULL, &hkey, &disp) == ERROR_SUCCESS)
    {
        DWORD type, dwsize;
        TCHAR filename[MAX_PATH];

        DLOG1(L"RegCreateKeyEx %s succ", szRegTouchPath);
        dwsize = MAX_PATH;
        filename[0] = 0;
        RegQueryValueEx(hkey, szRegDriverKey, NULL, &type, (LPBYTE)filename, &dwsize);
        if (!IsSameText(filename, szDriverDllName))
        {
            if (filename[0])
            {
                // ԭ��������ʱ�ű���
                REG_CHECK(RegSetValueEx(hkey, szRegOldDriverKey, NULL, REG_SZ, (LPBYTE)filename, WSTR_SIZE(filename)));
                DLOG3(L"RegSetValueEx %s: %s %x", szRegOldDriverKey, filename, ret);
            }
            REG_CHECK(RegSetValueEx(hkey, szRegDriverKey, NULL, REG_SZ, (LPBYTE)szDriverDllName, WSTR_SIZE(szDriverDllName)));
            DLOG3(L"RegSetValueEx %s: %s %x", szRegDriverKey, szDriverDllName, ret);
        }

        // д���ʼ����
        REG_WRITE_INT(szRegIntervalKey, g_Interval);
        REG_WRITE_INT(szRegDelayKey, g_Delay);
        REG_WRITE_INT(szRegAreaXKey, g_AreaX);
        REG_WRITE_INT(szRegAreaYKey, g_AreaY);

        RegCloseKey(hkey);
    }
    else
    {
        DLOG1(L"RegCreateKeyEx %s fail", szRegTouchPath);
        return false;
    }

    return ret;
}

// ɾ����ʼ������
bool RemoveRegSettings()
{
    DWORD disp;
    HKEY hkey;
    bool ret = true;

    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szRegTouchPath, 0, NULL, 0, NULL, NULL, &hkey, &disp) == ERROR_SUCCESS)
    {
        DWORD type, dwsize;
        TCHAR filename[MAX_PATH];

        dwsize = MAX_PATH;
        filename[0] = 0;
        RegQueryValueEx(hkey, szRegOldDriverKey, NULL, &type, (LPBYTE)filename, &dwsize);
        if (FileExists(filename))
        {
            // ����������ʱ�ָ�
            REG_CHECK(RegSetValueEx(hkey, szRegDriverKey, NULL, REG_SZ, (LPBYTE)filename, WSTR_SIZE(filename)));
        }
        else
        {
            // ������������ʱɾ�������ϵͳʹ��Ĭ������
            REG_CHECK(RegDeleteValue(hkey, szRegDriverKey));
        }

        // ɾ������
        RegDeleteValue(hkey, szRegOldDriverKey);
        RegDeleteValue(hkey, szRegIntervalKey);
        RegDeleteValue(hkey, szRegDelayKey);
        RegDeleteValue(hkey, szRegAreaXKey);
        RegDeleteValue(hkey, szRegAreaYKey);

        RegCloseKey(hkey);
    }
    else
    {
        DLOG1(L"RegCreateKeyEx %s fail", szRegTouchPath);
        return false;
    }

    return ret;
}

/************************************************************************/
/* �������ò���
/************************************************************************/

void RegReadInt(HKEY hkey, TCHAR * name, int * value)
{
    DWORD type, dw, dwsize;
    dwsize = sizeof(DWORD);
    if (RegQueryValueEx(hkey, name, NULL, &type, (LPBYTE)&dw, &dwsize) == ERROR_SUCCESS)
    {
        if (type == REG_DWORD)
        {
            *value = (int)dw;
            DLOG2(L"%s: %d", name, *value);
        }
    }
}

void LoadSettings()
{
    DWORD disp;
    HKEY hkey;

    DLOG(L"LoadSettings()");

    // ��ȡ����
    memset(g_OldDriverName, 0, sizeof(g_OldDriverName));
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szRegTouchPath, 0, NULL, 0, NULL, NULL, &hkey, &disp) == ERROR_SUCCESS)
    {
        DWORD type, dwsize = MAX_PATH;

        // ��ע����ж�ȡԭ��������
        RegQueryValueEx(hkey, szRegOldDriverKey, NULL, &type, (LPBYTE)g_OldDriverName, &dwsize);
        DLOG1(L"OldDriverName: %s", g_OldDriverName);

        RegReadInt(hkey, szRegIntervalKey, (int*)&g_Interval);
        RegReadInt(hkey, szRegDelayKey, &g_Delay);
        RegReadInt(hkey, szRegAreaXKey, &g_AreaX);
        RegReadInt(hkey, szRegAreaYKey, &g_AreaY);

        RegCloseKey(hkey);
    }

    // ���ע����е����������ڣ�����Ĭ������
    if (!FileExists(g_OldDriverName))
    {
        memset(g_OldDriverName, 0, sizeof(g_OldDriverName));
        for (int i = 0; szDefDriverNames[i]; i++)
        {
            DLOG1(L"Checking... %s", szDefDriverNames[i]);
            if (FileExists(szDefDriverNames[i]))
            {
                DLOG(L"Driver exists.");
                wcscpy(g_OldDriverName, szDefDriverNames[i]);
                break;
            }
        }
    }
}
