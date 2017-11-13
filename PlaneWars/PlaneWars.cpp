/******************************************************
* File name:    PlaneWars.cpp
* Auther:       RedMonkey
* Date:         2017/11/21
* Description:  All function implementation, including entry point into the applicaion
*****************************************************/

#include "stdafx.h"
#include "PlaneWars.h"

/* Global variables */
HINSTANCE         g_hInst;                         /* Defined by default. Current Instance           */
WCHAR             g_szTitle[MAX_LOADSTRING];       /* Defined by default. Title name                 */
WCHAR             g_szWindowClass[MAX_LOADSTRING]; /* Defined by default. Class name for main window */
GAMESTATUS        g_emGameStatus;                  /* Game status                */
                                                   /* List all bmp resoures here */
UINT              g_uiBmpNames[] = { IDB_BK,
                                     IDB_SMALL,
                                     IDB_MIDDLE,
                                     IDB_BIG,
                                     IDB_LOGO,
                                     IDB_START,
                                     IDB_GAMEOVER,
                                     IDB_RESTART,
                                     IDB_EXIT
                                    };

HBITMAP           g_hBmp[BMPCOUNT];                /* Bmp resource handle        */
PLANE             g_tPlaneArray[PLANECOUNT];       /* Plane detailed info        */
BOOL              g_bBigAdded;                     /* If big plane is added      */
UINT              g_uiScore;                       /* Game score                 */
UINT              g_uiMusicDeviceID;               /* Music device id            */


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    /* TODO: put code here */

    /* Initialize global strings */
    LoadStringW(hInstance, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PLANEWARS, g_szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    /* Application initialization */
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PLANEWARS));

    MSG msg;

    /* Main message loop */
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


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PLANEWARS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = g_szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


/******************************************************
* Function name:  InitInstance()
* Purpose:        Save current instance handle,create and show main window
* Input:
*    hInstance
*    nCmdShow
* Output:         N/A
* Return:         Window creation result
*****************************************************/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance;

   int cxScreen = GetSystemMetrics(SM_CXSCREEN);
   int cyScreen = GetSystemMetrics(SM_CYSCREEN);

   HWND hWnd = CreateWindowW(g_szWindowClass,
                             g_szTitle, 
                             WS_OVERLAPPEDWINDOW & (~WS_MAXIMIZEBOX) & (~WS_SIZEBOX),
                             (cxScreen - WNDWIDTH) / 2, 
                             (cyScreen - WNDHEIGHT + 100) / 2,
                             WNDWIDTH, 
                             WNDHEIGHT + 35,
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


/******************************************************
* Function name:     WndProc()
* Purpose:           Handle function to main window messages
* Input:
*    hWnd
*    message
*    wParam
*    lParam
* Output:            N/A
* Return:            Function result
* Description:
*    WM_CREATE:      Create window
*    WM_PAINT:       Draw main window
*    WM_START:       Start game,user defined message
*    WM_TIMER:       Timer message
*    WM_LBUTTONDOWN: Mouse left button down message
*    WM_COMMAND:     Menu message
*    WM_DESTROY:     Exit and return the application
*****************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            InitGame(lParam);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            DrawWindow(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_START:
        {
            memset(g_tPlaneArray, 0, sizeof(g_tPlaneArray));
            g_bBigAdded    = FALSE;
            g_emGameStatus = RUN;
            g_uiScore      = 0;

            UpdatePlaneInfo(TRUE, 0, 2);

            /* Send WM_TIMER every 50ms */
            SetTimer(hWnd, TIMER, 50, NULL);
        }
        break;
    case WM_TIMER:
        {
            TimerProc(hWnd);
        }
        break;
    case WM_LBUTTONDOWN:
        {
            LButtonDownProc(hWnd, lParam);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            /* Analyze menu cases */
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        {
            DestroyGame();
            PostQuitMessage(0);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}



/* Below are user defined functions */

/******************************************************
* Function name:  InitGame()
* Purpose:        Global variables initialization
* Input:          lParam
* Output:         N/A
* Return:         N/A
*****************************************************/
VOID InitGame(LPARAM lParam)
{
    // Initialize global variables
    g_emGameStatus = WELCOME;

    // Load bitmap resource
    for (int i = 0; i < BMPCOUNT; i++)
    {
        g_hBmp[i] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(g_uiBmpNames[i]));
    }

    // Open mci device
    MCI_OPEN_PARMS mciOpen;
    mciOpen.lpstrDeviceType = _T("mpegvideo");
    mciOpen.lpstrElementName = _T("..//res//Crash.mp3");

    mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)&mciOpen);

    g_uiMusicDeviceID = mciOpen.wDeviceID;

}

/******************************************************
* Function name:  DestroyGame()
* Purpose:        Resource release
* Input:          N/A
* Output:         N/A
* Return:         N/A
*****************************************************/
VOID DestroyGame()
{
    // Release bitmap resource
    for (int i = 0; i< BMPCOUNT; i++)
    {
        DeleteObject(g_hBmp[i]);
    }

    // Close mci device
    mciSendCommand(g_uiMusicDeviceID, MCI_CLOSE, NULL, NULL);
}

/******************************************************
* Function name:  UpdatePlaneInfo()
* Purpose:        Update Planes detailed info in g_tPlaneArray[]
* Input:          
*    bReset:      If TRUE,update all planes info, if FALSE, update designated plane info
*    uiIndex:     Index of plane needs to update. Only valid when bReset is FALSE. 
*    uiSpeed:     Move speed
* Output:         N/A
* Return:         N/A 
*****************************************************/
VOID UpdatePlaneInfo(BOOL bReset, UINT uiIndex, UINT uiSpeed)
{
    UINT        i, idx, begin, end;
    BITMAP      bmp;

    assert(uiIndex < PLANECOUNT);

    if (bReset)
    {
        begin = 0;
        end   = PLANECOUNT;
    }
    else
    {
        begin = uiIndex;
        end   = uiIndex + 1;
    }

    srand(GetTickCount());

    for (i = begin; i< end; i++)
    {
        /* idx is a random value: 0 - small, 1 - middle, 2 - big */
        idx = rand() % 3;

        /* Ensure there is only one big plane, idx == 2 only once */
        while ((idx == 2) && g_bBigAdded)
        {
            idx = rand() % 3;
        }

        if ((idx == 2) && !g_bBigAdded)
        {
            g_bBigAdded = TRUE;
        }

        GetObject(g_hBmp[idx + 1], sizeof(BITMAP), &bmp);

        g_tPlaneArray[i].hBmp      = g_hBmp[idx + 1];
        g_tPlaneArray[i].type      = (PLANETYPE)idx;
        g_tPlaneArray[i].size.cx   = bmp.bmWidth;

        /* If small, divided by 2. If middle, divided by 3. If big, devided by 4 */
        g_tPlaneArray[i].size.cy   = bmp.bmHeight / (idx +2);
        g_tPlaneArray[i].pos.x     = rand() % (WNDWIDTH - g_tPlaneArray[i].size.cx);

        /* Initial position is outside the client window */
        g_tPlaneArray[i].pos.y     = - g_tPlaneArray[i].size.cy - rand() % (g_tPlaneArray[i].size.cy * (4 - idx));

        /* If speed is 2, moveSpeed is 2(small) ,1(middle),1(big) */
        g_tPlaneArray[i].moveSpeed = (uiSpeed - idx) ? (uiSpeed - idx) : 1;
    }
}


/******************************************************
* Function name:  DrawWindow()
* Purpose:        Draw game window
* Input:          hdc
* Output:         N/A
* Return:         N/A
* Description:    The game window has 3 different pages.
*                 1. Welcome page
*                 Content: background + logo + start button
*                 2. Run page
*                 Content: background + planes
*                 3. Gameover page
*                 Content: background + restart button + exit button + score
*****************************************************/
VOID DrawWindow(HDC hdc)
{
    HDC        hdcMem, hdcTmp;
    HBITMAP    hBmpMem;
    BITMAP     bmp;
    UINT       i;
    HFONT      hf;
    LOGFONT    lf;
    RECT       rt;
    TCHAR      strScore[10];

    /* Use Double Buffering method to paint */

    /* hBmpMem is a tmp bitmap, used to store all kinds of things */
    hBmpMem    = CreateCompatibleBitmap(hdc, WNDWIDTH, WNDHEIGHT);

    /* hdcMem is tmp dc in memory corresponding to hBmpMem */
    hdcMem = CreateCompatibleDC(hdc);

    SelectObject(hdcMem, hBmpMem);


    /* hdcTmp is another tmp dc, used to store widgets, like background, plane, button,etc. */
    hdcTmp = CreateCompatibleDC(hdc);


    /* Draw background */
    SelectObject(hdcTmp, g_hBmp[0]);

    BitBlt(hdcMem, 0, 0, WNDWIDTH, WNDHEIGHT,
        hdcTmp, 0, 0, SRCCOPY);

    switch (g_emGameStatus)
    {
    case WELCOME:
        {
            /* Draw logo */
            SelectObject(hdcTmp, g_hBmp[4]);
            GetObject(g_hBmp[4], sizeof(BITMAP), &bmp);

            TransparentBlt(
                hdcMem,
                30, 150, bmp.bmWidth, bmp.bmHeight,
                hdcTmp,
                0, 0, bmp.bmWidth, bmp.bmHeight,
                RGB(255, 255, 255));

            /* Draw start button */
            SelectObject(hdcTmp, g_hBmp[5]);
            GetObject(g_hBmp[5], sizeof(BITMAP), &bmp);

            TransparentBlt(
                hdcMem,
                120, 350, bmp.bmWidth, bmp.bmHeight,
                hdcTmp,
                0, 0, bmp.bmWidth, bmp.bmHeight,
                RGB(255, 255, 255));
        }
        break;
    case RUN:
        {
            /* Draw planes */
            for (i = 0; i<PLANECOUNT; i++)
            {
                SelectObject(hdcTmp, g_tPlaneArray[i].hBmp);

                TransparentBlt(
                    hdcMem, g_tPlaneArray[i].pos.x, g_tPlaneArray[i].pos.y,
                    g_tPlaneArray[i].size.cx, g_tPlaneArray[i].size.cy,
                    hdcTmp, 0, g_tPlaneArray[i].hitCounter * g_tPlaneArray[i].size.cy,
                    g_tPlaneArray[i].size.cx, g_tPlaneArray[i].size.cy,
                    RGB(255, 255, 255));

                if (g_tPlaneArray[i].hitCounter == g_tPlaneArray[i].type + 1)
                {
                    g_tPlaneArray[i].hitCounter = -1;
                }
            }
        }
        break;
    case GAMEOVER:
        {
            /* Draw game over pic */
            SelectObject(hdcTmp, g_hBmp[6]);
            GetObject(g_hBmp[6], sizeof(BITMAP), &bmp);

            TransparentBlt(
                hdcMem,
                (WNDWIDTH - bmp.bmWidth)/2, (WNDHEIGHT - bmp.bmHeight)/2, bmp.bmWidth, bmp.bmHeight,
                hdcTmp,
                0, 0, bmp.bmWidth, bmp.bmHeight,
                RGB(255, 255, 255));

            /* Draw restart button */
            SelectObject(hdcTmp, g_hBmp[7]);
            GetObject(g_hBmp[7], sizeof(BITMAP), &bmp);

            TransparentBlt(
                hdcMem,
                100, 270, bmp.bmWidth, bmp.bmHeight,
                hdcTmp,
                0, 0, bmp.bmWidth, bmp.bmHeight,
                RGB(255, 255, 255));

            /* Draw exit button */
            SelectObject(hdcTmp, g_hBmp[8]);
            GetObject(g_hBmp[8], sizeof(BITMAP), &bmp);

            TransparentBlt(
                hdcMem,
                100, 310, bmp.bmWidth, bmp.bmHeight,
                hdcTmp,
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
            SelectObject(hdcMem, hf);

            rt.left   = 150;
            rt.top    = 210;
            rt.right  = 300;
            rt.bottom = 260;

            _itot_s(g_uiScore, strScore, 10);

            SetBkMode(hdcMem, TRANSPARENT);

            DrawText(hdcMem, strScore, -1, &rt, DT_CENTER);
        }
        break;
    default:
        break;
    }


    /* Blit everything onto screen  */
    BitBlt(hdc, 0, 0, WNDWIDTH, WNDHEIGHT, hdcMem, 0, 0, SRCCOPY);


    DeleteObject(hBmpMem);
    DeleteDC(hdcMem);
    DeleteDC(hdcTmp);
}


/******************************************************
* Function name:  TimerProc()
* Purpose:        Handle function to timer message
* Input:          hWnd
* Output:         N/A
* Return:         N/A
* Description:    Change plane Y postion and check if reaches to the bottom
*****************************************************/
VOID TimerProc(HWND hWnd)
{
    for (UINT i = 0; i < PLANECOUNT; i++)
    {
        /* After crash picture is showed, update score and plane info in crash postion */
        if (g_tPlaneArray[i].hitCounter == -1)
        {
            g_uiScore++;

            g_tPlaneArray[i].hitCounter = 0;

            if (g_tPlaneArray[i].type == BIG)
            {
                g_bBigAdded = FALSE;
            }

            UpdatePlaneInfo(FALSE, i, 3);
        }

        /* Move down moveSpeed every 50 ms */
        g_tPlaneArray[i].pos.y += g_tPlaneArray[i].moveSpeed;

        if (g_tPlaneArray[i].pos.y + g_tPlaneArray[i].size.cy >= WNDHEIGHT)
        {
            KillTimer(hWnd, TIMER);

            g_emGameStatus = GAMEOVER;

            break;
        }
    }


    /* Invalidate window rectangle region, generate WM_PAINT */
    InvalidateRect(hWnd, NULL, FALSE);
}


/******************************************************
* Function name:  LButtonDownProc()
* Purpose:        Handle function to mouse left button down message 
* Input:
*    hWnd
*    lParam
* Output:         N/A
* Return:         N/A
*****************************************************/
VOID LButtonDownProc(HWND hWnd, LPARAM lParam)
{
    UINT         index = 0;

    /* Mouse click coordinate */
    POINT        ptMouse; 

    ptMouse.x = LOWORD(lParam);
    ptMouse.y = HIWORD(lParam);


    switch (g_emGameStatus)
    {
    case WELCOME:
        {
            /* Check if game start button is clicked */
            if (CheckGameStartButtonDown(ptMouse))
            {
                SendMessage(hWnd, WM_START, 0, 0);
            }
        }
        break;
    case RUN:
        {
            /* Check if a plane is hit */
            Hit(ptMouse);
        }
        break;
    case GAMEOVER:
        {
            /* Check if game restart button is clicked */
            if (CheckGameReStartButtonDown(ptMouse))
            {
                SendMessage(hWnd, WM_START, 0, 0);
            }

            /* Check if game exit button is clicked */
            if (CheckGameExitButtonDown(ptMouse))
            {
                SendMessage(hWnd, WM_CLOSE, 0, 0);
            }
        }
        break;
    default:
        break;
    }
}


/******************************************************
* Function name:  Hit()
* Purpose:        Check if a plane is hit
* Input:          ptMouse
* Output:         N/A
* Return:         N/A
*****************************************************/
VOID Hit(POINT ptMouse)
{
    RECT           rc;
    MCI_PLAY_PARMS mciPlay;

    for (UINT i = 0; i<PLANECOUNT; i++)
    {
        rc.left   = g_tPlaneArray[i].pos.x;
        rc.top    = g_tPlaneArray[i].pos.y;
        rc.right  = rc.left + g_tPlaneArray[i].size.cx;
        rc.bottom = rc.top + g_tPlaneArray[i].size.cy;

        if (PtInRect(&rc, ptMouse))
        {
            g_tPlaneArray[i].hitCounter++;

            if (g_tPlaneArray[i].hitCounter > g_tPlaneArray[i].type + 1)
            {
                g_tPlaneArray[i].hitCounter = g_tPlaneArray[i].type + 1;
            }

            /* Play crash music */
            mciPlay.dwFrom = 0;
            mciSendCommand(g_uiMusicDeviceID, MCI_PLAY, MCI_FROM, (DWORD_PTR)&mciPlay);

            break;
        }
    }
}


BOOL CheckGameStartButtonDown(POINT ptMouse)
{
    RECT   rc;
    BITMAP bmp;

    GetObject(g_hBmp[5], sizeof(BITMAP), &bmp);

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

    GetObject(g_hBmp[7], sizeof(BITMAP), &bmp);

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

    GetObject(g_hBmp[8], sizeof(BITMAP), &bmp);

    rc.left   = 100;
    rc.top    = 310;
    rc.right  = bmp.bmWidth + rc.left;
    rc.bottom = bmp.bmHeight + rc.top;

    return PtInRect(&rc, ptMouse);
}