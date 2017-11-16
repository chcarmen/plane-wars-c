/******************************************************
* File name:     PlaneWars.h
* Auther:        RedMonkey
* Date:          2017/11/21
* Description:   Plane wars game source code
*****************************************************/

#pragma once

#include <assert.h>
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
#define PLANETYPECOUNT 3           /* 3 types: small, middle, big      */
#define BMPCOUNT       9           /* Bmp resource number              */


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
} GAMESTATUS;

/* Game info */
typedef struct {
    GAMESTATUS status;        /* Game status           */
    SIZE       boardSize;     /* Game window size      */
    UINT       planeTypeNum;  /* Plane type number     */
    UINT       planeNum;      /* Plane number          */
    UINT       score;         /* Game score            */
    BOOL       bigAdded;      /* If big plane is added */
} GAME;

/* 3 different types plane */
typedef enum
{
    SMALL,
    MIDDLE,
    BIG
}PLANETYPE;

/* Plane type info */
typedef struct {
    PLANETYPE type;    /* Plane type      */
    SIZE      size;    /* Plane size      */
    int       maxHC;   /* max hit counter */
} PLANETYPEINFO;

/* Plane info */
typedef struct {
    PLANETYPE type;    /* Plane type      */
    POINT     pos;     /* plane postion   */
    int       hc;      /* hit counter     */
    UINT      speed;   /* move speed      */
} PLANE;

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