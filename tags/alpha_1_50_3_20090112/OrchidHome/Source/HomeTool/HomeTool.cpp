/*******************************************************************************
* 软件名称：Orchid Mobile Software Home Plugin
* 文件名称：插件安装调试工具
* 文件作者：Zhou JingYu (zjy@cnpack.org)
* 单元标识：$Id: HomeTool.cpp,v 1.6 2006/10/24 03:58:00 Administrator Exp $
* 备    注：
*******************************************************************************/

#include <windows.h>
#include <shellapi.h>
#include <cfgmgrapi.h>

// 安装主题文件的配置XML
wchar_t* sSchemeXml =
  L"<wap-provisioningdoc>"
  L"<characteristic type=\"Home\">"
  L"<characteristic type=\"HKCU\\ControlPanel\\Home\">"
  L"<parm name=\"Scheme\" value=\"%s\" translation=\"install\"/>"
  L"</characteristic>"
  L"</characteristic>"
  L"</wap-provisioningdoc>";

bool SetCurrentScheme(TCHAR* file)
{
  HRESULT hr;
  TCHAR* sout = NULL;
  TCHAR buff[4096];
  wsprintf(buff, sSchemeXml, file);
  while (FAILED(hr = DMProcessConfigXML(buff, CFGFLAG_PROCESS, &sout)))
  {
    static int cnt = 0;
    delete sout;
    sout = NULL;
    if (++cnt > 10)
      return false;
    Sleep(1000);
  }
  return true;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
  SHELLEXECUTEINFO info;

  // 以后台模式安装主题 cab 文件
  memset(&info, 0, sizeof(info));
  info.cbSize = sizeof(info);
  info.lpVerb = L"Open";
  info.lpFile = L"wceload.exe";
  info.lpParameters = L"\\Storage\\OrchidHome.cab /silent";
  info.nShow = SW_SHOW;
  ShellExecuteEx(&info);

  // 等待安装结束
  WaitForSingleObject(info.hProcess, INFINITE);

  SetCurrentScheme(L"%CE19%\\Home\\Orchid.home.xml");
}
