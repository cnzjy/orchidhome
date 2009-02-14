/*******************************************************************************
* ������ƣ�Orchid Mobile Software Touch/Keypad Patch
* �ļ����ƣ�Touch Patch ��Ԫ
* �ļ����ߣ�OrchidSoft (master@orchid-soft.com)
* ��Ԫ��ʶ��$Id: OrchidTouch.cpp,v 1.15 2008/02/29 06:22:35 Administrator Exp $
* ��    ע��
*******************************************************************************/

#include "OrchidTouch.h"
#include "ODebug.h"
#include "OTUtils.h"
#include "OTComExport.h"
#include "OTConfig.h"
#include "tchddi.h"
#include <windows.h>

/************************************************************************/
/* ���ݶ���
/************************************************************************/

// ��ǰ�¼�����
typedef enum
{
    teWait,
    teDataDelay,
    teExit
} TTouchEvent;

TTouchEvent gTouchEvent = teWait;

INT gMoveDist = 5;

// ����������
typedef struct  
{
    TOUCH_PANEL_SAMPLE_FLAGS Flags;
    INT	X;
    INT	Y;
    UINT Tick;
} TTouchData;

TTouchData gLastTouchDown = {0};
TTouchData gLastTouchUp = {0};
TTouchData gLastTouchMove = {0};

// ԭ����DLL���
HMODULE hOldDriver = NULL;

// ������
HANDLE hTouchEvent = INVALID_HANDLE_VALUE;
HANDLE hTouchThread = INVALID_HANDLE_VALUE;

// ԭ����DDI������ַ
PFN_TOUCH_PANEL_GET_DEVICE_CAPS pfnTouchPanelGetDeviceCaps = NULL;
PFN_TOUCH_PANEL_SET_MODE pfnTouchPanelSetMode = NULL;
PFN_TOUCH_PANEL_ENABLE pfnTouchPanelEnable = NULL;
PFN_TOUCH_PANEL_DISABLE pfnTouchPanelDisable = NULL;
PFN_TOUCH_PANEL_READ_CALIBRATION_POINT pfnTouchPanelReadCalibrationPoint = NULL;
PFN_TOUCH_PANEL_READ_CALIBRATION_ABORT pfnTouchPanelReadCalibrationAbort = NULL;
PFN_TOUCH_PANEL_SET_CALIBRATION pfnTouchPanelSetCalibration = NULL;
PFN_TOUCH_PANEL_CALIBRATE_A_POINT pfnTouchPanelCalibrateAPoint = NULL;
PFN_TOUCH_PANEL_POWER_HANDLER pfnTouchPanelPowerHandler = NULL;

// ԭ�ص�����
PFN_TOUCH_PANEL_CALLBACK pfnTouchPanelCallBack = NULL;

/************************************************************************/
/* ��ʼ������
/************************************************************************/

void CreateTouchThread();
void FreeTouchThread();

VOID InitEnv()
{
    DLOG(L"InitEnv()");

    // ����Ƿ��ѳ�ʼ��
    if (hOldDriver)
        return;

    LoadSettings();

    // ����ԭ����������DDI������
    if (FileExists(g_OldDriverName))
    {
        hOldDriver = LoadDriver(g_OldDriverName);
        DLOG1(L"hOldDriver... %x", hOldDriver);
        if (hOldDriver)
        {
            pfnTouchPanelGetDeviceCaps = (PFN_TOUCH_PANEL_GET_DEVICE_CAPS)GetProcAddress(hOldDriver, L"TouchPanelGetDeviceCaps");
            DLOG1(L"pfnTouchPanelGetDeviceCaps: %x", pfnTouchPanelGetDeviceCaps);
            pfnTouchPanelSetMode = (PFN_TOUCH_PANEL_SET_MODE)GetProcAddress(hOldDriver, L"TouchPanelSetMode");
            DLOG1(L"pfnTouchPanelSetMode: %x", pfnTouchPanelSetMode);
            pfnTouchPanelEnable = (PFN_TOUCH_PANEL_ENABLE)GetProcAddress(hOldDriver, L"TouchPanelEnable");
            DLOG1(L"pfnTouchPanelEnable: %x", pfnTouchPanelEnable);
            pfnTouchPanelDisable = (PFN_TOUCH_PANEL_DISABLE)GetProcAddress(hOldDriver, L"TouchPanelDisable");
            DLOG1(L"pfnTouchPanelDisable: %x", pfnTouchPanelDisable);
            pfnTouchPanelReadCalibrationPoint = (PFN_TOUCH_PANEL_READ_CALIBRATION_POINT)GetProcAddress(hOldDriver, L"TouchPanelReadCalibrationPoint");
            DLOG1(L"pfnTouchPanelReadCalibrationPoint: %x", pfnTouchPanelReadCalibrationPoint);
            pfnTouchPanelReadCalibrationAbort = (PFN_TOUCH_PANEL_READ_CALIBRATION_ABORT)GetProcAddress(hOldDriver, L"TouchPanelReadCalibrationAbort");
            DLOG1(L"pfnTouchPanelReadCalibrationAbort: %x", pfnTouchPanelReadCalibrationAbort);
            pfnTouchPanelSetCalibration = (PFN_TOUCH_PANEL_SET_CALIBRATION)GetProcAddress(hOldDriver, L"TouchPanelSetCalibration");
            DLOG1(L"pfnTouchPanelSetCalibration: %x", pfnTouchPanelSetCalibration);
            pfnTouchPanelCalibrateAPoint = (PFN_TOUCH_PANEL_CALIBRATE_A_POINT)GetProcAddress(hOldDriver, L"TouchPanelCalibrateAPoint");
            DLOG1(L"pfnTouchPanelCalibrateAPoint: %x", pfnTouchPanelCalibrateAPoint);
            pfnTouchPanelPowerHandler = (PFN_TOUCH_PANEL_POWER_HANDLER)GetProcAddress(hOldDriver, L"TouchPanelPowerHandler");
            DLOG1(L"pfnTouchPanelPowerHandler: %x", pfnTouchPanelPowerHandler);
        }
    }

    CreateTouchThread();
}

VOID FinalEnv()
{
    DLOG(L"FinalEnv()");

    FreeTouchThread();

    if (hOldDriver)
    {
        FreeLibrary(hOldDriver);
        hOldDriver = 0;
    }
}

/************************************************************************/
/* �����߳�
/************************************************************************/

DWORD TouchThreadProc(
                      LPVOID lpParameter
                      )
{
    while (gTouchEvent != teExit)
    {
        if (gTouchEvent == teDataDelay)
        {
            // �������������ʱ�ȴ��Խ�������
            DLOG1(L"Start delay %d ms", g_Delay);
            if (WaitForSingleObject(hTouchEvent, g_Delay) == WAIT_TIMEOUT)
            {
                // �ȴ�����ʱ���û��ȡ�����Ͱ����¼�
                if (gTouchEvent == teDataDelay)
                {
                    gTouchEvent = teWait;
                    pfnTouchPanelCallBack(gLastTouchDown.Flags, gLastTouchDown.X, gLastTouchDown.Y);
                    DLOG3(L"Send touch data. Flags: %x, X: %d, Y: %d", gLastTouchDown.Flags, gLastTouchDown.X, gLastTouchDown.Y);
                }
            }
        }
        else
        {
            // �ȴ���һ�¼�
            DLOG(L"Wait for event");
            WaitForSingleObject(hTouchEvent, INFINITE);
        }
        ResetEvent(hTouchEvent);
    }
    return 0;
}

void CreateTouchThread()
{
    DLOG(L"CreateTouchThread()");

    hTouchEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    hTouchThread = CreateThread(NULL, 0, TouchThreadProc, NULL, 0, NULL);
    SetThreadPriority(hTouchThread, THREAD_PRIORITY_HIGHEST);
}

void FreeTouchThread()
{
    DLOG(L"FreeTouchThread()");

    if (hTouchThread != INVALID_HANDLE_VALUE)
    {
        gTouchEvent = teExit;
        SetEvent(hTouchEvent);
        //TerminateThread(hTouchThread);
    }

    if (hTouchEvent != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hTouchEvent);
    }
}

// ��־����
#define TOUCH_MASK (TouchSampleValidFlag | TouchSampleDownFlag | TouchSamplePreviousDownFlag | TouchSampleIgnore)
// ���±�־
#define TOUCH_DOWN (TouchSampleValidFlag | TouchSampleDownFlag)
// �ſ���־
#define TOUCH_UP   (TouchSampleValidFlag | TouchSamplePreviousDownFlag)
// �϶���־
#define TOUCH_MOVE (TouchSampleValidFlag | TouchSamplePreviousDownFlag | TouchSampleDownFlag)
// �ſ��϶���־
#define TOUCH_UPMOVE (TouchSampleValidFlag)

// �������ص�������
BOOL
TouchPanelCallBack(
                   TOUCH_PANEL_SAMPLE_FLAGS	Flags,
                   INT	X,
                   INT	Y
                   )
{
    if (pfnTouchPanelCallBack)
    {
        UINT tick = GetTickCount();
        TTouchData touchData;

        touchData.Flags = Flags;
        touchData.X = X;
        touchData.Y = Y;
        touchData.Tick = tick;

        if ((Flags & TOUCH_MASK) == TOUCH_DOWN)
        {
            DLOG1(L"Start touch down. Event: %d", gTouchEvent);
            // �ôε������һ��̧��ľ����ʱ������С��ָ��ֵʱ����Ϊ�ظ����
            if ((abs(X - gLastTouchUp.X) <= g_AreaX) && (abs(Y - gLastTouchUp.Y) <= g_AreaY) && 
                (tick - gLastTouchUp.Tick <= g_Interval))
            {
                // �ӳٷ��Ͱ�����Ϣ
                gLastTouchDown = touchData;
                gTouchEvent = teDataDelay;
                SetEvent(hTouchEvent);
                DLOG(L"Save touch down data and delay");
                return TRUE;
            }

            // �԰���ʱ���������ж��Ƿ��ƶ�
            gLastTouchMove = touchData;
            gTouchEvent = teWait;
        }
        else if ((Flags & TOUCH_MASK) == TOUCH_UP)
        {
            DLOG1(L"Start touch up. Event: %d", gTouchEvent);

            // ̧��ʱʼ�ռ�¼
            gLastTouchUp = touchData;

            // �����һ�ΰ��±��Ƴ٣���ֱ��ȡ��
            if (gTouchEvent == teDataDelay)
            {
                gTouchEvent = teWait;
                SetEvent(hTouchEvent);
                DLOG(L"Cancel touch data");
                return TRUE;
            }

            gTouchEvent = teWait;
        }
        else if ((Flags & TOUCH_MASK) == TOUCH_MOVE)
        {
            if (gTouchEvent == teDataDelay)
            {
                // ��ʱ����ʱ�ȼ���ƶ���Χ
                if ((abs(X - gLastTouchMove.X) <= g_AreaX) && (abs(Y - gLastTouchMove.Y) <= g_AreaY))
                {
                    DLOG3(L"Cancel touch move. Flags: %x, X: %d, Y: %d", Flags, X, Y);
                    return TRUE;
                }

                // ��ʱ���͹������϶���ĻӦ�������Ͱ����¼�
                gTouchEvent = teWait;
                pfnTouchPanelCallBack(gLastTouchDown.Flags, gLastTouchDown.X,gLastTouchDown.Y);
                SetEvent(hTouchEvent);
                DLOG(L"Send touch down data by touch move");
            }

            if ((abs(X - gLastTouchMove.X) <= gMoveDist) && (abs(Y - gLastTouchMove.Y) <= gMoveDist))
            {
                // ����Ƶ�����ֵ��ƶ��¼����������
                return TRUE;
            }
            gLastTouchMove = touchData;
        }
        else if ((Flags & TOUCH_MASK) == TOUCH_UPMOVE)
        {
            // ����̧��ʱ�ƶ��¼�Ӧ��ûɶ���壬ֱ�Ӷ���
            DLOG3(L"Skip touch up move. Flags: %x, X: %d, Y: %d", Flags, X, Y);
            return TRUE;
        }

        DLOG3(L"Flags: %x, X: %d, Y: %d", Flags, X, Y);
        return pfnTouchPanelCallBack(Flags, X, Y);
    }
    else
        return FALSE;
}

BOOL
TouchPanelEnable(
                 PFN_TOUCH_PANEL_CALLBACK   cbTouch
                 )
{
    DLOG(L"TouchPanelEnable");
    // �������ص���Ҫ����ס���д���
    pfnTouchPanelCallBack = cbTouch;
    if (pfnTouchPanelEnable)
        return pfnTouchPanelEnable(TouchPanelCallBack);
    else
        return FALSE;
}

VOID
TouchPanelDisable(
                  VOID
                  )
{
    DLOG(L"TouchPanelDisable");
    if (pfnTouchPanelDisable)
        pfnTouchPanelDisable();
}

BOOL
TouchPanelReadCalibrationPoint(
                               INT	*pUncalibratedX,
                               INT	*pUncalibratedY
                               )
{
    DLOG(L"TouchPanelReadCalibrationPoint");
    if (pfnTouchPanelReadCalibrationPoint)
        return pfnTouchPanelReadCalibrationPoint(pUncalibratedX, pUncalibratedY);
    else
        return FALSE;
}

BOOL
TouchPanelGetDeviceCaps(
                        INT		iIndex,
                        LPVOID  lpOutput
                        )
{
    DLOG1(L"TouchPanelGetDeviceCaps. iIndex: %d", iIndex);
    if (pfnTouchPanelGetDeviceCaps)
        return pfnTouchPanelGetDeviceCaps(iIndex, lpOutput);
    else
        return FALSE;
}

BOOL
TouchPanelSetMode(
                  INT		iIndex,
                  LPVOID	lpInput
                  )
{
    DLOG1(L"TouchPanelSetMode. iIndex: %d", iIndex);
    if (pfnTouchPanelSetMode)
        return pfnTouchPanelSetMode(iIndex, lpInput);
    else
        return FALSE;
}

VOID
TouchPanelReadCalibrationAbort(
                               VOID
                               )
{
    DLOG(L"TouchPanelReadCalibrationAbort");
    if (pfnTouchPanelReadCalibrationAbort)
        pfnTouchPanelReadCalibrationAbort();
}

BOOL
TouchPanelSetCalibration(
                         INT32	cCalibrationPoints,		//The number of calibration points
                         INT32	*pScreenXBuffer,		//List of screen X coords displayed
                         INT32	*pScreenYBuffer,		//List of screen Y coords displayed
                         INT32	*pUncalXBuffer,			//List of X coords collected
                         INT32	*pUncalYBuffer			//List of Y coords collected
                         )
{
    DLOG(L"TouchPanelSetCalibration");
    if (pfnTouchPanelSetCalibration)
        return pfnTouchPanelSetCalibration(cCalibrationPoints, pScreenXBuffer, pScreenYBuffer, pUncalXBuffer, pUncalYBuffer);
    else
        return FALSE;
}

void
TouchPanelCalibrateAPoint(
                          INT32   UncalX,
                          INT32   UncalY,
                          INT32   *pCalX,
                          INT32   *pCalY
                          )
{
    DLOG(L"TouchPanelCalibrateAPoint");
    if (pfnTouchPanelCalibrateAPoint)
        pfnTouchPanelCalibrateAPoint(UncalX, UncalY, pCalX, pCalY);
}

void
TouchPanelPowerHandler(
                       BOOL	bOff
                       )
{
    DLOG1(L"TouchPanelPowerHandler. bOff: %d", bOff);
    if (pfnTouchPanelPowerHandler)
        pfnTouchPanelPowerHandler(bOff);
}
