
> [!quote] Ignorant men don't know what good they hold in their hands until they've flung it away.
> — Sophocles

### Problem overview

This code solves the problem of **visually demonstrating and interacting with cubic Bézier curve construction** in a minimal, from-scratch Win32 environment.

More specifically, it addresses:

**The core problem:** How do you let a user intuitively define a smooth parametric curve by placing control points, and immediately see the result rendered on screen — without any graphics library, game engine, or GUI framework?

**What it replaces/avoids:**

- No OpenGL, DirectX, or SDL — just raw Win32 and pixel-level drawing
- No pre-built curve widgets — the math is implemented directly from the Bézier formula

**The practical use case it models:** Tools like Illustrator, Inkscape, font editors, and animation software all use Bézier curves for path drawing. This code is essentially the minimal proof-of-concept for that interaction model: click to place control points → curve updates instantly.

**The specific constraints it handles:**

- Collecting exactly 4 user-defined points via mouse input
- Approximating a continuous curve by sampling `t` at fine increments and plotting individual pixels
- Resetting state cleanly on right-click without crashing or leaving stale data
- Running entirely within the Win32 message loop model

In short: it solves the problem of **rendering an interactive cubic Bézier curve using only native Windows API and the parametric curve formula**, serving as a foundational graphics programming exercise.

### Code

```cpp
#include <windows.h>  // Includes Windows API for window creation and graphics
#include <vector>     // Includes vector for dynamic array storage

using namespace std;  // Allows using std names without prefix

struct Point {        // Defines a 2D point structure
    int x, y;         // x and y coordinates
};

vector<Point> points; // Global vector to store control points

// Drawing function Bezier - Function to calculate Bezier curve point
Point Bezier(float t, Point p0, Point p1, Point p2, Point p3) {
    Point p; // Point to return
// Calculate x using cubic Bezier formula: B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
    p.x = (1 - t)*(1 - t)*(1 - t) * p0.x +      // (1-t)³ * P₀ₓ
          3 * (1 - t)*(1 - t)*t * p1.x +        // 3(1-t)²t * P₁ₓ
          3 * (1 - t)*t*t * p2.x +              // 3(1-t)t² * P₂ₓ
          t*t*t * p3.x;                         // t³ * P₃ₓ

    // Calculate y using same formula
    p.y = (1 - t)*(1 - t)*(1 - t) * p0.y +
          3 * (1 - t)*(1 - t)*t * p1.y +
          3 * (1 - t)*t*t * p2.y +
          t*t*t * p3.y;

    return p;         // Return calculated point
}

// رسم الكيرف - Function to draw the Bezier curve
void DrawBezier(HDC hdc) {
    if (points.size() < 4) return;  // Need 4 points for cubic Bezier

    // Draw curve by plotting points from t=0 to t=1
    for (float t = 0; t <= 1; t += 0.001) {  // Small step for smooth curve
        Point p = Bezier(t, points[0], points[1], points[2], points[3]);  // Get point on curve
        SetPixel(hdc, p.x, p.y, RGB(255, 0, 0));  // Draw red pixel (255,0,0)
    }
}

// Window Procedure - Handles window messages
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_LBUTTONDOWN: {  // Left mouse button clicked
            if (points.size() < 4) {  // Only accept if we need more points
                Point p;                          // Temporary point for click
                p.x = LOWORD(lParam);             // Get x-coordinate from lParam
                p.y = HIWORD(lParam);             // Get y-coordinate from lParam
                points.push_back(p);              // Add point to our vector

                HDC hdc = GetDC(hwnd);            // Get device context for drawing
                SetPixel(hdc, p.x, p.y, RGB(0, 0, 255));  // Draw BLUE point (0,0,255)
                ReleaseDC(hwnd, hdc);             // Release device context

                if (points.size() == 4) {         // If we now have 4 points
                    hdc = GetDC(hwnd);            // Get device context again
                    DrawBezier(hdc);              // Draw the Bezier curve
                    ReleaseDC(hwnd, hdc);         // Release device context
                }
            }
            break;
        }

        case WM_RBUTTONDOWN:  // Right mouse button clicked
            points.clear();                 // Clear all control points
            InvalidateRect(hwnd, NULL, TRUE);  // Clear screen by forcing redraw
            break;

        case WM_DESTROY:    // Window is being destroyed
            PostQuitMessage(0);  // Post quit message to end application
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);  // Default message handling
}

// WinMain - Application entry point
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASS wc = {};                 // Initialize window class structure
    wc.lpszClassName = "BezierWindow"; // Set class name
    wc.hInstance = hInst;             // Set instance handle
    wc.lpfnWndProc = WndProc;         // Set window procedure
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // Set arrow cursor

    RegisterClass(&wc);               // Register window class

    // Create the main window
    HWND hwnd = CreateWindow(
        "BezierWindow",               // Window class name
        "Bezier Curve",               // Window title
        WS_OVERLAPPEDWINDOW,          // Window style (with title bar/borders)
        100, 100,                     // Position x, y
        800, 600,                     // Width, height
        NULL, NULL,                   // No menu, no parent window
        hInst, NULL                   // Instance handle, no extra params
    );

    ShowWindow(hwnd, nCmdShow);       // Show the window
    UpdateWindow(hwnd);               // Force initial paint

    MSG msg;                          // Message structure
    // Message loop - get and dispatch messages until quit
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);       // Translate key messages
        DispatchMessage(&msg);        // Dispatch to window procedure
    }

    return 0;                         // Return exit code
}
```

This C++ code creates a simple, interactive Windows desktop application using the native Windows API (Win32). Its purpose is to let a user click four times on the screen to define "control points," and then it draws a smooth line called a **Cubic Bézier Curve** that connects and bends around those points.

Here is a step-by-step breakdown of how the code works.

---

### 1. Headers and Global Data

```C++
#include <windows.h>
#include <vector>
using namespace std;

struct Point {
    int x, y;
};
vector<Point> points;
```

- **`#include <windows.h>`**: This includes the massive library required to create native Windows applications. It gives you access to functions for creating windows, capturing mouse clicks, and drawing pixels.

- **`#include <vector>`**: Includes the standard C++ dynamic array (vector) so we can store a list of points whose size can change.

- **`struct Point`**: Defines a simple custom data type to hold a 2D coordinate ($x$ and $y$).

- **`vector<Point> points;`**: A global list that will store the coordinates of where the user clicks.

---

### 2. The Core Math: Calculating the Curve

```C++
Point Bezier(float t, Point p0, Point p1, Point p2, Point p3) { ... }
```

This function is the mathematical heart of the program. A cubic Bézier curve requires four control points ($P_0, P_1, P_2, P_3$).

To draw the curve, we use a variable **$t$** that represents "time" or "percentage along the curve," going from $0.0$ (the very beginning) to $1.0$ (the exact end). The mathematical formula for a cubic Bézier curve is:

$$B(t) = (1-t)^3 P_0 + 3(1-t)^2 t P_1 + 3(1-t) t^2 P_2 + t^3 P_3$$

The code breaks this formula down to calculate the $x$ and $y$ coordinates separately:

- At $t = 0$, the formula simplifies completely to $P_0$ (the start point).

- At $t = 1$, it simplifies to $P_3$ (the end point).

- For any value of $t$ in between, the formula calculates a point that is being "pulled" by $P_1$ and $P_2$.

---

### 3. Drawing the Curve

```C++
void DrawBezier(HDC hdc) {
    if (points.size() < 4) return;
    
    for (float t = 0; t <= 1; t += 0.001) {
        Point p = Bezier(t, points[0], points[1], points[2], points[3]);
        SetPixel(hdc, p.x, p.y, RGB(255, 0, 0));
    }
}
```

- **`HDC hdc`**: A "Handle to a Device Context." This is essentially Windows giving you a digital canvas to draw on.

- **`if (points.size() < 4) return;`**: A safety check. We can't draw a cubic curve without exactly 4 points.

- **The `for` loop**: This loop slowly steps $t$ from $0$ to $1$ in tiny increments (`0.001`). For each step, it calculates exactly where that point should be on the screen using the `Bezier` function, and then draws a red pixel using `SetPixel` (`RGB(255, 0, 0)` is solid red). Because the steps are so small, the individual pixels look like a solid, smooth line.

---

### `WndProc` — Full Breakdown

#### Function Signature

```cpp
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
```

|Part|Type|Meaning|
|---|---|---|
|`LRESULT`|`long` (Win32 alias)|Return value sent back to Windows after handling a message|
|`CALLBACK`|macro (`__stdcall`)|Tells the compiler Windows will call this function, not your code|
|`HWND hwnd`|`void*` (handle)|A handle (ID/pointer) to the specific window that received the message|
|`UINT msg`|`unsigned int`|A numeric code identifying which event occurred (click, close, etc.)|
|`WPARAM wParam`|`unsigned int*`|Extra message data — content depends on which `msg` was sent|
|`LPARAM lParam`|`long*`|More extra data — for mouse messages, this encodes the X/Y position|

---

#### `WM_LBUTTONDOWN` — Left Click Handler

```cpp
case WM_LBUTTONDOWN:
```

`WM_LBUTTONDOWN` is a **predefined Win32 constant** (just an integer like `0x0201`). When the user left-clicks anywhere in the window, Windows sends this message code into `msg`, which routes here.

---

```cpp
if (points.size() < 4)
```

- `points` — the global `vector<Point>` storing clicked coordinates
- `.size()` — returns `size_t` (unsigned integer), the current count of stored points
- Guards against accepting more than 4 clicks, since a cubic Bézier needs exactly 4

---

```cpp
Point p;
```

- `Point` — your custom struct `{ int x, y; }`
- `p` — a temporary local variable to hold the coordinates of this single click before pushing it into the vector

---

```cpp
p.x = LOWORD(lParam);
p.y = HIWORD(lParam);
```

- `lParam` — for mouse messages, Windows packs **both X and Y** into this single `long` value
- `LOWORD(lParam)` — macro that extracts the **lower 16 bits** → this is the **X coordinate**
- `HIWORD(lParam)` — macro that extracts the **upper 16 bits** → this is the **Y coordinate**
- Both return `WORD` (`unsigned short`, 0–65535), assigned into `int x` and `int y`

Visually:

```
lParam (32 bits):
[ Y coordinate (bits 16–31) | X coordinate (bits 0–15) ]
        HIWORD extracts ↑         LOWORD extracts ↑
```

---

```cpp
points.push_back(p);
```

- `.push_back()` — appends `p` to the end of the `vector<Point>`
- After this line, `points.size()` increases by 1

---

```cpp
HDC hdc = GetDC(hwnd);
```

- `HDC` — **Handle to Device Context**, a Win32 type (`void*` internally)
- A Device Context is Windows' abstraction for "a surface you can draw on"
- `GetDC(hwnd)` — requests a drawing context for your window. Returns `NULL` on failure
- You **must** release this after use or you leak a system resource

---

```cpp
SetPixel(hdc, p.x, p.y, RGB(0, 0, 255));
```

- `SetPixel(HDC, int x, int y, COLORREF color)` — draws a single pixel on the DC
- `p.x`, `p.y` — the coordinates extracted from the click
- `RGB(0, 0, 255)` — macro that packs red=0, green=0, blue=255 into a `COLORREF` (`DWORD`/`unsigned long`), producing **pure blue**
- This marks where the user clicked visually

---

```cpp
ReleaseDC(hwnd, hdc);
```

- Releases the Device Context back to Windows
- `hdc` becomes invalid after this line — do not use it again
- Pairing `GetDC` / `ReleaseDC` is mandatory, similar to `new` / `delete`

---

```cpp
if (points.size() == 4) {
    hdc = GetDC(hwnd);
    DrawBezier(hdc);
    ReleaseDC(hwnd, hdc);
}
```

- Checks if the vector now has exactly 4 points
- Acquires a **fresh** DC (the previous one was already released)
- Calls `DrawBezier(hdc)` which uses those 4 stored points to compute and render the red curve
- Releases the DC again immediately after

The reason it gets a new DC rather than reusing the old one is that `ReleaseDC` was already called — the old `hdc` is a dangling handle at that point.

---

#### `WM_RBUTTONDOWN` — Right Click Handler

```cpp
case WM_RBUTTONDOWN:
    points.clear();
    InvalidateRect(hwnd, NULL, TRUE);
    break;
```

- `WM_RBUTTONDOWN` — same idea as left click but for the right mouse button
- `points.clear()` — removes all elements from the vector, `.size()` becomes 0
- `InvalidateRect(hwnd, NULL, TRUE)` — tells Windows the entire window is "dirty" and needs repainting
  - `hwnd` — which window to invalidate
  - `NULL` — means invalidate the **entire** client area, not just a rectangle
  - `TRUE` — tells Windows to **erase the background** before repainting (clears the drawn pixels)
- Windows responds by sending a `WM_PAINT` message, which triggers a full redraw — effectively clearing all the blue dots and the red curve

---

#### `WM_DESTROY` — Window Close Handler

```cpp
case WM_DESTROY:
    PostQuitMessage(0);
    break;
```

- `WM_DESTROY` — sent when the window is being destroyed (user clicked the X)
- `PostQuitMessage(0)` — posts a `WM_QUIT` message to the message queue
  - The `0` is the **exit code** your app returns to the OS
  - This causes `GetMessage()` in `WinMain`'s message loop to return `false`, breaking the loop and ending the program

---

#### Default Handling

```cpp
return DefWindowProc(hwnd, msg, wParam, lParam);
```

- Any message your `switch` doesn't handle (window resize, mouse move, repaint, etc.) falls through here
- `DefWindowProc` — Windows' built-in default handler that does standard behavior for all those unhandled messages
- Without this, things like resizing, minimizing, and rendering the title bar would break

---

### `WinMain` — Full Breakdown

#### Function Signature

```cpp
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow)
```

|Part|Type|Meaning|
|---|---|---|
|`int`|return type|Exit code returned to the OS when the app closes|
|`WINAPI`|macro (`__stdcall`)|Calling convention — Windows calls this function to start your app|
|`HINSTANCE hInst`|`void*` (handle)|Handle to this running instance of your program in memory|
|`HINSTANCE` (unnamed)|`void*`|Previously used for a "previous instance" — always `NULL` in modern Windows, so unnamed and ignored|
|`LPSTR` (unnamed)|`char*`|Command line arguments as a string — ignored here, so unnamed|
|`int nCmdShow`|`int`|A flag from Windows telling the app how to display itself (maximized, minimized, normal, etc.)|

This is the Windows equivalent of `main()`. The OS calls this to start your program.

---

#### Setting Up the Window Blueprint

```cpp
WNDCLASS wc = {};
```

- `WNDCLASS` — a Win32 struct that acts as a **blueprint/template** for your window before it exists
- `= {}` — zero-initializes all fields, equivalent to `memset(&wc, 0, sizeof(wc))`. Safe default.

---

```cpp
wc.lpszClassName = "BezierWindow";
```

- `lpszClassName` — field of type `LPCSTR` (`const char*`)
- This is the **name/ID** you assign to your window class
- Think of it as the type name — `CreateWindow` will reference this string later to know which blueprint to use
- `lpsz` prefix is Hungarian notation: `lp` = long pointer, `sz` = null-terminated string

---

```cpp
wc.hInstance = hInst;
```

- `hInstance` — field of type `HINSTANCE`
- Links this window class to **your specific running program**
- Windows needs this to know which executable owns this window class

---

```cpp
wc.lpfnWndProc = WndProc;
```

- `lpfnWndProc` — field of type `WNDPROC` (a function pointer)
- `lpfn` prefix = long pointer to function
- This is the **most critical field** — it tells Windows which function to call when events happen to this window
- Without this, your window would exist but couldn't respond to anything

---

```cpp
wc.hCursor = LoadCursor(NULL, IDC_ARROW);
```

- `hCursor` — field of type `HCURSOR` (`void*` handle)
- `LoadCursor(NULL, IDC_ARROW)` — loads a built-in Windows cursor
  - First arg `NULL` means load from the system, not from your own `.exe` resources
  - `IDC_ARROW` is a predefined constant for the standard arrow cursor
- Without this, the cursor would disappear when hovering over your window

---

#### Registering and Creating the Window

```cpp
RegisterClass(&wc);
```

- Takes a `const WNDCLASS*` pointer to your blueprint struct
- Submits the blueprint to Windows OS so it knows about `"BezierWindow"` as a valid window type
- Must happen **before** `CreateWindow` — you can't build from a blueprint Windows doesn't know about yet
- Returns an `ATOM` (a unique ID), but it's ignored here since failure handling is omitted for brevity

---

```cpp
HWND hwnd = CreateWindow(
    "BezierWindow",       // Which blueprint to use
    "Bezier Curve",       // Text shown in the title bar
    WS_OVERLAPPEDWINDOW,  // Style flags
    100, 100,             // X, Y position on screen
    800, 600,             // Width, Height in pixels
    NULL,                 // No parent window
    NULL,                 // No menu
    hInst,                // Which program owns this window
    NULL                  // No extra creation data
);
```

- `HWND` — **Handle to a Window** (`void*`), the ID you use to reference this window in every future call
- `"BezierWindow"` — must match `wc.lpszClassName` exactly, this is how Windows finds the blueprint
- `"Bezier Curve"` — `LPCSTR`, the title bar text
- `WS_OVERLAPPEDWINDOW` — a `DWORD` bitmask combining several style flags:

```
WS_OVERLAPPEDWINDOW =
    WS_OVERLAPPED    → basic window
  | WS_CAPTION       → title bar
  | WS_SYSMENU       → the X/minimize/maximize buttons
  | WS_THICKFRAME    → resizable border
  | WS_MINIMIZEBOX   → minimize button
  | WS_MAXIMIZEBOX   → maximize button
```

- `100, 100` — `int` X and Y screen coordinates of the top-left corner
- `800, 600` — `int` width and height in pixels
- First `NULL` — no parent window (`HWND`)
- Second `NULL` — no menu bar (`HMENU`)
- `hInst` — ties this window instance to your program
- Last `NULL` — no extra data passed to `WM_CREATE` (`LPVOID`)

---

#### Making the Window Visible

```cpp
ShowWindow(hwnd, nCmdShow);
```

- `ShowWindow(HWND, int)` — tells Windows to make the window visible
- `hwnd` — which window to show
- `nCmdShow` — the display flag passed in from the OS via `WinMain`. Common values:

|Value|Meaning|
|---|---|
|`SW_SHOWNORMAL` (1)|Normal window|
|`SW_SHOWMAXIMIZED` (3)|Start maximized|
|`SW_SHOWMINIMIZED` (2)|Start minimized|

---

```cpp
UpdateWindow(hwnd);
```

- Forces Windows to immediately send a `WM_PAINT` message to your window
- Without this, the window might appear blank until the first user interaction
- Ensures the initial frame is drawn right away

---

#### The Message Loop

```cpp
MSG msg;
```

- `MSG` — a Win32 struct that holds a single event/message from the OS
- Fields inside it: `HWND hwnd`, `UINT message`, `WPARAM wParam`, `LPARAM lParam`, `DWORD time`, `POINT pt`
- Declared outside the loop so it's not reallocated on every iteration

---

```cpp
while (GetMessage(&msg, NULL, 0, 0)) {
```

- `GetMessage(MSG*, HWND, UINT min, UINT max)` — **blocks** (waits) until a message arrives, then fills the `MSG` struct
  - First arg `&msg` — pointer to the struct to fill
  - `NULL` — retrieve messages for **any** window belonging to this thread
  - `0, 0` — no filter, retrieve **all** message types
- Returns `non-zero` normally, returns `0` when `WM_QUIT` is received — which breaks the loop and ends the program
- This loop is what keeps the program alive and responsive

---

```cpp
TranslateMessage(&msg);
```

- Converts raw keyboard key-down messages (`WM_KEYDOWN`) into character messages (`WM_CHAR`)
- For a mouse-only app like this it has no real effect, but it's standard boilerplate in every Win32 message loop

---

```cpp
DispatchMessage(&msg);
```

- Takes the filled `MSG` struct and routes it to the correct `WndProc` function
- This is the line that actually calls `WndProc(hwnd, msg, wParam, lParam)` behind the scenes

---

#### Return

```cpp
return 0;
```

- Returns the exit code to the OS
- `0` conventionally means **success**
- When `GetMessage` receives `WM_QUIT`, the loop exits and this line runs, cleanly terminating the process

