/*******************************************************************************
* 软件名称：Orchid Mobile Software Touch/Keypad Patch
* 文件名称：配置单元
* 文件作者：OrchidSoft (master@orchid-soft.com)
* 单元标识：$Id: OTConfig.cpp,v 1.7 2008/02/29 06:22:35 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "ODebug.h"
#include "OTConfig.h"
#include "OTUtils.h"
#include "OTComExport.h"
#include <windows.h>

/************************************************************************/
/* 配置参数
/************************************************************************/

// 两次点击最小间隔
UINT g_Interval = DEF_Interval;

// 第二次点击最小延时
INT  g_Delay = DEF_Delay;

// 相邻两次点击最小距离
INT  g_AreaX = DEF_AreaX;
INT  g_AreaY = DEF_AreaY;

// 旧驱动名称
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
    L"hwTouch.dll",  // 全屏手写驱动
    L"touch.dll",    // 系统原始驱动
    NULL
};

/************************************************************************/
/* 安装/反安装处理 
/************************************************************************/

#define REG_CHECK(fn)  if (fn != ERROR_SUCCESS) ret = false;
#define WSTR_SIZE(str) (wcslen(str) * sizeof(wchar_t))
#define REG_WRITE_INT(key, value) RegSetValueEx(hkey, key, NULL, REG_DWORD, (LPBYTE)&value, sizeof(DWORD));

// 写入初始化设置
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
                // 原驱动存在时才备份
                REG_CHECK(RegSetValueEx(hkey, szRegOldDriverKey, NULL, REG_SZ, (LPBYTE)filename, WSTR_SIZE(filename)));
                DLOG3(L"RegSetValueEx %s: %s %x", szRegOldDriverKey, filename, ret);
            }
            REG_CHECK(RegSetValueEx(hkey, szRegDriverKey, NULL, REG_SZ, (LPBYTE)szDriverDllName, WSTR_SIZE(szDriverDllName)));
            DLOG3(L"RegSetValueEx %s: %s %x", szRegDriverKey, szDriverDllName, ret);
        }

        // 写入初始配置
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

// 删除初始化设置
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
            // 旧驱动存在时恢复
            REG_CHECK(RegSetValueEx(hkey, szRegDriverKey, NULL, REG_SZ, (LPBYTE)filename, WSTR_SIZE(filename)));
        }
        else
        {
            // 旧驱动不存在时删除驱动项，系统使用默认驱动
            REG_CHECK(RegDeleteValue(hkey, szRegDriverKey));
        }

        // 删除配置
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
/* 加载配置参数
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

    // 读取参数
    memset(g_OldDriverName, 0, sizeof(g_OldDriverName));
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szRegTouchPath, 0, NULL, 0, NULL, NULL, &hkey, &disp) == ERROR_SUCCESS)
    {
        DWORD type, dwsize = MAX_PATH;

        // 从注册表中读取原驱动名称
        RegQueryValueEx(hkey, szRegOldDriverKey, NULL, &type, (LPBYTE)g_OldDriverName, &dwsize);
        DLOG1(L"OldDriverName: %s", g_OldDriverName);

        RegReadInt(hkey, szRegIntervalKey, (int*)&g_Interval);
        RegReadInt(hkey, szRegDelayKey, &g_Delay);
        RegReadInt(hkey, szRegAreaXKey, &g_AreaX);
        RegReadInt(hkey, szRegAreaYKey, &g_AreaY);

        RegCloseKey(hkey);
    }

    // 如果注册表中的驱动不存在，查找默认驱动
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
