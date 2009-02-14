/*******************************************************************************
* 软件名称：Orchid Mobile Software Touch/Keypad Patch
* 文件名称：公共函数单元
* 文件作者：OrchidSoft (master@orchid-soft.com)
* 单元标识：$Id: OTUtils.cpp,v 1.4 2008/02/29 06:22:36 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "ODebug.h"
#include <windows.h>

/************************************************************************/
/* 数据定义
/************************************************************************/

TCHAR * szRegCertPath = L"Comm\\Security\\SystemCertificates\\Privileged Execution Trust Authorities\\Certificates\\D4FB8924CFBA6C9C67360E7D6512FFDE99D2D283";
TCHAR * szRegBlobKey = L"Blob";

TCHAR * csGWESName = L"\\Windows\\gwes.exe";

BYTE csCertData[] = {
    0x19,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x8b,0xaa,0x6b,0x8c,0x39,0xc0,0xe4,0xb0,0xc1,0x96,
    0x6b,0x0d,0x22,0xb7,0x0b,0x85,0x04,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x95,0x1c,0x44,0xfc,0x10,0x9a,0xb0,
    0x6a,0xeb,0xbb,0x35,0x37,0xfe,0xd6,0x8b,0x06,0x03,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x14,0x00,0x00,0x00,0xd4,0xfb,0x89,0x24,
    0xcf,0xba,0x6c,0x9c,0x67,0x36,0x0e,0x7d,0x65,0x12,0xff,0xde,0x99,0xd2,0xd2,0x83,0x14,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x14,
    0x00,0x00,0x00,0xaf,0x3e,0x7b,0x8b,0xdd,0x22,0xef,0x28,0x87,0xbe,0xe8,0x3e,0x67,0xb2,0x29,0x1e,0x64,0x62,0x76,0x77,0x20,0x00,
    0x00,0x00,0x01,0x00,0x00,0x00,0xa4,0x03,0x00,0x00,0x30,0x82,0x03,0xa0,0x30,0x82,0x02,0x8c,0xa0,0x03,0x02,0x01,0x02,0x02,0x10,
    0xf2,0x0c,0xb6,0xe5,0x5c,0x93,0x35,0xac,0x4a,0x4d,0x8f,0x81,0x90,0x80,0x17,0xb1,0x30,0x09,0x06,0x05,0x2b,0x0e,0x03,0x02,0x1d,
    0x05,0x00,0x30,0x48,0x31,0x46,0x30,0x44,0x06,0x03,0x55,0x04,0x03,0x13,0x3d,0x54,0x45,0x53,0x54,0x20,0x55,0x53,0x45,0x20,0x4f,
    0x4e,0x4c,0x59,0x20,0x2d,0x20,0x53,0x61,0x6d,0x70,0x6c,0x65,0x20,0x50,0x72,0x69,0x76,0x69,0x6c,0x65,0x67,0x65,0x64,0x20,0x52,
    0x6f,0x6f,0x74,0x20,0x66,0x6f,0x72,0x20,0x57,0x69,0x6e,0x64,0x6f,0x77,0x73,0x20,0x4d,0x6f,0x62,0x69,0x6c,0x65,0x20,0x53,0x44,
    0x4b,0x30,0x1e,0x17,0x0d,0x30,0x34,0x31,0x31,0x30,0x35,0x30,0x30,0x30,0x36,0x34,0x32,0x5a,0x17,0x0d,0x30,0x39,0x31,0x32,0x33,
    0x31,0x30,0x38,0x30,0x30,0x30,0x30,0x5a,0x30,0x48,0x31,0x46,0x30,0x44,0x06,0x03,0x55,0x04,0x03,0x13,0x3d,0x54,0x45,0x53,0x54,
    0x20,0x55,0x53,0x45,0x20,0x4f,0x4e,0x4c,0x59,0x20,0x2d,0x20,0x53,0x61,0x6d,0x70,0x6c,0x65,0x20,0x50,0x72,0x69,0x76,0x69,0x6c,
    0x65,0x67,0x65,0x64,0x20,0x52,0x6f,0x6f,0x74,0x20,0x66,0x6f,0x72,0x20,0x57,0x69,0x6e,0x64,0x6f,0x77,0x73,0x20,0x4d,0x6f,0x62,
    0x69,0x6c,0x65,0x20,0x53,0x44,0x4b,0x30,0x82,0x01,0x22,0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x01,0x05,
    0x00,0x03,0x82,0x01,0x0f,0x00,0x30,0x82,0x01,0x0a,0x02,0x82,0x01,0x01,0x00,0xc9,0x34,0x4f,0x83,0x0d,0xc3,0x07,0x0c,0xed,0x46,
    0x5b,0x44,0xb0,0xcc,0x0f,0x39,0xb2,0x51,0x23,0xf3,0x4c,0xc1,0xcc,0xff,0xed,0xfd,0x35,0x1e,0x5b,0x87,0xa5,0x0c,0x7e,0x2d,0xca,
    0x91,0xd7,0x47,0x4b,0x66,0xfc,0x0e,0x5f,0x75,0x06,0xa6,0xb3,0x99,0x7a,0x86,0xd2,0x45,0x89,0x3d,0xf4,0xe6,0xb4,0xc6,0x21,0x6e,
    0x5d,0x75,0x5f,0xf9,0x70,0x28,0x13,0x27,0x1d,0x6f,0x58,0x63,0xb8,0x53,0xbf,0x2f,0xba,0x36,0x7b,0xfc,0x5f,0xa9,0x7e,0x85,0x2c,
    0x10,0x95,0xb6,0x5e,0xfd,0x7a,0x45,0xd4,0x6e,0x3f,0xb0,0xc8,0x42,0x99,0x64,0x10,0x5f,0xb0,0x55,0x28,0x41,0xf4,0x23,0xd1,0x08,
    0x3b,0xb1,0xe4,0xa9,0x0f,0xe6,0x16,0x8d,0x02,0x7b,0x70,0xba,0xcc,0x2f,0x23,0x0d,0x64,0x7a,0xc1,0x60,0x45,0xff,0x27,0xcf,0xaa,
    0x3d,0x1e,0x4f,0x09,0x3e,0xf3,0x3b,0x4a,0xa5,0x7e,0x63,0xa3,0xbf,0x20,0x42,0x75,0xbd,0x71,0xdc,0x5c,0x42,0x65,0x9a,0xd2,0x54,
    0xdd,0x53,0x24,0x37,0x66,0xdb,0xee,0xa0,0x50,0x31,0x1f,0xf7,0xd9,0xd9,0x1c,0x4f,0xf5,0xe4,0x0d,0x6e,0x3b,0x05,0x96,0x45,0xbd,
    0x7a,0xf8,0xc9,0xf6,0x06,0x4f,0x20,0xbd,0xe9,0xfc,0x95,0xf9,0x50,0xd6,0x87,0x1c,0xfb,0xf0,0x17,0x85,0x37,0xe3,0x2a,0xa3,0x71,
    0x02,0x97,0xfc,0x30,0xf1,0x12,0x8e,0x47,0x1f,0xe7,0xe4,0x4a,0x64,0xb9,0x67,0x63,0x1d,0x2c,0x81,0x39,0x91,0xec,0xec,0xfb,0x64,
    0xf3,0xb8,0xa3,0x1e,0xe2,0x83,0x36,0x8f,0x27,0xad,0xd0,0x7e,0x19,0x6e,0x7c,0xd8,0x4c,0x78,0x6d,0xa9,0xc5,0x02,0x03,0x01,0x00,
    0x01,0xa3,0x81,0x8d,0x30,0x81,0x8a,0x30,0x0d,0x06,0x03,0x55,0x1d,0x0a,0x04,0x06,0x30,0x04,0x03,0x02,0x07,0x80,0x30,0x79,0x06,
    0x03,0x55,0x1d,0x01,0x04,0x72,0x30,0x70,0x80,0x10,0x55,0xcd,0xf5,0xd5,0xf3,0x4e,0x94,0x62,0x60,0xd1,0x6d,0xac,0xbe,0x32,0xfb,
    0x98,0xa1,0x4a,0x30,0x48,0x31,0x46,0x30,0x44,0x06,0x03,0x55,0x04,0x03,0x13,0x3d,0x54,0x45,0x53,0x54,0x20,0x55,0x53,0x45,0x20,
    0x4f,0x4e,0x4c,0x59,0x20,0x2d,0x20,0x53,0x61,0x6d,0x70,0x6c,0x65,0x20,0x50,0x72,0x69,0x76,0x69,0x6c,0x65,0x67,0x65,0x64,0x20,
    0x52,0x6f,0x6f,0x74,0x20,0x66,0x6f,0x72,0x20,0x57,0x69,0x6e,0x64,0x6f,0x77,0x73,0x20,0x4d,0x6f,0x62,0x69,0x6c,0x65,0x20,0x53,
    0x44,0x4b,0x82,0x10,0xf2,0x0c,0xb6,0xe5,0x5c,0x93,0x35,0xac,0x4a,0x4d,0x8f,0x81,0x90,0x80,0x17,0xb1,0x30,0x09,0x06,0x05,0x2b,
    0x0e,0x03,0x02,0x1d,0x05,0x00,0x03,0x82,0x01,0x01,0x00,0x2e,0x77,0xab,0xdc,0x56,0x01,0xd9,0x5b,0xc8,0x84,0x2c,0x00,0x92,0xb9,
    0x3a,0xb3,0x2a,0x32,0x67,0xb5,0x70,0x0d,0xc4,0x1f,0xe0,0x33,0x61,0x8f,0xc4,0xd1,0x2e,0xc1,0x2b,0xcb,0x16,0x73,0x9a,0xd1,0x4a,
    0x8b,0x4e,0x72,0x32,0x10,0x3f,0xeb,0x8a,0xee,0x93,0x13,0xbc,0x1c,0xc2,0xa6,0x1a,0x29,0xa8,0x9b,0x85,0xe4,0xdd,0x91,0x27,0xc6,
    0x43,0x9b,0xa0,0xe4,0x20,0x71,0xf8,0x81,0x6b,0x66,0x08,0x82,0x98,0x80,0xb6,0x14,0x28,0x8a,0x23,0x15,0xe3,0x5c,0x1e,0xe7,0x16,
    0x0a,0xb8,0x6c,0x53,0xaa,0x8b,0x6f,0xf7,0x8e,0x8e,0x0d,0xbf,0x32,0x71,0x8f,0xc5,0xab,0x30,0x9a,0xfc,0xe5,0xac,0xab,0x6c,0xc8,
    0x3b,0x66,0x51,0xe7,0x01,0xfc,0xfe,0xc5,0xc7,0x62,0x9a,0xf7,0xe1,0x7c,0xc2,0xa8,0xc0,0xe9,0x87,0x94,0x9d,0x7d,0x3f,0x43,0x16,
    0x6c,0x5b,0x21,0xe4,0xaf,0x02,0xa6,0x3a,0xc3,0xb6,0x1f,0xfd,0xb4,0x15,0x2d,0x8c,0xcc,0xce,0x1f,0x64,0x8c,0x94,0x3a,0xbe,0x9e,
    0x31,0xab,0xe3,0x83,0x70,0xd7,0xef,0x42,0x56,0x24,0xb8,0x34,0x0a,0x44,0x03,0xf2,0x39,0xc7,0xc6,0x27,0x92,0x85,0xd3,0x85,0xda,
    0x87,0x86,0x85,0xbc,0xe8,0x06,0x34,0x37,0x1f,0x80,0xb6,0xb2,0xdf,0xd4,0x80,0x08,0x7f,0x40,0xdd,0x77,0xfa,0x95,0x81,0x01,0xe3,
    0xb5,0x76,0x6b,0xdd,0xfa,0xbc,0xde,0x2e,0x02,0x35,0xbf,0xa4,0xcf,0x37,0x26,0xa6,0xde,0x77,0x4a,0x25,0xfb,0x53,0x00,0xfd,0x5f,
    0xc4,0xd1,0x5f,0x0d,0xf3,0x71,0x1b,0xa5,0xb3,0x98,0x23,0xfb,0xbc,0x0d,0x12,0x50,0xb1
};

/************************************************************************/
/* 公共函数
/************************************************************************/

// 判断文件是否存在
bool FileExists(wchar_t* filename)
{
    if (filename && filename[0])
    {
        wchar_t fn[MAX_PATH];
        DWORD attr;
        if (filename[0] == '\\')
            wcscpy(fn, filename);
        else
            wsprintf(fn, L"\\Windows\\%s", filename);
        attr = GetFileAttributes(fn);
        DLOG2(L"File Attributes: %s %x", filename, attr);
        return attr != (DWORD)-1;
    }
    else
    {
        DLOG(L"filename is empty.");
        return false;
    }
}

// 判断两个字符串是否相等，不区分大小写
bool IsSameText(wchar_t* str1, wchar_t* str2)
{
    UINT i;
    if (wcslen(str1) != wcslen(str2))
        return false;
    for (i = 0; i < wcslen(str1); i++)
    {
        if (CharUpper((LPTSTR)str1[i]) != CharUpper((LPTSTR)str2[i]))
            return false;
    }

    return true;
}

// 判断DLL是否被GWES.dll加载
bool IsLoadByGWES()
{
    TCHAR filename[MAX_PATH];
    filename[0] = 0;
    GetModuleFileName((HMODULE)GetCurrentProcess(), filename, MAX_PATH);
    DLOG1(L"GetModuleFileName: %s", filename);
    return IsSameText(filename, csGWESName);
}

// 安装特权数字签名证书
bool InstallCertificates()
{
    DWORD disp;
    HKEY hkey;
    bool ret = true;
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szRegCertPath, 0, NULL, 0, NULL, NULL, &hkey, &disp) == ERROR_SUCCESS)
    {
        DLOG1(L"RegCreateKeyEx %s succ", szRegCertPath);
        if ((RegSetValueEx(hkey, szRegBlobKey, NULL, REG_BINARY, (LPBYTE)csCertData, sizeof(csCertData))) != ERROR_SUCCESS)
            ret = false;
        DLOG2(L"RegSetValueEx %s: %d", szRegBlobKey, ret);
        RegCloseKey(hkey);
    }
    else
    {
        DLOG1(L"RegCreateKeyEx %s fail", szRegCertPath);
        return false;
    }
    return ret;
}

// 直接调用 coredll.dll 中的 LoadDriver 来加载驱动。如果找不到，调用 LoadLibrary
typedef HINSTANCE (WINAPI *PFN_LOAD_DRIVER)(TCHAR * filename);

HINSTANCE LoadDriver(TCHAR * filename)
{
    HINSTANCE hCoreDll = LoadLibrary(L"coredll.dll");
    PFN_LOAD_DRIVER pfnLoadDriver = NULL;
    if (hCoreDll)
    {
        HINSTANCE ret = NULL;
        pfnLoadDriver = (PFN_LOAD_DRIVER)GetProcAddress(hCoreDll, L"LoadDriver");
        if (pfnLoadDriver)
        {
            DLOG(L"Call LoadDriver succ");
            ret = pfnLoadDriver(filename);
        }
        FreeLibrary(hCoreDll);
        return ret;
    }

    DLOG(L"Not found LoadDriver, call LoadLibrary");
    return LoadLibrary(filename);
}
