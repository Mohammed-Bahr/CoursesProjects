#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <cmath>

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

POINT points[5];
int pointCount = 0;

COLORREF colors[5] = {
    RGB(255,0,0),
    RGB(0,255,0),
    RGB(0,0,255),
    RGB(255,255,0),
    RGB(255,0,255)
};

int WINAPI WinMain(HINSTANCE hThisInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszArgument,
    int nCmdShow)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);

    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);

    if (!RegisterClassEx(&wincl))
        return 0;

    hwnd = CreateWindowEx(
        0,
        szClassName,
        _T("Click To Draw Points"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        600,
        400,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);

    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

void DrawDDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++)
    {
        SetPixel(hdc, round(x), round(y), color);
        x += xInc;
        y += yInc;
    }
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case WM_LBUTTONDOWN:
    {
        if (pointCount < 5)
        {
            points[pointCount].x = LOWORD(lParam);
            points[pointCount].y = HIWORD(lParam);
            pointCount++;
        }

        InvalidateRect(hwnd, NULL, TRUE);
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
        SelectObject(hdc, hBrush);

        for (int i = 0; i < pointCount; i++)
        {
            Ellipse(hdc,
                points[i].x - 5,
                points[i].y - 5,
                points[i].x + 5,
                points[i].y + 5);
        }

        if (pointCount == 5)
        {
            DrawDDA(hdc, points[0].x, points[0].y, points[2].x, points[2].y, colors[0]);
            DrawDDA(hdc, points[2].x, points[2].y, points[4].x, points[4].y, colors[1]);
            DrawDDA(hdc, points[4].x, points[4].y, points[1].x, points[1].y, colors[2]);
            DrawDDA(hdc, points[1].x, points[1].y, points[3].x, points[3].y, colors[3]);
            DrawDDA(hdc, points[3].x, points[3].y, points[0].x, points[0].y, colors[4]);
        }

        DeleteObject(hBrush);
        EndPaint(hwnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}