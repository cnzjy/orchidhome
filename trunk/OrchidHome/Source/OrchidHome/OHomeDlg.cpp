/*******************************************************************************
* ������ƣ�Orchid Mobile Software Home Plugin
* �ļ����ƣ�ϵͳ��Ϣ��Ԫ
* �ļ����ߣ�Zhou JingYu (zjy@cnpack.org)
* ��Ԫ��ʶ��$Id: OHomeDlg.cpp,v 1.32 2007/01/31 14:03:47 Administrator Exp $
* ��    ע���õ�Ԫ�Ĳ������ݲο��� CeleTask 1.7 �Ĵ���
*******************************************************************************/

#include <aygshell.h>
#include <tpcshell.h>
#include "OHomeData.h"
#include "OHomeDlg.h"
#include "OSysInfo.h"

#define INVALID_INT       0x7FFFFFFF

#define colorTitle        COLOR_BLACK
#define colorBackgnd      COLOR_WHITE
#define colorBackText     0xE0E0E0
#define colorHighlight    0xFFB0B0
#define colorHightBorder  0xCC9090
#define colorNormal       COLOR_BLACK
#define colorWeekend      COLOR_RED
#define colorLunar        COLOR_RED
#define colorSolar        COLOR_BLUE
#define colorMonth        COLOR_GREEN
#define colorJieQi        COLOR_PURPLE
#define colorBottom       COLOR_BLUE
#define colorHoliday      COLOR_GREEN

static wchar_t* sMainDlg = L"MainDlg";
static wchar_t* sCurrPage = L"CurrPage";

/*******************************************************************************
* ����ʱ�䴦����
*******************************************************************************/

// ����ת��
void ODateTimeToSystemTime(ODATE* date, OTIME* time, SYSTEMTIME* stime)
{
  if (!stime) return;

  if (date && GetDateIsValid(date->year, date->month, date->day))
  {
    stime->wYear = date->year;
    stime->wMonth = date->month;
    stime->wDay = date->day;
    stime->wDayOfWeek = GetWeek(date->year, date->month, date->day);
  }

  if (time && GetTimeIsValid(time->hour, time->minute, time->second))
  {
    stime->wHour = time->hour;
    stime->wMinute = time->minute;
    stime->wSecond = time->second;
    stime->wMilliseconds = 0;
  }
}

// ����ת��
void SystemTimeToODateTime(SYSTEMTIME* stime, ODATE* date, OTIME* time)
{
  if (!stime) return;

  if (date && GetDateIsValid(stime->wYear, stime->wMonth, stime->wDay))
  {
    date->year = stime->wYear;
    date->month = stime->wMonth;
    date->day = stime->wDay;
  }

  if (time && GetTimeIsValid(stime->wHour, stime->wMinute, stime->wSecond))
  {
    time->hour = stime->wHour;
    time->minute = stime->wMinute;
    time->second = stime->wSecond;
  }
}

/************************************************************************/
/* ���ձ༭�Ի���                                                       */
/************************************************************************/

static CHoliday* g_holiday = NULL;
static bool g_day_mod = false;
static bool g_addother = false;

static void UserDlg_UpdateFromHolidayType(HWND hwnd, int type)
{
  bool isWeek = type == htWeek;
  bool hasYear = type != htWeek && type != htSolar && type != htLunar;
  SetControlVisible(hwnd, IDC_EDITYEAR, !isWeek);
  SetControlVisible(hwnd, IDC_EDITMONTH, !isWeek);
  SetControlVisible(hwnd, IDC_EDITDAY, !isWeek);
  SetControlVisible(hwnd, IDC_TEXTYEAR, !isWeek);
  SetControlVisible(hwnd, IDC_TEXTMONTH, !isWeek);
  SetControlVisible(hwnd, IDC_TEXTDAY, !isWeek);
  SetControlVisible(hwnd, IDC_EDITWEEKMONTH, isWeek);
  SetControlVisible(hwnd, IDC_EDITWEEK, isWeek);
  SetControlVisible(hwnd, IDC_EDITWEEKDAY, isWeek);
  SetControlVisible(hwnd, IDC_TEXTWEEKMONTH, isWeek);
  SetControlVisible(hwnd, IDC_TEXTWEEK, isWeek);
  SetControlEnabled(hwnd, IDC_EDITYEAR, hasYear);
  SetControlEnabled(hwnd, IDC_CHECKADDDAY, !isWeek && type != htLunar && type != htSolar);
}

static CHoliday* UserDlg_AddOtherHoliday(CHoliday* holiday)
{
  ODATE date;
  OLUNAR lunar;
  CHoliday* ret;
  switch(holiday->data.type)
  {
  case htSolarBirth:
  case htSolarDay:
    date.year = holiday->data.year;
    date.month = holiday->data.month;
    date.day = holiday->data.day;
    if (GetLunarInfo(&date, &lunar))
    {
      ret = new CHoliday;
      ret->data.type = holiday->data.type == htSolarBirth ? htLunarBirth : htLunarDay;
      ret->data.level = holiday->data.level;
      ret->data.year = lunar.year;
      ret->data.month = lunar.month;
      ret->data.day = lunar.day;
      CloneString(ret->text, holiday->text);
      CloneString(ret->stext, holiday->stext);
      return ret;
    }
    break;
  case htLunarBirth:
  case htLunarDay:
    lunar.year = holiday->data.year;
    lunar.month = holiday->data.month;
    lunar.day = holiday->data.day;
    lunar.leap = 0;
    lunar.little = 0;
    if (LunarToDate(&lunar, &date))
    {
      ret = new CHoliday;
      ret->data.type = holiday->data.type == htLunarBirth ? htSolarBirth : htSolarDay;
      ret->data.level = holiday->data.level;
      ret->data.year = date.year;
      ret->data.month = date.month;
      ret->data.day = date.day;
      CloneString(ret->text, holiday->text);
      CloneString(ret->stext, holiday->stext);
      return ret;
    }
    break;
  }
  return NULL;
}

static void UserDlg_InitDialog(HWND hwnd)
{
  // ��ʼ�������б�
  InitSpinBox(hRes, hwnd, IDC_LISTHOLITYPE, IDS_LD_SOLAR, IDS_LD_SOLAR + MAX_HOLIDAY, g_holiday->data.type);

  // ��ʼ����������
  SetStrValueToEdit(hwnd, IDC_EDITHOLINAME, g_holiday->text);
  SetStrValueToEdit(hwnd, IDC_EDITHOLISHORT, g_holiday->stext);
  SetIntValueToEdit(hwnd, IDC_EDITYEAR, g_holiday->data.year);
  SetIntValueToEdit(hwnd, IDC_EDITMONTH, g_holiday->data.month);
  SetIntValueToEdit(hwnd, IDC_EDITDAY, g_holiday->data.day);
  SetIntValueToEdit(hwnd, IDC_EDITWEEKMONTH, g_holiday->data.month);
  SetIntValueToEdit(hwnd, IDC_EDITWEEK, g_holiday->data.week);
  SetIntValueToEdit(hwnd, IDC_EDITWEEKDAY, g_holiday->data.weekday);

  // �����������ÿؼ�������
  UserDlg_UpdateFromHolidayType(hwnd, g_holiday->data.type);
}

static bool UserDlg_SetHolidayFromControls(HWND hwnd)
{
  int y, m, d, wm, w, wd;
  wchar_t buff[TEXT_LEN];
  int type = SendMessage(GetDlgItem(hwnd, IDC_LISTHOLITYPE), LB_GETCURSEL, 0, 0);

  // ȡ������
  y = GetIntValueFromEdit(hwnd, IDC_EDITYEAR);
  m = GetIntValueFromEdit(hwnd, IDC_EDITMONTH);
  d = GetIntValueFromEdit(hwnd, IDC_EDITDAY);
  wm = GetIntValueFromEdit(hwnd, IDC_EDITWEEKMONTH);
  w = GetIntValueFromEdit(hwnd, IDC_EDITWEEK);
  wd = GetIntValueFromEdit(hwnd, IDC_EDITWEEKDAY);
  if (wd == 7) wd = 0;
  switch(type)
  {
  case htSolar:
  case htSolarBirth:
  case htSolarDay:
    if (GetDateIsValid(type == htSolar ? 2000 : y, m, d))
      break;
    else
    {
      ShowMsgBoxRes(hRes, hwnd, 0, IDS_MSG_INVALIDDATE, MB_OK | MB_ICONERROR);
      return false;
    }
  case htLunar:
  case htLunarBirth:
  case htLunarDay:
    ODATE date;
    OLUNAR lunar;
    lunar.year = type == htLunar ? 2000 : y;
    lunar.month = m;
    lunar.day = d;
    if (LunarToDate(&lunar, &date))
      break;
    else
    {
      ShowMsgBoxRes(hRes, hwnd, 0, IDS_MSG_INVALIDDATE, MB_OK | MB_ICONERROR);
      return false;
    }
  case htWeek:
    if (wm > 0 && wm <= 12 && w > 0 && w <= 5 && wd >= 0 && wd < 7)
      break;
    else
    {
      ShowMsgBoxRes(hRes, hwnd, 0, IDS_MSG_INVALIDDATE, MB_OK | MB_ICONERROR);
      return false;
    }
  default:
    return false;
  }

  // ȡ����
  SendMessage(GetDlgItem(hwnd, IDC_EDITHOLINAME), WM_GETTEXT, LENOF(buff), (LPARAM)buff);
  if (buff[0] == 0)
  {
    ShowMsgBoxRes(hRes, hwnd, 0, IDS_MSG_NAMEISEMPTY, MB_OK | MB_ICONERROR);
    return false;
  }
  else
    CloneString(g_holiday->text, buff);

  // ȡ��д
  SendMessage(GetDlgItem(hwnd, IDC_EDITHOLISHORT), WM_GETTEXT, LENOF(buff), (LPARAM)buff);
  CloneString(g_holiday->stext, buff);

  // ��������
  g_holiday->data.type = type;
  g_holiday->data.level = 0;

  // ��������
  switch(type)
  {
  case htSolar:
  case htSolarBirth:
  case htSolarDay:
  case htLunar:
  case htLunarBirth:
  case htLunarDay:
    g_holiday->data.year = (type == htSolar || type == htLunar) ? 0 : y;
    g_holiday->data.month = m;
    g_holiday->data.day = d;
    break;
  case htWeek:
    g_holiday->data.month = wm;
    g_holiday->data.week = w;
    g_holiday->data.weekday = wd;
    break;
  }

  g_addother = GetCheckBoxChecked(hwnd, IDC_CHECKADDDAY);
  return true;
}

BOOL CALLBACK UserDlg_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message){
    case WM_INITDIALOG:
      {
        wchar_t title[TITLE_LEN];

        InitializeDialog(hRes, hwnd, 0, IDR_MENU_OKCANCEL, true);

        HomeData->sysinfo->GetDlgTitle(title, TITLE_LEN, IDS_TI_USERDAY);
        SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)title);

        UserDlg_InitDialog(hwnd);

        return TRUE;
      }

    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
      case IDOK:
        if (UserDlg_SetHolidayFromControls(hwnd))
        {
          g_day_mod = true;
          EndDialog(hwnd, LOWORD(wParam));
        }
        return TRUE;

      case IDCANCEL:
        g_day_mod = false;
        EndDialog(hwnd, LOWORD(wParam));
        return TRUE;

      case IDC_LISTHOLITYPE:
        if (HIWORD(wParam) == LBN_SELCHANGE)
        {
          int type = SendMessage(GetDlgItem(hwnd, IDC_LISTHOLITYPE), LB_GETCURSEL, 0, 0);
          UserDlg_UpdateFromHolidayType(hwnd, type);
        }
        return TRUE;
      }
      return FALSE;

    case WM_NOTIFY:
      break;

    case WM_HOTKEY:
      if (VK_TBACK == HIWORD(lParam))
        SHSendBackToFocusWindow(message, wParam, lParam);
      break;
  }
  return FALSE;
}

/************************************************************************/
/* ���ӱ༭�Ի���                                                       */
/************************************************************************/

static CAlarm* g_alarm = NULL;
static bool g_alarm_mod = false;

static void UpdateControlsFromAlarmType(HWND hwnd, int type)
{
  bool hasEndTime = (type == atFlightMode) || (type == atSilentMode) || (type == atChime);
  bool hasSnd = (type == atAlarm) || (type == atChime);
  bool hasTarget = type == atExecute;
  SetControlVisible(hwnd, IDC_ALARMTIME2, hasEndTime);
  SetControlVisible(hwnd, IDC_LBLENDTIME, hasEndTime);
  SetControlEnabled(hwnd, IDC_EDITALARMTARGET, hasTarget);
  SetControlEnabled(hwnd, IDC_LISTALARMMODE, hasSnd);
  SetControlEnabled(hwnd, IDC_LISTSNDFILE, hasSnd);
  SetControlEnabled(hwnd, IDC_LISTVOLUME, hasSnd);
  SetControlEnabled(hwnd, IDC_LISTRINGCOUNT, hasSnd);
}

static void UpdateControlsFromRecurType(HWND hwnd, int type)
{
  bool isWeekly = type == arWeekly;
  bool isMonthly = type == arMonthly;
  bool isAnnually = type == arAnnually;
  bool isInterval = type == arInterval;
  SetControlVisible(hwnd, IDC_LBLALARMWEEK, isWeekly);
  for (int i = IDC_CHECKWEEK1; i <= IDC_CHECKWEEK7; i++)
    SetControlVisible(hwnd, i, isWeekly);
  SetControlVisible(hwnd, IDC_LBLALARMMONTH, isMonthly);
  SetControlVisible(hwnd, IDC_LBLALARMANNUALLY, isAnnually);
  SetControlVisible(hwnd, IDC_LBLALARMMONTH1, isMonthly || isAnnually);
  SetControlVisible(hwnd, IDC_LBLALARMMONTH2, isMonthly || isAnnually);
  SetControlVisible(hwnd, IDC_EDITALARMMONTH, isMonthly || isAnnually);
  SetControlVisible(hwnd, IDC_EDITALARMDAY, isMonthly || isAnnually);
  SetControlEnabled(hwnd, IDC_EDITALARMMONTH, isAnnually);
  SetControlVisible(hwnd, IDC_LBLALARMINTERVAL, isInterval);
  SetControlVisible(hwnd, IDC_LBLALARMINTERVAL1, isInterval);
  SetControlVisible(hwnd, IDC_EDITALARMINTERVAL, isInterval);
  // todo: �������öԻ���߶�
}

static void InitEditAlarmDlg(HWND hwnd)
{
  // ��ʼ�������б�
  InitSpinBox(hRes, hwnd, IDC_LISTALARMTYPE, IDS_LT_ALARM, IDS_LT_ALARM + MAX_ALARMTYPE, g_alarm->data.alarmType);
  InitSpinBox(hRes, hwnd, IDC_LISTALARMMODE, IDS_LT_RING, IDS_LT_RING + MAX_ALARMMODE, g_alarm->data.alarmMode);
  InitSpinBox(hRes, hwnd, IDC_LISTRECURTYPE, IDS_LT_ONCE, IDS_LT_ONCE + MAX_RECURTYPE, g_alarm->data.recurInfo.type);
  InitSpinBox(hRes, hwnd, IDC_LISTSNDFILE, IDS_LT_SNDFILEDEF, IDS_LT_SNDFILEDEF, 0);
  InitSpinBox(hRes, hwnd, IDC_LISTVOLUME, IDS_LT_VOLUMEDEF, IDS_LT_VOLUME5, g_alarm->data.volume);
  for (int i = 0; i < 10; i++)
  {
    wchar_t buff[8];
    wsprintf(buff, L"%d", i);
    SendMessage(GetDlgItem(hwnd, IDC_LISTRINGCOUNT), LB_ADDSTRING, 0, (LPARAM)buff);
  }
  InitSpinBox(hRes, hwnd, IDC_LISTRINGCOUNT, 0, 0, g_alarm->data.ringCount);

  DateTime_SetFormat(GetDlgItem(hwnd, IDC_ALARMTIME), L"H:mm");
  DateTime_SetFormat(GetDlgItem(hwnd, IDC_ALARMTIME2), L"H:mm");

  // ��ʼ����������
  //SetStrValueToEdit(hwnd, IDC_EDITHOLINAME, g_alarm->text);
  //SetIntValueToEdit(hwnd, IDC_EDITYEAR, g_alarm->data.year);

  //DateTime_SetSystemtime

  // �����������ÿؼ�������
  UpdateControlsFromAlarmType(hwnd, g_alarm->data.alarmType);
  UpdateControlsFromRecurType(hwnd, g_alarm->data.recurInfo.type);
}

static bool SetAlarmFromControls(HWND hwnd)
{
  return true;
}

BOOL CALLBACK EditAlarm_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message){
    case WM_INITDIALOG:
      {
        wchar_t title[TITLE_LEN];

        InitializeDialog(hRes, hwnd, 0, IDR_MENU_OKCANCEL, true);

        HomeData->sysinfo->GetDlgTitle(title, TITLE_LEN, IDS_CAP_ALARMS);
        SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)title);

        InitEditAlarmDlg(hwnd);

        return TRUE;
      }

    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
      case IDOK:
        if (SetAlarmFromControls(hwnd))
        {
          g_alarm_mod = true;
          EndDialog(hwnd, LOWORD(wParam));
        }
        return TRUE;

      case IDCANCEL:
        g_alarm_mod = false;
        EndDialog(hwnd, LOWORD(wParam));
        return TRUE;

      case IDC_LISTALARMTYPE:
        if (HIWORD(wParam) == LBN_SELCHANGE)
        {
          int type = SendMessage(GetDlgItem(hwnd, IDC_LISTALARMTYPE), LB_GETCURSEL, 0, 0);
          UpdateControlsFromAlarmType(hwnd, type);
        }
        return TRUE;

      case IDC_LISTRECURTYPE:
        if (HIWORD(wParam) == LBN_SELCHANGE)
        {
          int type = SendMessage(GetDlgItem(hwnd, IDC_LISTRECURTYPE), LB_GETCURSEL, 0, 0);
          UpdateControlsFromRecurType(hwnd, type);
        }
        return TRUE;
      }
      return FALSE;

    case WM_NOTIFY:
      break;

    case WM_HOTKEY:
      if (VK_TBACK == HIWORD(lParam))
        SHSendBackToFocusWindow(message, wParam, lParam);
      break;
  }
  return FALSE;
}

/************************************************************************/
/* ���öԻ���                                                           */
/************************************************************************/

static void OptionDlg_UpdateFromCounter(HWND hwnd)
{
  bool enabled = GetCheckBoxChecked(hwnd, IDC_CHECKCOUNTER);
  SetControlEnabled(hwnd, IDC_EDITCOUNTER, enabled);
  SetControlEnabled(hwnd, IDC_DTPCOUNTER, enabled);
}

void OptionDlg_InitDialog(HWND hwnd)
{
  InitSpinBox(hRes, hwnd, IDC_LISTHOLIDAYLEVEL, IDS_LT_HOLIDAYLEVEL0, IDS_LT_HOLIDAYLEVEL3, HomeData->holidaylevel);

  SYSTEMTIME stime;
  DateTime_SetFormat(GetDlgItem(hwnd, IDC_DTPCOUNTER), L"yyy-MM-dd HH:mm");
  GetLocalTime(&stime);
  stime.wMonth++;
  if (stime.wMonth > 12)
  {
    stime.wYear++;
    stime.wMonth = 1;
  }
  ODateTimeToSystemTime(&HomeData->downcounter->date, &HomeData->downcounter->time, &stime);
  DateTime_SetSystemtime(GetDlgItem(hwnd, IDC_DTPCOUNTER), GDT_VALID, &stime);

  // ��ʼ������
  int longi, longp, lati, latp;
  longi = (int)(HomeData->geo.longitude);
  longp = (int)(fabs(HomeData->geo.longitude - (double)longi) * 60.0 + 0.5);
  lati = (int)(HomeData->geo.latitude);
  latp = (int)(fabs(HomeData->geo.latitude - (double)lati) * 60.0 + 0.5);
  SetIntValueToEdit(hwnd, IDC_EDITLONGITUDE, longi);
  SetIntValueToEdit(hwnd, IDC_EDITLONGITUDE1, longp);
  SetIntValueToEdit(hwnd, IDC_EDITLATITUDE, lati);
  SetIntValueToEdit(hwnd, IDC_EDITLATITUDE1, latp);
  SetIntValueToEdit(hwnd, IDC_EDITZONETIME, HomeData->geo.zonetime);
  SetIntValueToEdit(hwnd, IDC_EDITTEMPERATURE, HomeData->temp_adjust);
  SetIntValueToEdit(hwnd, IDC_EDITINFOINTERVAL, HomeData->GetInfoInterval() / 1000);
  SetCheckBoxChecked(hwnd, IDC_CHECKCOUNTER, HomeData->downenabled);
  SetStrValueToEdit(hwnd, IDC_EDITCOUNTER, HomeData->downcounter->text);
  SetCheckBoxChecked(hwnd, IDC_CHECKTEMPERATURE, HomeData->temp_fix);

  OptionDlg_UpdateFromCounter(hwnd);
}

bool OptionDlg_FinishDialog(HWND hwnd)
{
  int longi, longp, lati, latp, zonetime, adjust, infointerval;
  longi = GetIntValueFromEdit(hwnd, IDC_EDITLONGITUDE, INVALID_INT);
  longp = GetIntValueFromEdit(hwnd, IDC_EDITLONGITUDE1, INVALID_INT);
  lati = GetIntValueFromEdit(hwnd, IDC_EDITLATITUDE, INVALID_INT);
  latp = GetIntValueFromEdit(hwnd, IDC_EDITLATITUDE1, INVALID_INT);
  zonetime = GetIntValueFromEdit(hwnd, IDC_EDITZONETIME, INVALID_INT);
  adjust = GetIntValueFromEdit(hwnd, IDC_EDITTEMPERATURE, INVALID_INT);
  infointerval = GetIntValueFromEdit(hwnd, IDC_EDITINFOINTERVAL, INVALID_INT);

  if (longi > 180 || longi < -180 || longp > 60 || longp < 0 ||
    lati > 180 || lati < -180 || latp > 60 || latp < 0 ||
    zonetime > 24 || zonetime < -24 || adjust > 100 || adjust < -100 ||
    infointerval <= 0 || infointerval > 1000)
  {
    ShowMsgBoxRes(hRes, hwnd, 0, IDS_MSG_INVALIDINT, MB_OK | MB_ICONERROR);
    return false;
  }

  // ȡ����ʱ����
  bool downenabled = GetCheckBoxChecked(hwnd, IDC_CHECKCOUNTER);
  if (downenabled)
  {
    wchar_t buff[TEXT_LEN];
    SendMessage(GetDlgItem(hwnd, IDC_EDITCOUNTER), WM_GETTEXT, LENOF(buff), (LPARAM)buff);
    if (buff[0] == 0)
    {
      ShowMsgBoxRes(hRes, hwnd, 0, IDS_MSG_NAMEISEMPTY, MB_OK | MB_ICONERROR);
      return false;
    }
    else
      CloneString(HomeData->downcounter->text, buff);
  }

  // ��������
  if (longi >= 0)
    HomeData->geo.longitude = (double)longi + (double)longp / 60;
  else
    HomeData->geo.longitude = (double)longi - (double)longp / 60;
  if (lati >= 0)
    HomeData->geo.latitude = (double)lati + (double)latp / 60;
  else
    HomeData->geo.latitude = (double)lati - (double)latp / 60;
  HomeData->geo.zonetime = zonetime;
  HomeData->temp_adjust = adjust;
  HomeData->temp_fix = GetCheckBoxChecked(hwnd, IDC_CHECKTEMPERATURE);
  HomeData->downenabled = downenabled;
  HomeData->holidaylevel = SendMessage(GetDlgItem(hwnd, IDC_LISTHOLIDAYLEVEL), LB_GETCURSEL, 0, 0);
  HomeData->SetInfoInterval(infointerval * 1000);

  if (HomeData->downenabled)
  {
    SYSTEMTIME stime;
    DateTime_GetSystemtime(GetDlgItem(hwnd, IDC_DTPCOUNTER), &stime);
    SystemTimeToODateTime(&stime, &HomeData->downcounter->date, &HomeData->downcounter->time);
  }

  HomeData->SaveSettings();
  HomeData->CheckData(true);
  return true;
}

BOOL CALLBACK Option_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message){
    case WM_INITDIALOG:
      {
        wchar_t title[TITLE_LEN];

        InitializeDialog(hRes, hwnd, 0, IDR_MENU_OKCANCEL, true);

        HomeData->sysinfo->GetDlgTitle(title, TITLE_LEN, IDS_CAP_OPTION);
        SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)title);

        OptionDlg_InitDialog(hwnd);

        return TRUE;
      }

    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
      case IDOK:
        if (OptionDlg_FinishDialog(hwnd))
          EndDialog(hwnd, LOWORD(wParam));
        return TRUE;

      case IDCANCEL:
        EndDialog(hwnd, LOWORD(wParam));
        return TRUE;

      case IDC_CHECKCOUNTER:
        OptionDlg_UpdateFromCounter(hwnd);
        return TRUE;
      }
      return FALSE;

    case WM_NOTIFY:
      break;

    case WM_HOTKEY:
      if (VK_TBACK == HIWORD(lParam))
        SHSendBackToFocusWindow(message, wParam, lParam);
      break;
  }
  return FALSE;
}

/*******************************************************************************
* �����ܶԻ���
*******************************************************************************/

static int g_currpage;  // ��ǰ��ʾҳ��
static int g_currdays;
static HMENU g_hMainMenu;
static HMENU g_hSubMenu[PAGE_LAST + 1];

static WNDPROC g_lpfnMainDlg = NULL;
static WNDPROC g_lpfnListView = NULL;

// ��ʾ���Ի���
void ShowMainDlg(int page)
{
  if (page < 0)
    HomeData->registry->ReadInt(sMainDlg, sCurrPage, g_currpage);
  else
    g_currpage = page;
  DialogBox(hRes, MAKEINTRESOURCE(IDD_MAINDLG), NULL, Main_DlgProc);
}

// ���Ի������໯��Ϣ������
LRESULT CALLBACK MainDlg_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  // �öԻ��������а���
  if (msg == WM_GETDLGCODE)
    return DLGC_WANTALLKEYS;
  else if (msg == WM_ERASEBKGND)
    return TRUE;

  return CallWindowProc((WNDPROC)g_lpfnMainDlg, hwnd, msg, wParam, lParam);
}

// ���б����໯��Ϣ������
LRESULT CALLBACK MainDlg_LVWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  // �öԻ��������а���
  if (msg == WM_GETDLGCODE)
    return DLGC_WANTALLKEYS;
  else if (msg == WM_KEYDOWN || msg == WM_KEYUP)
  {
    if ((wParam >= '0' && wParam <= '9') || wParam == VK_F8 || wParam == VK_F9 || 
      wParam == VK_ESCAPE || wParam == VK_RETURN)
      return SendMessage(GetParent(hwnd), msg, wParam, lParam);
  }

  return CallWindowProc((WNDPROC)g_lpfnListView, hwnd, msg, wParam, lParam);
}

/**************************************
* ����������
**************************************/

// ��������Ƿ���Ч������
static void MainDlg_CheckDate()
{
  ODATE date;
  DecodeDate(g_currdays, &date.year, &date.month, &date.day);
  if (date.year <= LUNAR_START_YEAR)
    EncodeDate(LUNAR_START_YEAR + 1, 1, 1, &g_currdays);
  else if (date.year > LUNAR_END_YEAR)
    EncodeDate(LUNAR_END_YEAR, 12, 31, &g_currdays);
}

// ����������
static void MainDlg_DrawCalendar(HDC hdc, RECT* rc)
{
  ODATE date;
  OTIME time;
  OCALENDAR_INFO info;
  RECT rect;
  wchar_t buff[TEXT_LEN];
  HFONT saveFont, hBgFont, hBoldFont, hFont;
  LOGFONT logFont, lBgFont, lBoldFont, lFont;
  int lh = (rc->bottom - rc->top - 1) / 14; // һ��Ҫ��ʾ 14 ��
  int lh1 = lh * 2;
  int lw = (rc->right - rc->left) / 7; // һ��Ҫ��ʾ 7 ��
  int sy = 1;
  int sx = (rc->right - rc->left - lw * 7) / 2 + 1;
  int y = sy;
  if (rc->bottom - rc->top - lh * 14 > 5)
    lh1++;

  // ���浱ǰ����
  saveFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
  GetObject(saveFont, sizeof(LOGFONT), &logFont);
  SetBkMode(hdc, TRANSPARENT);

  // ��������
  lFont = logFont;
  hFont = CreateFontIndirect(&lFont);
  lBoldFont = logFont;
  lBoldFont.lfWeight = FW_BOLD;
  hBoldFont = CreateFontIndirect(&lBoldFont);
  lBgFont = logFont;
  lBgFont.lfHeight *= 5;
  lBgFont.lfWeight = FW_BOLD;
  hBgFont = CreateFontIndirect(&lBgFont);

  DecodeDate(g_currdays, &date.year, &date.month, &date.day);
  time.hour = HomeData->m_time.wHour;
  time.minute = HomeData->m_time.wMinute;
  time.second = HomeData->m_time.wSecond;
  GetCalendarInfo(&date, &time, &info);

  // �屳��
  FillRectColor(hdc, rc, colorBackgnd);

  // ���Ʊ�������
  wsprintf(buff, L"%d\n%s%s", date.year, sMonthName[date.month - 1], sMonthStr);
  SetTextColor(hdc, colorBackText);
  SelectObject(hdc, hBgFont);
  rect = *rc;
  DrawText(hdc, buff, wcslen(buff), &rect, DT_CENTER | DT_CALCRECT);
  OffsetRect(&rect, ((rc->right - rc->left) - (rect.right - rect.left)) / 2,
    ((rc->bottom - rc->top) - (rect.bottom - rect.top)) / 2);
  DrawText(hdc, buff, wcslen(buff), &rect, DT_CENTER);

  // ���Ƶ�һ�е�����Ϣ
  wsprintf(buff, sFmtCalToday, date.year, date.month, info.sShengXiao, info.sMonth, info.sXingZuo);
  SetRect(&rect, rc->left, y, rc->right, y + lh + 1);
  SetTextColor(hdc, colorTitle);
  SelectObject(hdc, hBoldFont);
  DrawText(hdc, buff, wcslen(buff), &rect, DT_CENTER | DT_VCENTER);
  y += lh;

  // ����������
  SelectObject(hdc, hFont);
  for (int i = 0; i < 7; i++)
  {
    SetTextColor(hdc, (i == 0 || i == 6) ? colorWeekend : colorNormal);
    SetRect(&rect, sx + i * lw, y, sx + (i + 1) * lw, y + lh + 2);
    DrawText(hdc, sWeekName[i], wcslen(sWeekName[i]), &rect, DT_CENTER | DT_VCENTER);
  }
  y += lh;

  // ���½���
  int d1, d2, j1, j2;
  GetJieQiFromMonth(date.year, date.month, &d1, &j1, &d2, &j2);

  // ���½���
  CHolidayItems* items = new CHolidayItems;
  ODATE sdate, edate;
  sdate = date;
  sdate.day = 1;
  edate = date;
  edate.day = GetMonthDays(date.year, date.month);
  HomeData->holiday->GetHoliday(items, &sdate, &edate, 0xFFFF);
  HomeData->userday->GetHoliday(items, &sdate, &edate, 0xFFFF);
  items->Sort();

  // ��������
  for (int i = 0; i < GetMonthDays(date.year, date.month); i++)
  {
    ODATE cdate = date;
    OLUNAR lunar;
    int week;
    cdate.day = i + 1;
    week = GetWeek(cdate.year, cdate.month, cdate.day);
    GetLunarInfo(&cdate, &lunar);

    // Ϊ��ǰ������䱳��
    if (cdate.day == date.day) // ��ǰ����
    {
      RECT rcbg;
      SetRect(&rcbg, sx + week * lw, y + 2, sx + (week + 1) * lw - 1, y + lh * 2 + 2);
      FillRectColor(hdc, &rcbg, colorHighlight);
      DrawRectColor(hdc, &rcbg, colorHightBorder);
    }

    // ��������
    SelectObject(hdc, hBoldFont);
    SetTextColor(hdc, (week == 0 || week == 6) ? colorWeekend : colorNormal);
    SetRect(&rect, sx + week * lw, y, sx + (week + 1) * lw, y + lh + 2);
    wsprintf(buff, L"%d", i + 1);
    DrawText(hdc, buff, wcslen(buff), &rect, DT_CENTER | DT_VCENTER);

    // ������������
    SelectObject(hdc, hFont);
    SetRect(&rect, sx + week * lw, y + lh, sx + (week + 1) * lw, y + lh * 2 + 2);
    if (cdate.day == d1 || cdate.day == d2)
    {
      // �н�������ʾ����
      wchar_t* p = cdate.day == d1 ? sJieQi[j1 - 1] : sJieQi[j2 - 1];
      SetTextColor(hdc, colorJieQi);
      DrawText(hdc, p, wcslen(p), &rect, DT_CENTER | DT_VCENTER);
    }
    else
    {
      memset(buff, 0, sizeof(buff));
      // ���ҽ���
      for (int j = 0; j < items->GetCount(); j++)
      {
        OHOLIDAY_ITEM* item = (OHOLIDAY_ITEM*)items->Get(j);
        if (item->date.day == cdate.day && item->holiday->stext && item->holiday->stext[0])
        {
          wcsncpy(buff, item->holiday->stext, 2);
          switch (item->holiday->data.type)
          {
          case htLunar:
          case htLunarDay:
          case htLunarBirth:
            SetTextColor(hdc, colorLunar);
            break;
          default:
            SetTextColor(hdc, colorSolar);
            break;
          }
          break;
        }
        else if (item->date.day > cdate.day)
          break;
      }

      if (buff[0] == 0)
      {
        // �޽���
        if (lunar.day == 1)
        {
          // ��һ��ʾ�·�
          wchar_t s[TEXT_LEN];
          SetTextColor(hdc, colorMonth);
          if (lunar.month >= 10)
            wsprintf(s, L"%d", lunar.month);
          else
            wcscpy(s, sMonthName[lunar.month - 1]);
          if (lunar.leap)
            wsprintf(buff, L"%s%s", sLeapMonth, s);
          else
            wsprintf(buff, L"%s%s", s, sMonthStr);
        }
        else
        {
          // ��ʾũ������
          SetTextColor(hdc, colorNormal);
          GetLunarDayStr(lunar.day, buff);
        }
      }

      DrawText(hdc, buff, wcslen(buff), &rect, DT_CENTER | DT_VCENTER);
    }

    if (week == 6)
      y += lh1;
  }

  // ���Ƶ�ǰ����Ϣ
  wsprintf(buff, sFmtCalDayCount, g_currdays - HomeData->alldays);
  SelectObject(hdc, hBoldFont);
  SetTextColor(hdc, colorBottom);
  y = sy + 2 * lh + 5 * lh1;
  SetRect(&rect, sx + 2 * lw, y, rc->right, y + lh * 1 + 2);
  DrawText(hdc, buff, wcslen(buff), &rect, DT_LEFT);
  y += lh + 1;

  // ���Ƶ�ǰ����
  wchar_t tmp[256];
  buff[0] = 0;
  for (int i = 0; i < items->GetCount(); i++)
  {
    OHOLIDAY_ITEM* item = (OHOLIDAY_ITEM*)items->Get(i);
    if (item->date.day == date.day)
    {
      HomeData->GetHolidayStr(item, false, false, tmp, 256);
      STRNCAT(buff, tmp);
      STRNCAT(buff, L" ");
    }
  }
  SelectObject(hdc, hBoldFont);
  SetTextColor(hdc, colorHoliday);
  SetRect(&rect, sx + 2 * lw, y, rc->right, y + lh + 2);
  DrawText(hdc, buff, wcslen(buff), &rect, DT_LEFT);

  delete items;

  // �ͷ�����
  SelectObject(hdc, saveFont);
  DeleteObject(hFont);
  DeleteObject(hBgFont);
  DeleteObject(hBoldFont);
}

// ��ʾ������ϸ��Ϣ
static void MainDlg_ShowDetailCalInfo(HWND hwnd)
{
  wchar_t buff[1024], title[TITLE_LEN];
  ODATE date;
  OTIME time;
  OCALENDAR_INFO info;

  // ������Ϣ
  DecodeDate(g_currdays, &date.year, &date.month, &date.day);
  time.hour = HomeData->m_time.wHour;
  time.minute = HomeData->m_time.wMinute;
  time.second = HomeData->m_time.wSecond;
  GetCalendarInfo(&date, &time, &info);

  // �ճ�������Ϣ
  SUNTYPE suntype;
  SUNTIME suntime;
  wchar_t slight[NAME_LEN], srise[NAME_LEN], stransit[NAME_LEN], sset[NAME_LEN], sdark[NAME_LEN];
  suntype = GetSunRiseSetTime(date.year, date.month, date.day,
    HomeData->geo.longitude, HomeData->geo.latitude, HomeData->geo.zonetime, &suntime);
  HomeData->GetSunTimeStr(suntype, &suntime.lighttime, slight);
  HomeData->GetSunTimeStr(suntype, &suntime.risetime, srise);
  HomeData->GetSunTimeStr(suntype, &suntime.transittime, stransit);
  HomeData->GetSunTimeStr(suntype, &suntime.settime, sset);
  HomeData->GetSunTimeStr(suntype, &suntime.darktime, sdark);

  // ������Ϣ
  wchar_t tmp[256], stoday[512];
  CHolidayItems* items = new CHolidayItems;
  stoday[0] = 0;
  HomeData->holiday->GetHoliday(items, &date, &date, 0xFFFF);
  HomeData->userday->GetHoliday(items, &date, &date, 0xFFFF);
  items->Sort();
  for (int i = 0; i < items->GetCount(); i++)
  {
    OHOLIDAY_ITEM* item = (OHOLIDAY_ITEM*)items->Get(i);
    HomeData->GetHolidayStr(item, false, false, tmp, 256);
    STRNCAT(stoday, tmp);
    STRNCAT(stoday, L"\n");
  }
  if (!stoday[0]) wsprintf(stoday, L"%s\n", sNone);
  delete items;

  // ��ʾ�Ի���
  wsprintf(buff, sFmtCalInfo,
    info.date.year, info.date.month, info.date.day, info.sWeek, info.sXingZuo,
    info.sShengXiao, info.sMonth, info.sDay, info.sJieQi,
    info.sgzYear, info.sgzMonth, info.sgzDay,
    srise, sset, slight, sdark, stransit,
    stoday
    );

  HomeData->sysinfo->GetDlgTitle(title, TITLE_LEN, IDS_CAP_CALENDER);
  MessageBox(hwnd, buff, title, MB_OK);
}

// ��ʾ���½�����Ϣ
static void MainDlg_ShowMonthHoliday(HWND hwnd)
{
  wchar_t buff[1024], tmp[256], sday[512], title[TITLE_LEN];
  ODATE date, sdate, edate;
  CHolidayItems* items = new CHolidayItems;

  DecodeDate(g_currdays, &date.year, &date.month, &date.day);
  sdate = date;
  sdate.day = 1;
  edate = date;
  edate.day = GetMonthDays(date.year, date.month);
  HomeData->holiday->GetHoliday(items, &sdate, &edate, 0xFFFF);
  HomeData->userday->GetHoliday(items, &sdate, &edate, 0xFFFF);
  items->Sort();

  sday[0] = 0;
  for (int i = 0; i < items->GetCount(); i++)
  {
    OHOLIDAY_ITEM* item = (OHOLIDAY_ITEM*)items->Get(i);
    HomeData->GetHolidayStr(item, false, true, tmp, 256);
    STRNCAT(sday, tmp);
    STRNCAT(sday, L"\n");
  }
  if (!sday[0]) wsprintf(sday, L"%s\n", sNone);
  delete items;

  // ��ʾ�Ի���
  wsprintf(buff, sFmtMonthHoliDay, sday);

  HomeData->sysinfo->GetDlgTitle(title, TITLE_LEN, IDS_CAP_MONTHHOLIDAY);
  MessageBox(hwnd, buff, title, MB_OK);
}

// ������
static void MainDlg_OnCalCommand(HWND hwnd, UINT cmd)
{
  ODATE date;
  DecodeDate(g_currdays, &date.year, &date.month, &date.day);
  switch(cmd)
  {
  case IDM_LASTDAY:
  case VK_LEFT:
  case '4':
    g_currdays--;
    break;
  case IDM_NEXTDAY:
  case VK_RIGHT:
  case '6':
    g_currdays++;
    break;
  case IDM_LASTWEEK:
  case VK_UP:
  case '2':
    g_currdays -= 7;
    break;
  case IDM_NEXTWEEK:
  case VK_DOWN:
  case '8':
    g_currdays += 7;
    break;
  case IDM_LASTMONTH:
  case IDM_NEXTMONTH:
  case '1':
  case '3':
    if (cmd == '1' || cmd == IDM_LASTMONTH)
      date.month--;
    else
      date.month++;
    if (date.month == 0)
    {
      date.year--;
      date.month = 12;
    }
    if (date.month > 12)
    {
      date.year++;
      date.month = 1;
    }
    if (date.day > GetMonthDays(date.year, date.month))
      date.day = GetMonthDays(date.year, date.month);
    EncodeDate(date.year, date.month, date.day, &g_currdays);
    break;
  case IDM_LASTYEAR:
  case IDM_NEXTYEAR:
  case '7':
  case '9':
    if (cmd == '7' || cmd == IDM_LASTYEAR)
      date.year--;
    else
      date.year++;
    if (date.month == 2 && date.day == 29)
      date.day = 28;
    EncodeDate(date.year, date.month, date.day, &g_currdays);
    break;
  case IDM_GOTOTODAY:
  case '0':
    g_currdays = HomeData->alldays;
    break;
  case IDM_DAYDETAIL:
  case VK_RETURN:
  case '5':
    MainDlg_ShowDetailCalInfo(hwnd);
    return;
  case IDM_MONTHHOLIDAY:
    MainDlg_ShowMonthHoliday(hwnd);
    return;
  }
  MainDlg_CheckDate();
  InvalidateRect(hwnd, NULL, true);
}

/**************************************
* ���ӹ���
**************************************/

static void MainDlg_UpdateAlarmList(HWND hwnd)
{
  ListView_SetItemCount(hwnd, HomeData->alarms->Count());
}

LRESULT MainDlg_OnAlarmsLVNotify(HWND hwnd, LPARAM lParam)
{
  NMHDR* pnmh = (NMHDR*) lParam;
  LRESULT lResult = 0;

  switch(pnmh->code)
  {
  case LVN_GETDISPINFO:
    {
      LV_DISPINFO* pdi = (LV_DISPINFO*)lParam;

      if(0 == pdi->item.iSubItem)
      {
        if(pdi->item.mask & LVIF_TEXT)
        {
          //wchar_t buff[TEXT_LEN], sz[TEXT_LEN];

          //wcsncat(pdi->item.pszText, sz, pdi->item.cchTextMax - wcslen(pdi->item.pszText) - 1);
        }
      }
    }
    break;

  case LVN_ITEMACTIVATE:
    {
      NMLISTVIEW* pnmlv = (NMLISTVIEW*)lParam;
    }
    break;

  case LVN_ODFINDITEM:
    {
      NMLVFINDITEM* pFindItem = (NMLVFINDITEM*)lParam;
    }
    break;
  }

  return(lResult);
}

BOOL CALLBACK Alarms_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#if 0
  HWND hListView = GetDlgItem(hwnd, IDC_LISTITEMS);
  switch(message){
    case WM_INITDIALOG:
      {
        wchar_t title[TITLE_LEN];

        InitializeDialog(hRes, hwnd, IDC_LISTITEMS, IDR_MENU_ITEMS, false);

        HomeData->sysinfo->GetDlgTitle(title, TITLE_LEN, IDS_CAP_ALARMS);
        SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)title);

        InitAlarmsListViewColumn(hListView);

        ListView_SetExtendedListViewStyle(hListView,
          ListView_GetExtendedListViewStyle(hListView) | LVS_EX_FULLROWSELECT);
        ListView_SetItemCount(hListView, HomeData->alarms->Count());

        if (HomeData->alarms->Count() > 0)
          SetListViewItemIndex(hListView, 0);

        return TRUE;
      }

    case WM_COMMAND:
      int select;
      select = ListView_GetNextItem(hListView, -1, LVNI_ALL | LVNI_FOCUSED);
      switch(LOWORD(wParam))
      {
      case IDCANCEL:
        HomeData->SaveAlarms();
        EndDialog(hwnd, LOWORD(wParam));
        return TRUE;

      case ID_ITEMADD:
        g_alarm = new CAlarm;
        g_alarm_mod = false;
        DialogBox(hRes, MAKEINTRESOURCE(IDD_EDITALARMDLG), NULL, EditAlarm_DlgProc);
        if (g_alarm_mod)
        {
          HomeData->alarms->Add(g_alarm);
          SetListViewItemIndex(hListView, HomeData->alarms->Count() - 1);
          MainDlg_UpdateAlarmList(hListView);
        }
        else
        {
          delete g_alarm;
        }
        g_alarm = NULL;
        return TRUE;

      case IDOK:
      case ID_ITEMEDIT:
        if (select >= 0)
        {
          g_alarm = HomeData->alarms->Get(select);
          g_alarm_mod = false;
          DialogBox(hRes, MAKEINTRESOURCE(IDD_EDITALARMDLG), NULL, EditAlarm_DlgProc);
          MainDlg_UpdateAlarmList(hListView);
          g_alarm = NULL;
        }
        return TRUE;

      case ID_ITEMDELETE:
        if (select >= 0)
        {
          HomeData->alarms->Delete(select);
          MainDlg_UpdateHolidayList(hListView);
          if (HomeData->alarms->Count() > 0)
            SetListViewItemIndex(hListView, min(HomeData->alarms->Count() - 1, select));
        }
        return TRUE;

      case ID_ITEMCLEAR:
        if (ShowMsgBoxRes(hRes, hwnd, 0, IDS_MSG_CLEAR, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
          HomeData->alarms->Clear();
          MainDlg_UpdateHolidayList(hListView);
        }
        return TRUE;

      }
      return FALSE;

    case WM_NOTIFY:
      if(IDC_LISTITEMS == wParam)
      {
        MainDlg_OnAlarmsLVNotify(GetDlgItem(hwnd, IDC_LISTITEMS), lParam);
        return TRUE;
      }
      break;
  }
#endif
  return FALSE;
}

// �������ӿؼ��¼�
static BOOL MainDlg_OnAlarmsCommand(HWND hwnd, UINT cmd)
{
  HWND hListView = GetDlgItem(hwnd, IDC_LISTMAINALARM);
  int select = ListView_GetNextItem(hListView, -1, LVNI_ALL | LVNI_FOCUSED);
  switch (cmd)
  {
  case IDM_ALARMADD:
  case '1':
    SYSTEMTIME stime;
    GetLocalTime(&stime);
    g_alarm = new CAlarm;
    g_alarm->data.alarmType = atAlarm;
    g_alarm->data.alarmMode = amRing;
    g_alarm->data.dateStart.year = stime.wYear;
    g_alarm->data.dateStart.month = stime.wMonth;
    g_alarm->data.dateStart.day = stime.wDay;
    // todo: ������һ������ʱ��
    g_alarm->data.timeStart.hour = stime.wHour;
    g_alarm->data.timeStart.minute = stime.wMinute;
    g_alarm->data.timeEnd.hour = stime.wHour;
    g_alarm->data.timeEnd.minute = stime.wMinute;
    g_alarm->data.enabled = true;
    g_alarm->data.recurInfo.type = arOnce;
    g_alarm->data.volume = 0;
    g_alarm_mod = false;
    DialogBox(hRes, MAKEINTRESOURCE(IDD_EDITALARMDLG), NULL, EditAlarm_DlgProc);
    if (g_alarm_mod)
    {
      HomeData->alarms->Add(g_alarm);
      MainDlg_UpdateAlarmList(hListView);
      SetListViewItemIndex(hListView, HomeData->alarms->Count() - 1);
    }
    else
    {
      delete g_alarm;
    }
    g_alarm = NULL;
    break;
  case IDM_ALARMEDIT:
  case '2':
  case VK_RETURN:
    break;
  case IDM_ALARMDELETE:
  case '3':
    break;
  case IDM_ALARMPREVIEW:
  case '0':
    break;
  }
  //MessageBox(hwnd, L"TODO: �����С���", APPNAME, MB_OK | MB_ICONINFORMATION);
  return TRUE;
}

/**************************************
* �Զ�����չ���
**************************************/

static void MainDlg_InitUserDay(HWND hwnd)
{
  ListView_SetItemCount(hwnd, HomeData->userday->Count());
  if (HomeData->userday->Count() > 0)
    SetListViewItemIndex(hwnd, 0);
}

static void MainDlg_UpdateHolidayList(HWND hwnd)
{
  HomeData->userday->UpdateItemIndex();
  ListView_SetItemCount(hwnd, HomeData->userday->Count());
  // ��ֹ��ǰ�����б�������Ѿ�ɾ������
  HomeData->CheckData(true);
}

LRESULT MainDlg_OnHolidayLVNotify(HWND hwnd, LPARAM lParam)
{
  NMHDR* pnmh = (NMHDR*) lParam;
  LRESULT lResult = 0;

  switch(pnmh->code)
  {
  case LVN_GETDISPINFO:
    {
      LV_DISPINFO* pdi = (LV_DISPINFO*)lParam;

      if(0 == pdi->item.iSubItem)
      {
        if(pdi->item.mask & LVIF_TEXT)
        {
          CHoliday* holiday = HomeData->userday->Get(pdi->item.iItem);
          wchar_t buff[TEXT_LEN], sz[TEXT_LEN];

          // ��������
          switch(holiday->data.type)
          {
          case htSolar:
          case htLunar:
          case htWeek:
            wsprintf(pdi->item.pszText, L"[%s]", sLtHoliday);
            break;
          case htSolarBirth:
          case htLunarBirth:
            wsprintf(pdi->item.pszText, L"[%s]", sLtBirthDay);
            break;
          case htSolarDay:
          case htLunarDay:
            wsprintf(pdi->item.pszText, L"[%s]", sLtRemDay);
            break;
          }

          // ��������
          wcsncat(pdi->item.pszText, holiday->text, pdi->item.cchTextMax - wcslen(pdi->item.pszText) - 1);

          buff[0] = 0;
          switch(holiday->data.type)
          {
          case htSolar:
            wsprintf(buff, sLfSolarShort, holiday->data.month, holiday->data.day);
            break;
          case htLunar:
            wsprintf(buff, sLfLunarShort, holiday->data.month, holiday->data.day);
            break;
          case htSolarBirth:
          case htSolarDay:
            wsprintf(buff, sLfSolarDate, holiday->data.year, holiday->data.month, holiday->data.day);
            break;
          case htLunarBirth:
          case htLunarDay:
            wsprintf(buff, sLfLunarDate, holiday->data.year, holiday->data.month, holiday->data.day);
            break;
          case htWeek:
            wsprintf(buff, sLfWeekDate, holiday->data.month, holiday->data.week, sWeekName[holiday->data.weekday]);
            break;
          }

          wsprintf(sz, L"(%s)", buff);
          wcsncat(pdi->item.pszText, sz, pdi->item.cchTextMax - wcslen(pdi->item.pszText) - 1);
        }
      }
    }
    break;

  case LVN_ITEMACTIVATE:
    {
      NMLISTVIEW* pnmlv = (NMLISTVIEW*)lParam;
    }
    break;

  case LVN_ODFINDITEM:
    {
      NMLVFINDITEM* pFindItem = (NMLVFINDITEM*)lParam;
    }
    break;
  }

  return(lResult);
}

// ����ؼ��¼�
static BOOL MainDlg_OnUserDayCommand(HWND hwnd, UINT cmd)
{
  HWND hListView = GetDlgItem(hwnd, IDC_LISTMAINUSERDAY);
  int select = ListView_GetNextItem(hListView, -1, LVNI_ALL | LVNI_FOCUSED);
  switch (cmd)
  {
  case IDM_HOLIDAYADD:
  case '1':
    SYSTEMTIME stime;
    GetLocalTime(&stime);
    g_holiday = new CHoliday;
    g_holiday->data.type = htSolarBirth;
    g_holiday->data.year = stime.wYear;
    g_holiday->data.month = stime.wMonth;
    g_holiday->data.day = stime.wDay;
    g_holiday->data.week = 1;
    g_holiday->data.weekday = stime.wDayOfWeek;
    g_day_mod = false;
    DialogBox(hRes, MAKEINTRESOURCE(IDD_EDITDAYDLG), NULL, UserDlg_DlgProc);
    if (g_day_mod)
    {
      HomeData->userday->Add(g_holiday);
      if (g_addother)
      {
        CHoliday* holiday = UserDlg_AddOtherHoliday(g_holiday);
        if (holiday)
          HomeData->userday->Add(holiday);
      }
      MainDlg_UpdateHolidayList(hListView);
      SetListViewItemIndex(hListView, HomeData->userday->Count() - 1);
    }
    else
    {
      delete g_holiday;
    }
    g_holiday = NULL;
    return TRUE;

  case IDM_HOLIDAYEDIT:
  case '2':
  case VK_RETURN:
    if (select >= 0)
    {
      g_holiday = HomeData->userday->Get(select);
      g_day_mod = false;
      DialogBox(hRes, MAKEINTRESOURCE(IDD_EDITDAYDLG), NULL, UserDlg_DlgProc);
      if (g_day_mod && g_addother)
      {
        CHoliday* holiday = UserDlg_AddOtherHoliday(g_holiday);
        if (holiday)
        {
          HomeData->userday->Insert(select + 1, holiday);
          MainDlg_UpdateHolidayList(hListView);
        }
      }
      g_holiday = NULL;
    }
    return TRUE;

  case IDM_HOLIDAYDELETE:
  case '3':
    if (select >= 0)
    {
      HomeData->userday->Delete(select);
      MainDlg_UpdateHolidayList(hListView);
      if (HomeData->userday->Count() > 0)
        SetListViewItemIndex(hListView, min(HomeData->userday->Count() - 1, select));
    }
    return TRUE;

  case IDM_HOLIDAYCLEAR:
    if (ShowMsgBoxRes(hRes, hwnd, 0, IDS_MSG_CLEAR, MB_ICONQUESTION | MB_YESNO) == IDYES)
    {
      HomeData->userday->Clear();
      MainDlg_UpdateHolidayList(hListView);
    }
    return TRUE;
  }
  return FALSE;
}

/**************************************
* �����б���
**************************************/

static void MainDlg_InitTools(HWND hwnd)
{
  for (int i = 0; i < HomeData->tools->GetCount(); i++)
  {
    CShortCut* shortcut = (CShortCut*)HomeData->tools->Get(i);
    LVITEM item;
    wchar_t buff[TEXT_LEN];
    memset(&item, 0, sizeof(LVITEM));
    item.mask = LVIF_TEXT;
    item.iItem = i;
    if (i < 9)
    {
      wsprintf(buff, L"%d. %s", i + 1, shortcut->title);
      item.pszText = buff;
    }
    else
      item.pszText = shortcut->title;
    item.cchTextMax = TEXT_LEN;
    ListView_InsertItem(hwnd, &item);
  }

  if (HomeData->tools->GetCount() > 0)
    SetListViewItemIndex(hwnd, 0);
}

static void MainDlg_ExecTool(HWND hwnd)
{
  int select = ListView_GetNextItem(GetDlgItem(hwnd, IDC_LISTMAINTOOLS), -1, LVNI_ALL | LVNI_FOCUSED);
  if (select >= 0)
  {
    CShortCut* shortcut = (CShortCut*)HomeData->tools->Get(select);
    bool repaint;
    shortcut->DoAction(repaint);
  }
}

// ������
static void MainDlg_OnToolKeyDown(HWND hwnd, UINT key)
{
  if (key == VK_RETURN)
  {
    MainDlg_ExecTool(hwnd);
  }
  else
  {
    int select = key - '1';
    if (select >= 0 && select <= min(HomeData->tools->GetCount(), 9))
    {
      SetListViewItemIndex(hwnd, select);
      CShortCut* shortcut = (CShortCut*)HomeData->tools->Get(select);
      bool repaint;
      shortcut->DoAction(repaint);
    }
  }
}

/**************************************
* �Ի�����
**************************************/

// ��ʼ�� ListView �ؼ�
static void MainDlg_InitListView(HWND hwnd, UINT ctrlid)
{
  RECT rc;
  HWND hListView = GetDlgItem(hwnd, ctrlid);
  GetClientRect(hwnd, &rc);
  MoveWindow(hListView, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);

  LV_COLUMN lvColumn;
  lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
  lvColumn.fmt = LVCFMT_LEFT;
  lvColumn.cx = rc.right - rc.left;
  ListView_InsertColumn(hListView, 0, &lvColumn);
  ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT | LVS_EX_NOHSCROLL);

  g_lpfnListView = (WNDPROC)SetWindowLong(hListView, GWL_WNDPROC, (LONG)MainDlg_LVWndProc);
}

// �������Ի���ǰ����ؼ�
static void MainDlg_UpdateFocus(HWND hwnd)
{
  if (g_currpage == PAGE_ALARM)
    SetFocus(GetDlgItem(hwnd, IDC_LISTMAINALARM));
  else if (g_currpage == PAGE_USERDAY)
    SetFocus(GetDlgItem(hwnd, IDC_LISTMAINUSERDAY));
  else if (g_currpage == PAGE_TOOLS)
    SetFocus(GetDlgItem(hwnd, IDC_LISTMAINTOOLS));
  else
    SetFocus(hwnd);
}

// ���õ�ǰҳ��
static void MainDlg_SetCurrPage(HWND hwnd, int page)
{
  // ���õ�ǰҳ���
  g_currpage = page;
  if (g_currpage < PAGE_FIRST)
    g_currpage = PAGE_LAST;
  if (g_currpage > PAGE_LAST)
    g_currpage = PAGE_FIRST;

  // ���±���
  wchar_t title[TITLE_LEN];
  HomeData->sysinfo->GetDlgTitle(title, TITLE_LEN, IDS_TI_CALENDER + g_currpage);
  SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)title);

  // ������˵�
  RemoveMenu(g_hMainMenu, 1, MF_BYPOSITION);
  InsertMenu(g_hMainMenu, 1, MF_BYPOSITION | MF_POPUP, (UINT)g_hSubMenu[g_currpage], sAction);
  CheckMenuRadioItem(g_hMainMenu, IDM_PAGECALENDAR, IDM_PAGECALENDAR + PAGE_LAST, 
    IDM_PAGECALENDAR + g_currpage, MF_BYCOMMAND);

  // ���ÿؼ�
  SetControlVisible(hwnd, IDC_LISTMAINALARM, g_currpage == PAGE_ALARM);
  SetControlVisible(hwnd, IDC_LISTMAINUSERDAY, g_currpage == PAGE_USERDAY);
  SetControlVisible(hwnd, IDC_LISTMAINTOOLS, g_currpage == PAGE_TOOLS);
  MainDlg_UpdateFocus(hwnd);

  // �ػ�Ի���
  InvalidateRect(hwnd, NULL, true);
}

// ��ʼ���Ի���
static void MainDlg_InitDialog(HWND hwnd)
{
  InitializeDialog(hRes, hwnd, NULL, IDR_MENU_MAIN, false);

  g_hMainMenu = (HMENU)SendMessage(SHFindMenuBar(hwnd), SHCMBM_GETMENU, (WPARAM)0, (LPARAM)0);
  for (int i = PAGE_FIRST; i <= PAGE_LAST; i++)
    g_hSubMenu[i] = GetSubMenu(g_hMainMenu, i + 1);

  // ��ʾ��������
  g_currdays = HomeData->alldays;

  // ���໯�Ի����Դ������а���
  g_lpfnMainDlg = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (LONG)MainDlg_WndProc);

  // �ؼ���ʼ��
  MainDlg_InitListView(hwnd, IDC_LISTMAINALARM);
  MainDlg_InitListView(hwnd, IDC_LISTMAINUSERDAY);
  MainDlg_InitListView(hwnd, IDC_LISTMAINTOOLS);

  MainDlg_InitUserDay(GetDlgItem(hwnd, IDC_LISTMAINUSERDAY));
  MainDlg_InitTools(GetDlgItem(hwnd, IDC_LISTMAINTOOLS));

  // ��ʼ���˵���
  MainDlg_SetCurrPage(hwnd, g_currpage);
}

static void MainDlg_EndDialog(HWND hwnd, int nResult)
{
  HomeData->SaveHoliday();
  HomeData->registry->WriteInt(sMainDlg, sCurrPage, g_currpage);
  EndDialog(hwnd, nResult);
}

// ����������
static void MainDlg_OnPaint(HWND hwnd)
{
  if (g_currpage == PAGE_CALENDAR)
  {
    RECT rc, rcMem;
    PAINTSTRUCT ps;
    HDC hdc;
    HDC hdcMem;
    HBITMAP hBmpMem, hBmpSave;

    // ʹ����ʱDC���л������
    GetClientRect(hwnd, &rc);
    hdc = GetDC(0);
    hBmpMem = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
    ReleaseDC(0, hdc);
    hdcMem = CreateCompatibleDC(0);
    hBmpSave = (HBITMAP)SelectObject(hdcMem, hBmpMem);

    hdc = BeginPaint(hwnd, &ps);

    SetRect(&rcMem, 0, 0, rc.right - rc.left, rc.bottom - rc.top);
    MainDlg_DrawCalendar(hdcMem, &rcMem);
    BitBlt(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdcMem, 0, 0, SRCCOPY);

    EndPaint(hwnd, &ps);

    SelectObject(hdcMem, hBmpSave);
    DeleteDC(hdcMem);
    DeleteObject(hBmpMem);
  }
  else
  {
    HDC hdc;
    RECT rc;
    PAINTSTRUCT ps;
    hdc = BeginPaint(hwnd, &ps);
    GetClientRect(hwnd, &rc);
    FillRectColor(hdc, &rc, COLOR_WHITE);
    EndPaint(hwnd, &ps);
  }
}

// ����˵�����
static void MainDlg_OnMenuPopup(HWND hwnd, HMENU hMenu)
{
  
}

// ����ؼ��¼�
static void MainDlg_OnCommand(HWND hwnd, UINT cmd)
{
  switch(cmd)
  {
  case IDCANCEL:
    MainDlg_EndDialog(hwnd, cmd);
    break;

  case IDM_PAGECALENDAR:
  case IDM_PAGEALARM:
  case IDM_PAGEUSERDAY:
  case IDM_PAGETOOLS:
    MainDlg_SetCurrPage(hwnd, cmd - IDM_PAGECALENDAR);
    break;
  case IDM_PAGELAST:
    MainDlg_SetCurrPage(hwnd, g_currpage - 1);
    break;
  case IDM_PAGENEXT:
    MainDlg_SetCurrPage(hwnd, g_currpage + 1);
    break;

  case IDM_LASTDAY:
  case IDM_NEXTDAY:
  case IDM_LASTWEEK:
  case IDM_NEXTWEEK:
  case IDM_LASTMONTH:
  case IDM_NEXTMONTH:
  case IDM_LASTYEAR:
  case IDM_NEXTYEAR:
  case IDM_GOTOTODAY:
  case IDM_DAYDETAIL:
  case IDM_MONTHHOLIDAY:
    MainDlg_OnCalCommand(hwnd, cmd);
    break;

  case IDM_ALARMADD:
  case IDM_ALARMEDIT:
  case IDM_ALARMDELETE:
  case IDM_ALARMCLEAR:
  case IDM_ALARMPREVIEW:
    MainDlg_OnAlarmsCommand(hwnd, cmd);
    break;

  case IDM_HOLIDAYADD:
  case IDM_HOLIDAYEDIT:
  case IDM_HOLIDAYDELETE:
  case IDM_HOLIDAYCLEAR:
    MainDlg_OnUserDayCommand(hwnd, cmd);
    break;

  case IDM_EXECTOOL:
    MainDlg_ExecTool(hwnd);
    break;

  case IDM_OPTION:
    HomeData->sysinfo->ShowOption();
    break;

  case IDM_MAILTO:
    ExecuteFile(MAILTO, NULL);
    break;
  case IDM_WEBSITE:
    ExecuteFile(WEBSITE, NULL);
    break;
  case IDM_ABOUT:
    HomeData->sysinfo->ShowAbout();
    break;
  }
}

// ������
static void MainDlg_OnKeyDown(HWND hwnd, UINT key)
{
  if (key == VK_ESCAPE) // ����
    MainDlg_EndDialog(hwnd, LOWORD(key));
  else if (key == VK_F8) // �Ǻ�
    MainDlg_SetCurrPage(hwnd, g_currpage - 1);
  else if (key == VK_F9) // ����
    MainDlg_SetCurrPage(hwnd, g_currpage + 1);
  else if (g_currpage == PAGE_CALENDAR)
    MainDlg_OnCalCommand(hwnd, key);
  else if (g_currpage == PAGE_ALARM)
    MainDlg_OnAlarmsCommand(hwnd, key);
  else if (g_currpage == PAGE_USERDAY)
    MainDlg_OnUserDayCommand(hwnd, key);
  else if (g_currpage == PAGE_TOOLS)
    MainDlg_OnToolKeyDown(hwnd, key);
}

// �Ի���������
BOOL CALLBACK Main_DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
  case WM_INITDIALOG:
    MainDlg_InitDialog(hwnd);
    return TRUE;

  case WM_PAINT:
    MainDlg_OnPaint(hwnd);
    return TRUE;

  case WM_KEYDOWN:
    MainDlg_OnKeyDown(hwnd, wParam);
    return TRUE;

  case WM_INITMENUPOPUP:
    MainDlg_OnMenuPopup(hwnd, (HMENU)wParam);
    return TRUE;

  case WM_COMMAND:
    MainDlg_OnCommand(hwnd, LOWORD(wParam));
    return TRUE;

  case WM_NOTIFY:
    if (IDC_LISTMAINALARM == wParam)
    {
      MainDlg_OnAlarmsLVNotify(GetDlgItem(hwnd, IDC_LISTMAINALARM), lParam);
      return TRUE;
    }
    else if (IDC_LISTMAINUSERDAY == wParam)
    {
      MainDlg_OnHolidayLVNotify(GetDlgItem(hwnd, IDC_LISTMAINUSERDAY), lParam);
      return TRUE;
    }
    break;

  case WM_ACTIVATE:
    if (LOWORD(wParam) == WA_ACTIVE)
      MainDlg_UpdateFocus(hwnd);
    return TRUE;
  }
  return FALSE;
}
