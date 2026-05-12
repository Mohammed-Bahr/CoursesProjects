 // ============================================================
//  Filling Task - Computer Graphics
//  Draw a 4-point polygon, then click inside to flood fill
//  Based on: Prof. Reda A. El-Khoribi - Filling Algorithms
//  Uses Win32 + GDI (Visual C++ / MinGW)
// ============================================================

#include <windows.h>
#include <stack>
using namespace std;

// -------------------------------------------------------
//  Constants
// -------------------------------------------------------
#define MAX_POINTS   4
#define BORDER_COLOR RGB(229, 57, 53)   // red border
#define FILL_COLOR   RGB(66, 165, 245)  // blue fill

// -------------------------------------------------------
//  Globals
// -------------------------------------------------------
POINT  g_points[MAX_POINTS];
int    g_count      = 0;     // how many points placed so far
bool   g_closed     = false; // polygon complete?
bool   g_filled     = false; // already flood-filled?
HWND   g_hwnd       = NULL;

// -------------------------------------------------------
//  Draw a single pixel
// -------------------------------------------------------
void DrawPixel(HDC hdc, int x, int y, COLORREF color)
{
    SetPixel(hdc, x, y, color);
}

// -------------------------------------------------------
//  Bresenham line drawing (for polygon edges)
// -------------------------------------------------------
void DrawLine(HDC hdc, int x0, int y0, int x1, int y1, COLORREF color)
{
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        DrawPixel(hdc, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

// -------------------------------------------------------
//  Draw the polygon (edges + vertex dots)
// -------------------------------------------------------
void DrawPolygon(HDC hdc)
{
    if (g_count == 0) return;

    // edges
    for (int i = 1; i < g_count; i++)
        DrawLine(hdc, g_points[i-1].x, g_points[i-1].y,
                      g_points[i].x,   g_points[i].y,   BORDER_COLOR);

    // closing edge when polygon is complete
    if (g_closed)
        DrawLine(hdc, g_points[g_count-1].x, g_points[g_count-1].y,
                      g_points[0].x,          g_points[0].y,          BORDER_COLOR);

    // vertex dots
    for (int i = 0; i < g_count; i++)
    {
        int x = g_points[i].x, y = g_points[i].y;
        for (int dy = -3; dy <= 3; dy++)
            for (int dx = -3; dx <= 3; dx++)
                if (dx*dx + dy*dy <= 9)
                    DrawPixel(hdc, x+dx, y+dy, BORDER_COLOR);
    }
}

// -------------------------------------------------------
//  Non-Recursive Flood Fill  (from the PDF reference)
//  FloodFill(x, y, Cb=boundary, Cf=fill)
// -------------------------------------------------------
struct Vertex { int x, y; Vertex(int x,int y):x(x),y(y){} };

void NRFloodFill(HDC hdc, int startX, int startY,
                 COLORREF Cb, COLORREF Cf)
{
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    int W = rc.right, H = rc.bottom;

    stack<Vertex> S;
    S.push(Vertex(startX, startY));

    while (!S.empty())
    {
        Vertex v = S.top();
        S.pop();

        if (v.x < 0 || v.x >= W || v.y < 0 || v.y >= H) continue;

        COLORREF c = GetPixel(hdc, v.x, v.y);
        if (c == Cb || c == Cf) continue;

        SetPixel(hdc, v.x, v.y, Cf);

        S.push(Vertex(v.x + 1, v.y));
        S.push(Vertex(v.x - 1, v.y));
        S.push(Vertex(v.x,     v.y + 1));
        S.push(Vertex(v.x,     v.y - 1));
    }
}

// -------------------------------------------------------
//  Status text
// -------------------------------------------------------
void DrawStatus(HDC hdc, const char* msg)
{
    RECT rc; GetClientRect(g_hwnd, &rc);
    rc.bottom = 30;
    SetBkColor(hdc, RGB(240, 240, 240));
    SetTextColor(hdc, RGB(50, 50, 50));
    DrawTextA(hdc, msg, -1, &rc,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

// -------------------------------------------------------
//  Window Procedure
// -------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
                          WPARAM wParam, LPARAM lParam)
{
    static HDC      hdcMem  = NULL;
    static HBITMAP  hBmp    = NULL;
    static int      W = 800, H = 600;

    switch (msg)
    {
    // ---- create back-buffer ----
    case WM_CREATE:
    {
        g_hwnd = hwnd;
        HDC hdc = GetDC(hwnd);
        hdcMem  = CreateCompatibleDC(hdc);
        hBmp    = CreateCompatibleBitmap(hdc, W, H);
        SelectObject(hdcMem, hBmp);
        // white background
        HBRUSH hBr = CreateSolidBrush(RGB(255,255,255));
        RECT rc = {0,0,W,H};
        FillRect(hdcMem, &rc, hBr);
        DeleteObject(hBr);
        ReleaseDC(hwnd, hdc);
        break;
    }

    // ---- left mouse click ----
    case WM_LBUTTONDOWN:
    {
        int mx = LOWORD(lParam);
        int my = HIWORD(lParam);

        // --- place polygon points ---
        if (!g_closed)
        {
            g_points[g_count++] = { mx, my };

            // clear & redraw
            HBRUSH hBr = CreateSolidBrush(RGB(255,255,255));
            RECT rc = {0,0,W,H};
            FillRect(hdcMem, &rc, hBr);
            DeleteObject(hBr);

            DrawPolygon(hdcMem);

            if (g_count == MAX_POINTS)
            {
                g_closed = true;
                // redraw with closing edge
                DrawPolygon(hdcMem);
            }

            InvalidateRect(hwnd, NULL, FALSE);
        }
        // --- flood fill on click inside ---
        else if (!g_filled)
        {
            NRFloodFill(hdcMem, mx, my, BORDER_COLOR, FILL_COLOR);
            g_filled = true;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;
    }

    // ---- reset on right click ----
    case WM_RBUTTONDOWN:
    {
        g_count  = 0;
        g_closed = false;
        g_filled = false;
        HBRUSH hBr = CreateSolidBrush(RGB(255,255,255));
        RECT rc = {0,0,W,H};
        FillRect(hdcMem, &rc, hBr);
        DeleteObject(hBr);
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    }

    // ---- paint ----
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // blit back-buffer
        BitBlt(hdc, 0, 0, W, H, hdcMem, 0, 0, SRCCOPY);

        // status bar overlay
        const char* status;
        if (!g_closed)
        {
            static char buf[64];
            wsprintfA(buf, "Click to place point %d of %d",
                      g_count + 1, MAX_POINTS);
            status = buf;
        }
        else if (!g_filled)
            status = "Polygon ready!  Click INSIDE to flood fill";
        else
            status = "Done!  Right-click to reset";

        DrawStatus(hdc, status);
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        DeleteDC(hdcMem);
        DeleteObject(hBmp);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

// -------------------------------------------------------
//  WinMain
// -------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE,
                   LPSTR, int nCmdShow)
{
    const char CLASS[] = "PolygonFillClass";

    WNDCLASSA wc      = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(NULL, IDC_CROSS);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS;
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        CLASS,
        "Filling Task - Draw 4-Point Polygon & Click to Fill",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        820, 640,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
