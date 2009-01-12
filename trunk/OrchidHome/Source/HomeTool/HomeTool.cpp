/*******************************************************************************
* ������ƣ�Orchid Mobile Software Home Plugin
* �ļ����ƣ������װ���Թ���
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: HomeTool.cpp,v 1.6 2006/10/24 03:58:00 Administrator Exp $
* ��    ע��
*******************************************************************************/

#include <windows.h>
#include <shellapi.h>
#include <cfgmgrapi.h>

// ��װ�����ļ�������XML
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

  // �Ժ�̨ģʽ��װ���� cab �ļ�
  memset(&info, 0, sizeof(info));
  info.cbSize = sizeof(info);
  info.lpVerb = L"Open";
  info.lpFile = L"wceload.exe";
  info.lpParameters = L"\\Storage\\OrchidHome.cab /silent";
  info.nShow = SW_SHOW;
  ShellExecuteEx(&info);

  // �ȴ���װ����
  WaitForSingleObject(info.hProcess, INFINITE);

  SetCurrentScheme(L"%CE19%\\Home\\Orchid.home.xml");
}
