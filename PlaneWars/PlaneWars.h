/******************************************************
* File name:     PlaneWars.h
* Auther:        RedMonkey
* Date:          2017/11/21
* Description:   Macros and function declaration of implementing the plane wars game
*****************************************************/

#pragma once

#include <assert.h>

/* needed by mci */
#include <mmsystem.h>

#include "resource.h"

#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"winmm.lib")


#define MAX_LOADSTRING 100         /* Defined by default               */
#define WM_START       (WM_USER+1) /* User defined start message       */
#define TIMER          1           /* Timer ID                         */
#define WNDWIDTH       380         /* Game window width                */
#define WNDHEIGHT      550         /* Game window height               */
#define PLANECOUNT     10          /* Plane number shows in the window */
#define BMPCOUNT       9           /* Bmp resource number              */


/* 3 different types plane */
typedef enum
{
    SMALL,
    MIDDLE,
    BIG
}PLANETYPE;

/* Plane info */
typedef struct
{
    HBITMAP          hBmp;         /* Plane bmp handle                         */
    POINT            pos;          /* Plane postion                            */
    SIZE             size;         /* Plane size                               */
    PLANETYPE        type;         /* Plane type                               */
    int              moveSpeed;    /* Each plane may have different move speed */
    int              hitCounter;   /* Plane crashes only when hitCounter achieves maximum. 
                                   Possible value: small:0-1,  middle:0-2,  big:0-3, After crash: -1 */
}PLANE, *PPLANE;

/* The game window has 3 diff statuses:
*    WELCOME:  show logo, start button
*    RUN:      show planes
*    GAMEOVER: show score, restart button, exit buttion
*/
typedef enum
{
    WELCOME,
    RUN,
    GAMEOVER
}GAMESTATUS;


/* Function declaration */
ATOM              MyRegisterClass(HINSTANCE hInstance);
BOOL              InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK  WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK  About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
VOID              InitGame(LPARAM lParam);
VOID              DestroyGame();
VOID              UpdatePlaneInfo(BOOL bReset, UINT uiIndex, UINT uiSpeed);
VOID              DrawWindow(HDC hdc);
VOID              TimerProc(HWND hWnd);
VOID              LButtonDownProc(HWND hWnd, LPARAM lParam);
VOID              Hit(POINT ptMouse);
BOOL              CheckGameStartButtonDown(POINT ptMouse);
BOOL              CheckGameReStartButtonDown(POINT ptMouse);
BOOL              CheckGameExitButtonDown(POINT ptMouse);