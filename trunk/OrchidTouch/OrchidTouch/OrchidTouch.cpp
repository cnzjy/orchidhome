/*******************************************************************************
* 软件名称：Orchid Mobile Software Touch/Keypad Patch
* 文件名称：Touch Patch 单元
* 文件作者：OrchidSoft (master@orchid-soft.com)
* 单元标识：$Id: OrchidTouch.cpp,v 1.15 2008/02/29 06:22:35 Administrator Exp $
* 备    注：
*******************************************************************************/

#include "OrchidTouch.h"
#include "ODebug.h"
#include "OTUtils.h"
#include "OTComExport.h"
#include "OTConfig.h"
#include "tchddi.h"
#include <windows.h>

/************************************************************************/
/* 数据定义
/************************************************************************/

// 当前事件动作
typedef enum
{
    teWait,
    teDataDelay,
    teExit
} TTouchEvent;

TTouchEvent gTouchEvent = teWait;

INT gMoveDist = 5;

// 触摸屏数据
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

// 原驱动DLL句柄
HMODULE hOldDriver = NULL;

// 对象句柄
HANDLE hTouchEvent = INVALID_HANDLE_VALUE;
HANDLE hTouchThread = INVALID_HANDLE_VALUE;

// 原驱动DDI函数地址
PFN_TOUCH_PANEL_GET_DEVICE_CAPS pfnTouchPanelGetDeviceCaps = NULL;
PFN_TOUCH_PANEL_SET_MODE pfnTouchPanelSetMode = NULL;
PFN_TOUCH_PANEL_ENABLE pfnTouchPanelEnable = NULL;
PFN_TOUCH_PANEL_DISABLE pfnTouchPanelDisable = NULL;
PFN_TOUCH_PANEL_READ_CALIBRATION_POINT pfnTouchPanelReadCalibrationPoint = NULL;
PFN_TOUCH_PANEL_READ_CALIBRATION_ABORT pfnTouchPanelReadCalibrationAbort = NULL;
PFN_TOUCH_PANEL_SET_CALIBRATION pfnTouchPanelSetCalibration = NULL;
PFN_TOUCH_PANEL_CALIBRATE_A_POINT pfnTouchPanelCalibrateAPoint = NULL;
PFN_TOUCH_PANEL_POWER_HANDLER pfnTouchPanelPowerHandler = NULL;

// 原回调函数
PFN_TOUCH_PANEL_CALLBACK pfnTouchPanelCallBack = NULL;

/************************************************************************/
/* 初始化处理
/************************************************************************/

void CreateTouchThread();
void FreeTouchThread();

VOID InitEnv()
{
    DLOG(L"InitEnv()");

    // 检查是否已初始化
    if (hOldDriver)
        return;

    LoadSettings();

    // 加载原驱动并查找DDI函数表
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
/* 处理线程
/************************************************************************/

DWORD TouchThreadProc(
                      LPVOID lpParameter
                      )
{
    while (gTouchEvent != teExit)
    {
        if (gTouchEvent == teDataDelay)
        {
            // 触摸屏点击后延时等待以进行消抖
            DLOG1(L"Start delay %d ms", g_Delay);
            if (WaitForSingleObject(hTouchEvent, g_Delay) == WAIT_TIMEOUT)
            {
                // 等待结束时如果没有取消则发送按下事件
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
            // 等待下一事件
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

// 标志掩码
#define TOUCH_MASK (TouchSampleValidFlag | TouchSampleDownFlag | TouchSamplePreviousDownFlag | TouchSampleIgnore)
// 按下标志
#define TOUCH_DOWN (TouchSampleValidFlag | TouchSampleDownFlag)
// 放开标志
#define TOUCH_UP   (TouchSampleValidFlag | TouchSamplePreviousDownFlag)
// 拖动标志
#define TOUCH_MOVE (TouchSampleValidFlag | TouchSamplePreviousDownFlag | TouchSampleDownFlag)
// 放开拖动标志
#define TOUCH_UPMOVE (TouchSampleValidFlag)

// 触摸屏回调处理函数
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
            // 该次点击与上一次抬起的距离和时间间隔都小于指定值时可能为重复点击
            if ((abs(X - gLastTouchUp.X) <= g_AreaX) && (abs(Y - gLastTouchUp.Y) <= g_AreaY) && 
                (tick - gLastTouchUp.Tick <= g_Interval))
            {
                // 延迟发送按下信息
                gLastTouchDown = touchData;
                gTouchEvent = teDataDelay;
                SetEvent(hTouchEvent);
                DLOG(L"Save touch down data and delay");
                return TRUE;
            }

            // 以按下时的数据来判断是否移动
            gLastTouchMove = touchData;
            gTouchEvent = teWait;
        }
        else if ((Flags & TOUCH_MASK) == TOUCH_UP)
        {
            DLOG1(L"Start touch up. Event: %d", gTouchEvent);

            // 抬起时始终记录
            gLastTouchUp = touchData;

            // 如果上一次按下被推迟，则直接取消
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
                // 延时发送时先检查移动范围
                if ((abs(X - gLastTouchMove.X) <= g_AreaX) && (abs(Y - gLastTouchMove.Y) <= g_AreaY))
                {
                    DLOG3(L"Cancel touch move. Flags: %x, X: %d, Y: %d", Flags, X, Y);
                    return TRUE;
                }

                // 延时发送过程中拖动屏幕应立即发送按下事件
                gTouchEvent = teWait;
                pfnTouchPanelCallBack(gLastTouchDown.Flags, gLastTouchDown.X,gLastTouchDown.Y);
                SetEvent(hTouchEvent);
                DLOG(L"Send touch down data by touch move");
            }

            if ((abs(X - gLastTouchMove.X) <= gMoveDist) && (abs(Y - gLastTouchMove.Y) <= gMoveDist))
            {
                // 跳过频繁出现的移动事件以提高性能
                return TRUE;
            }
            gLastTouchMove = touchData;
        }
        else if ((Flags & TOUCH_MASK) == TOUCH_UPMOVE)
        {
            // 触笔抬起时移动事件应该没啥意义，直接丢弃
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
    // 触摸屏回调需要拦截住进行处理
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
