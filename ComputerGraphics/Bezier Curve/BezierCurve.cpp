#include <windows.h>
#include <cmath>
#include <vector>

using namespace std;

struct Point {
    int x, y;
};

vector<Point> clicks;

// Draw pixel
void DrawPixel(HDC hdc, int x, int y, COLORREF color) {
    SetPixel(hdc, x, y, color);
}

// Bresenham Line Algorithm
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        DrawPixel(hdc, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx)  { err += dx; y1 += sy; }
    }
}

// Midpoint Circle Algorithm
void DrawCircle(HDC hdc, int xc, int yc, int r, COLORREF color) {
    int x = 0, y = r;
    int d = 1 - r;

    auto draw8 = [&](int x, int y) {
        DrawPixel(hdc, xc + x, yc + y, color);
        DrawPixel(hdc, xc - x, yc + y, color);
        DrawPixel(hdc, xc + x, yc - y, color);
        DrawPixel(hdc, xc - x, yc - y, color);
        DrawPixel(hdc, xc + y, yc + x, color);
        DrawPixel(hdc, xc - y, yc + x, color);
        DrawPixel(hdc, xc + y, yc - x, color);
        DrawPixel(hdc, xc - y, yc - x, color);
    };

    while (x <= y) {
        draw8(x, y);
        if (d < 0)
            d += 2 * x + 3;
        else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

// Fill ring (VERY IMPORTANT 🔥)
void FillRing(HDC hdc, int xc, int yc, int r1, int r2, COLORREF color) {
    int rMin = min(r1, r2);
    int rMax = max(r1, r2);

    for (int y = -rMax; y <= rMax; y++) {
        for (int x = -rMax; x <= rMax; x++) {
            int d = x * x + y * y;

            if (d >= rMin * rMin && d <= rMax * rMax) {
                DrawPixel(hdc, xc + x, yc + y, color);
            }
        }
    }
}

// Distance
int GetRadius(Point a, Point b) {
    return (int)sqrt((a.x - b.x) * (a.x - b.x) +
                     (a.y - b.y) * (a.y - b.y));
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    HDC hdc;

    switch (msg) {

    case WM_LBUTTONDOWN:
    {
        Point p = { LOWORD(lp), HIWORD(lp) };
        clicks.push_back(p);

        if (clicks.size() == 3) {
            hdc = GetDC(hwnd);

            Point center = clicks[0];
            Point p1 = clicks[1];
            Point p2 = clicks[2];

            int r1 = GetRadius(center, p1);
            int r2 = GetRadius(center, p2);

            // Fill ring
            FillRing(hdc, center.x, center.y, r1, r2, RGB(200, 0, 0));

            // Draw circles
            DrawCircle(hdc, center.x, center.y, r1, RGB(0, 0, 0));
            DrawCircle(hdc, center.x, center.y, r2, RGB(0, 0, 0));

            // Draw lines
            DrawLine(hdc, center.x, center.y, p1.x, p1.y, RGB(0, 0, 255));
            DrawLine(hdc, center.x, center.y, p2.x, p2.y, RGB(0, 255, 0));

            ReleaseDC(hwnd, hdc);

            clicks.clear(); // reset for next draw
        }
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

// WinMain
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpszClassName = "CircleApp";
    wc.hInstance = hInst;
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "CircleApp",
        "Circle Ring Task",
        WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
