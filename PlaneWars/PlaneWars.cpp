/******************************************************
* File name:    PlaneWars.cpp
* Auther:       RedMonkey
* Date:         2017/11/17
* Description:  Plane wars game source code
*****************************************************/

#include "stdafx.h"
#include "PlaneWars.h"

#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"winmm.lib")

#define MAX_LOADSTRING 100
#define TICK_TIMER     1

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HBITMAP hBmpBig;
HBITMAP hBmpBk;
HBITMAP hBmpExit;
HBITMAP hBmpGameover;
HBITMAP hBmpLogo;
HBITMAP hBmpMiddle;
HBITMAP hBmpPlay;
HBITMAP hBmpRestart;
HBITMAP hBmpSmall;
HBITMAP hBmpStart;

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
VOID                InitGame();
VOID                UninitGame();
VOID                DrawWindow(HDC hdc);
VOID                TimerProc(HWND hWnd);
VOID                LButtonDownProc(HWND hWnd, LPARAM lParam);
BOOL                CheckGameStartButtonDown(POINT ptMouse);
BOOL                CheckGameReStartButtonDown(POINT ptMouse);
BOOL                CheckGameExitButtonDown(POINT ptMouse);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLANEWARS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLANEWARS));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLANEWARS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   // init the game
   InitGame();

   // calculate the window size from client size
   BITMAP bmp;
   GetObject(hBmpBk, sizeof(BITMAP), &bmp);
   RECT rect ={ 0, 0, bmp.bmWidth, bmp.bmHeight };
   AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW  & (~WS_MAXIMIZEBOX) & (~WS_SIZEBOX), FALSE);
   int nWndWidth  = rect.right - rect.left;
   int nWndHeight = rect.bottom - rect.top;

   int cxScreen = GetSystemMetrics(SM_CXSCREEN);
   int cyScreen = GetSystemMetrics(SM_CYSCREEN);

   HWND hWnd = CreateWindowW(szWindowClass, 
                             szTitle,
                             WS_OVERLAPPEDWINDOW  & (~WS_MAXIMIZEBOX) & (~WS_SIZEBOX),
                             (cxScreen - nWndWidth) / 2,
                             (cyScreen - nWndHeight + 100) / 2,
                             nWndWidth,
                             nWndHeight,
                             nullptr,
                             nullptr,
                             hInstance,
                             nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            DrawWindow(hdc);
            EndPaint(hWnd, &ps);
        }
        break; 
    case WM_TIMER:
        TimerProc(hWnd);
        break;
    case WM_LBUTTONDOWN:
        LButtonDownProc(hWnd, lParam);
        break;
    case WM_DESTROY:
        UninitGame();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

VOID InitGame()
{
    int ret;
    PLANE_TYPE_INFO_T plane_type;
    BITMAP bmp;

    hBmpBig         = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BIG));
    hBmpBk          = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BK));
    hBmpExit        = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_EXIT));
    hBmpGameover    = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_GAMEOVER));
    hBmpLogo        = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LOGO));
    hBmpMiddle      = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MIDDLE));
    hBmpPlay        = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_PLAY));
    hBmpRestart     = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_RESTART));
    hBmpSmall       = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SMALL));
    hBmpStart       = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_START));

    // init the game module
    game_init();

    // set board size of the game
    GetObject(hBmpBk, sizeof(BITMAP), &bmp);
    ret = game_set_board_size(bmp.bmWidth, bmp.bmHeight);
    assert(ret == 0);

    // set plane number
    ret = game_set_plane_num(10);
    assert(ret == 0);

    // set small plane type
    GetObject(hBmpSmall, sizeof(BITMAP), &bmp);
    plane_type.type = SMALL;
    plane_type.size_x = bmp.bmWidth;
    plane_type.size_y = bmp.bmHeight / 2; // 2 pictures in one bmp file
    plane_type.max_hp = 1;
    plane_type.speed = 5;
    ret = game_set_plane_type_info(&plane_type);
    assert(ret == 0);

    // set middle plane type
    GetObject(hBmpMiddle, sizeof(BITMAP), &bmp);
    plane_type.type = MIDDLE;
    plane_type.size_x = bmp.bmWidth;
    plane_type.size_y = bmp.bmHeight / 3; // 3 pictures in one bmp file
    plane_type.max_hp = 2;
    plane_type.speed = 4;
    ret = game_set_plane_type_info(&plane_type);
    assert(ret == 0);

    // set big plane type
    GetObject(hBmpBig, sizeof(BITMAP), &bmp);
    plane_type.type = BIG;
    plane_type.size_x = bmp.bmWidth;
    plane_type.size_y = bmp.bmHeight / 4; // 4 pictures in one bmp file
    plane_type.max_hp = 3;
    plane_type.speed = 3;
    ret = game_set_plane_type_info(&plane_type);
    assert(ret == 0);
}

VOID UninitGame()
{
    DeleteObject(hBmpBig);
    DeleteObject(hBmpBk);
    DeleteObject(hBmpExit);
    DeleteObject(hBmpGameover);
    DeleteObject(hBmpLogo);
    DeleteObject(hBmpMiddle);
    DeleteObject(hBmpPlay);
    DeleteObject(hBmpRestart);
    DeleteObject(hBmpSmall);
    DeleteObject(hBmpStart);
}

VOID DrawWindow(HDC hdc)
{
    HDC        hDcTmp, hDcMem;
    HBITMAP    hBmpMem;
    BITMAP     bmp;
    int        i;
    HFONT      hf;
    LOGFONT    lf;
    RECT       rt;
    TCHAR      strScore[10];
    int        nBkWidth;
    int        nBkHeight;

    /* Use Double Buffering method to paint */
    
    /* background width and height */
    GetObject(hBmpBk, sizeof(BITMAP), &bmp);
    nBkWidth  = bmp.bmWidth;
    nBkHeight = bmp.bmHeight;

    /* hBmpMem is a tmp bitmap, used to store all kinds of things */
    hBmpMem    = CreateCompatibleBitmap(hdc, bmp.bmWidth, bmp.bmHeight);

    /* hDcMem is tmp dc in memory corresponding to hBmpMem */
    hDcMem = CreateCompatibleDC(hdc);
    SelectObject(hDcMem, hBmpMem);

    /* hDcTmp is another tmp dc, used to store widgets, like background, plane, button,etc. */
    hDcTmp = CreateCompatibleDC(hdc);
    SelectObject(hDcTmp, hBmpBk);

    BitBlt(hDcMem, 0, 0, nBkWidth, nBkHeight,
        hDcTmp, 0, 0, SRCCOPY);

    GAME_INFO_T * pGame = game_get_game_info();
    assert(pGame != NULL);

    switch (pGame->status)
    {
    case WELCOME:
        {
            /* Draw logo */
            SelectObject(hDcTmp, hBmpLogo);
            GetObject(hBmpLogo, sizeof(BITMAP), &bmp);

            TransparentBlt(
                hDcMem,
                30, 150, bmp.bmWidth, bmp.bmHeight,
                hDcTmp,
                0, 0, bmp.bmWidth, bmp.bmHeight,
                RGB(255, 255, 255));

            /* Draw start button */
            SelectObject(hDcTmp, hBmpStart);
            GetObject(hBmpStart, sizeof(BITMAP), &bmp);

            TransparentBlt(
                hDcMem,
                120, 350, bmp.bmWidth, bmp.bmHeight,
                hDcTmp,
                0, 0, bmp.bmWidth, bmp.bmHeight,
                RGB(255, 255, 255));
        }
        break;
    case RUN:
        {
            PLANE_INFO_T      * pPlane     = NULL;
            PLANE_TYPE_INFO_T * pPlaneType = NULL;
            int bmp_pos_y;

            /* Draw planes */
            for (i = 0; i<pGame->plane_num; i++)
            {
                pPlane = game_get_plane_info(i);
                assert(pPlane != NULL);
                pPlaneType = game_get_plane_type_info(pPlane->type);
                assert(pPlaneType != NULL);

                switch (pPlane->type) 
                {
                case SMALL:
                    SelectObject(hDcTmp, hBmpSmall);
                    break;
                case MIDDLE:
                    SelectObject(hDcTmp, hBmpMiddle);
                    break;
                case BIG:
                    SelectObject(hDcTmp, hBmpBig);
                    break;
                default:
                    assert(0);
                    break;
                }

                bmp_pos_y = pPlaneType->size_y * (pPlaneType->max_hp - pPlane->hp);

                TransparentBlt(
                    hDcMem,
                    pPlane->pos_x, pPlane->pos_y, pPlaneType->size_x, pPlaneType->size_y,
                    hDcTmp,
                    0, bmp_pos_y, pPlaneType->size_x, pPlaneType->size_y,
                    RGB(255, 255, 255));
            }
        }
        break;
    case GAMEOVER:
        {
            /* Draw game over pic */
            SelectObject(hDcTmp, hBmpGameover);
            GetObject(hBmpGameover, sizeof(BITMAP), &bmp);

            TransparentBlt(
                hDcMem,
                (nBkWidth - bmp.bmWidth)/2, (nBkHeight - bmp.bmHeight)/2, bmp.bmWidth, bmp.bmHeight,
                hDcTmp,
                0, 0, bmp.bmWidth, bmp.bmHeight,
                RGB(255, 255, 255));

            /* Draw restart button */
            SelectObject(hDcTmp, hBmpRestart);
            GetObject(hBmpRestart, sizeof(BITMAP), &bmp);

            TransparentBlt(
                hDcMem,
                100, 270, bmp.bmWidth, bmp.bmHeight,
                hDcTmp,
                0, 0, bmp.bmWidth, bmp.bmHeight,
                RGB(255, 255, 255));

            /* Draw exit button */
            SelectObject(hDcTmp, hBmpExit);
            GetObject(hBmpExit, sizeof(BITMAP), &bmp);

            TransparentBlt(
                hDcMem,
                100, 310, bmp.bmWidth, bmp.bmHeight,
                hDcTmp,
                0, 0, bmp.bmWidth, bmp.bmHeight,
                RGB(255, 255, 255));

            /* Draw score */
            lf.lfHeight = 25;
            lf.lfWidth = 25;
            lf.lfEscapement = 0;
            lf.lfItalic = FALSE;
            lf.lfUnderline = FALSE;
            lf.lfStrikeOut = FALSE;
            lf.lfCharSet = CHINESEBIG5_CHARSET;

            hf = CreateFontIndirect(&lf);
            SelectObject(hDcMem, hf);

            rt.left   = 150;
            rt.top    = 210;
            rt.right  = 300;
            rt.bottom = 260;

            _itot_s(pGame->score, strScore, 10);

            SetBkMode(hDcMem, TRANSPARENT);

            DrawText(hDcMem, strScore, -1, &rt, DT_CENTER);
        }
        break;
    default:
        break;
    }

    /* Blit everything onto screen  */
    BitBlt(hdc, 0, 0, nBkWidth, nBkHeight, hDcMem, 0, 0, SRCCOPY);

    DeleteObject(hBmpMem);
    DeleteDC(hDcMem);
    DeleteDC(hDcTmp);
}

BOOL CheckGameStartButtonDown(POINT ptMouse)
{
    RECT   rc;
    BITMAP bmp;

    GetObject(hBmpStart, sizeof(BITMAP), &bmp);

    rc.left   = 120;
    rc.top    = 350;
    rc.right  = bmp.bmWidth + rc.left;
    rc.bottom = bmp.bmHeight + rc.top;

    return PtInRect(&rc, ptMouse);
}

BOOL CheckGameReStartButtonDown(POINT ptMouse)
{
    RECT   rc;
    BITMAP bmp;

    GetObject(hBmpRestart, sizeof(BITMAP), &bmp);

    rc.left   = 100;
    rc.top    = 270;
    rc.right  = bmp.bmWidth + rc.left;
    rc.bottom = bmp.bmHeight + rc.top;

    return PtInRect(&rc, ptMouse);
}

BOOL CheckGameExitButtonDown(POINT ptMouse)
{
    RECT   rc;
    BITMAP bmp;

    GetObject(hBmpExit, sizeof(BITMAP), &bmp);

    rc.left   = 100;
    rc.top    = 310;
    rc.right  = bmp.bmWidth + rc.left;
    rc.bottom = bmp.bmHeight + rc.top;

    return PtInRect(&rc, ptMouse);
}

VOID TimerProc(HWND hWnd)
{
    game_tick_handler();

    GAME_INFO_T * pGame = game_get_game_info();
    assert(pGame != NULL);

    if (pGame->status != RUN)
    {
        KillTimer(hWnd, TICK_TIMER);
    }

    /* Play crash music if crash plane exists */
    int i = 0;
    for (i=0; i<pGame->plane_num; i++)
    {
        PLANE_INFO_T * pPlane = game_get_plane_info(i);
        assert(pPlane != NULL);
        if (pPlane->hp == 0)
        {
            PlaySound(
                MAKEINTRESOURCE(IDR_WAVE_CRASH),
                GetModuleHandle(NULL),
                SND_RESOURCE | SND_ASYNC);
            break;
        }
    }

    /* Invalidate window rectangle region, generate WM_PAINT */
    InvalidateRect(hWnd, NULL, FALSE);
}

VOID LButtonDownProc(HWND hWnd, LPARAM lParam)
{
    /* Mouse click coordinate */
    POINT        ptMouse;

    ptMouse.x = LOWORD(lParam);
    ptMouse.y = HIWORD(lParam);

    GAME_INFO_T * pGame = game_get_game_info();
    assert(pGame != NULL);

    switch (pGame->status) 
    {
    case WELCOME:
        if (CheckGameStartButtonDown(ptMouse))
        {
            game_start();
            SetTimer(hWnd, TICK_TIMER, 50, NULL);
        }
        break;
    case RUN:
        EVENT_INFO_T event;
        event.event = HIT;
        event.arg1 = ptMouse.x;
        event.arg2 = ptMouse.y;
        game_set_event(&event);
        break;
    case GAMEOVER:
        /* Check if game restart button is clicked */
        if (CheckGameReStartButtonDown(ptMouse))
        {
            UninitGame();
            InitGame();
            game_start();
            SetTimer(hWnd, TICK_TIMER, 50, NULL);
        }
        /* Check if game exit button is clicked */
        if (CheckGameExitButtonDown(ptMouse))
        {
            SendMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;
    default:
        assert(0);
        break;
    }
}
