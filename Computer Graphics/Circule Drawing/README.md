
> [!quote] You are the only person on Earth who can use your ability.
> — Zig Ziglar

```horizontal
![[Attachments/1.jpeg]]
---
![[Attachments/2.jpeg]]
```

### **The Goal**

You need to write a program that draws a colored ring (an annulus) with two lines inside it, based on where the user clicks their mouse on the screen.

### Step 1: The Inputs (3 Mouse Clicks)

Your program needs to wait for the user to click the mouse exactly three times:

- **Click 1:** This sets the **center point** for everything you are going to draw.
    
- **Click 2:** This sets the size (radius) of the **first circle**. The distance between Click 1 and Click 2 is the radius.
    
- **Click 3:** This sets the size (radius) of the **second circle**. The distance between Click 1 and Click 3 is the new radius.
    

_(Note: The instructions say you can use any mouse button event, like a Left Click `WM_LBUTTONUP` or Right Click `WM_RBUTTONUP`)._

### Step 2: The Outputs (What to Draw)

Once the user has clicked three times, your program must draw the following:

- **Two Circles:** Draw both circles using the exact same center point (Click 1), but with the two different radii from Clicks 2 and 3.
    
- **Fill the Gap:** You need to color in the space between the inner circle and the outer circle.
    
    - _How to fill it:_ The document says "fill... with other circles." This means you should write a loop that draws many circles right next to each other between the inner radius and the outer radius to make it look solidly colored.
        
    - _The colors:_ In the screenshot, the ring is divided into four different colored quarters (blue, red, green, black). You will likely need to use `if` conditions based on the $x$ and $y$ coordinates to change the color of the pixels depending on which quarter of the circle is being drawn.
        
- **Two Lines:** 
	* Draw one line starting from the center and ending at the exact spot of Click 2.
	    
    - Draw a second line starting from the center and ending at the exact spot of Click 3.
        

### Step 3: The Rules for Algorithms

Based on your Arabic notes from the professor:

- **For the Circles:** You are allowed to use **any** of the 3 circle-drawing algorithms you learned in class (for example: Direct/Cartesian, Polar, or Midpoint/Bresenham).
    
- **For the Lines:** You must use the specific line-drawing algorithm that was taught in the **previous class** (likely DDA or Bresenham's line algorithm).


 

## Code  
```cpp
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
```

 
## 🔥 4. FUNCTION-BY-FUNCTION DEEP ANALYSIS

### 1. `DrawPixel`

This is the smallest building block of computer graphics.

#### ✅ Function Purpose
- **Why it exists:** The operating system (Windows) draws complex shapes (buttons, text) for you automatically, but to draw custom graphics (like our specific math-based shapes), we need to manually control individual dots (pixels) on the screen.
- **Problem solved:** It abstracts the complex Windows API call `SetPixel` into a simple, readable function.

#### ✅ Parameters
- `HDC hdc`: **Handle to Device Context**. Think of this as your "Canvas" or "Paintbrush". Without this, Windows doesn't know *where* (which window) to draw.
- `int x, int y`: The coordinates. `x` is how far from the left, `y` is how far from the top.
- `COLORREF color`: The color. It's usually created using the `RGB(R, G, B)` macro.

#### ✅ Internal Variables
- None. It simply passes the data along.

#### ✅ Step-by-Step Logic
1.  The function receives the coordinates and color.
2.  It calls `SetPixel(hdc, x, y, color)`.
3.  Windows changes the electrical state of the pixel at `(x, y)` on the monitor to represent the color.

#### ✅ Example Run
- Call: `DrawPixel(hdc, 10, 10, RGB(255, 0, 0));`
- Result: A single red dot appears 10 pixels from the left and 10 pixels from the top of the window.

---

### 2. `DrawLine` (Bresenham's Algorithm)

#### ✅ Function Purpose
- **Why it exists:** We want to draw a line between any two points using only pixels. Since screens are a grid of squares, drawing a diagonal line is tricky—we have to choose which squares to color to approximate the line.
- **Problem solved:** It calculates exactly which pixels to turn on to create a straight line without needing slow floating-point math (decimals).

#### ✅ Parameters
- `x1, y1`: Start point coordinates.
- `x2, y2`: End point coordinates.
- `hdc, color`: Canvas and color.

#### ✅ Internal Variables
- `dx`: The total horizontal distance (absolute difference).
- `dy`: The total vertical distance (absolute difference).
- `sx` (step X): Direction of movement horizontally. If `x2` is to the right of `x1`, this is `1`. If left, `-1`.
- `sy` (step Y): Direction of movement vertically.
- `err`: The "Error" accumulator. This tracks how far off our pixel grid is from the perfect mathematical line.
> **Then** `err` is storing the accumulated deviation from the ideal mathematical line and helps decide the next pixel position using only integer arithmetic

#### ✅ Step-by-Step Logic
1.  **Calculate Distances:** Find `dx` and `dy`.
2.  **Set Direction:** Determine `sx` and `sy` so we know whether to add or subtract as we move.
3.  **Initialize Error:** `err = dx - dy`. This starts the balance.
4.  **Loop Forever:**
    a. Draw the current pixel at `(x1, y1)`.
    b. Check if we reached the end. If yes, break.
    c. **The Magic Logic:** We calculate `e2 = 2 * err`.
    d. If `e2 > -dy`: We are too low vertically. We move up/down (`y1 += sy`) and adjust the error.
    e. If `e2 < dx`: We are too far horizontally. We move left/right (`x1 += sx`) and adjust the error.

#### ✅ Example Run
Imagine drawing from `(0,0)` to `(5,2)`.
1.  `dx=5`, `dy=2`.
2.  The algorithm plots pixels. It mostly moves X, but every few pixels, it moves Y too, creating a stair-step effect that looks like a diagonal line.

---

### 3. `DrawCircle` (Midpoint Circle Algorithm)

#### ✅ Function Purpose
- **Why it exists:** Calculating `sin` and `cos` for every degree of a circle is very slow for a computer.
- **Problem solved:** It uses integer math to calculate only 1/8th of the circle (a slice of pizza) and mirrors it 8 times to complete the circle.

#### ✅ Parameters
- `xc, yc`: The center coordinates.
- `r`: The radius.
- `hdc, color`: Canvas and color.

#### ✅ Internal Variables
- `x, y`: Current plotting coordinates. Start at `x=0` (top of circle relative to center) and `y=r` (far right).
- `d`: Decision parameter. It tells us if the next pixel should be stepped diagonally or just horizontally.
- `draw8`: A **lambda function** (an inner function). It takes one point `(x, y)` and draws it in all 8 octants (mirrors it across X and Y axes).

#### ✅ Step-by-Step Logic
1.  Start at the top of the circle: `x=0`, `y=r`.
2.  Enter a loop while `x <= y`.
3.  Call `draw8(x, y)` to stamp the current pixel and its 7 mirrors.
4.  Check `d` (decision parameter):
    - If `d < 0`: The line is close to the circle's edge. Just move `x` forward.
    - Else: The line is drifting outside. Move `x` forward and `y` backward (downwards) to curve back in.
5.  Update `d` based on the decision.

#### ✅ Example Run
If radius is 5.
1.  Draw pixel at `(0, 5)`, `(5, 0)`, `(0, -5)`, etc.
2.  Move `x` to 1. Calculate if `y` should stay 5 or drop to 4.
3.  Draw those new mirrored points.
4.  Repeat until `x` meets `y`.

---

### 4. `FillRing`

#### ✅ Function Purpose
- **Why it exists:** To draw a solid shape (a ring or donut) rather than just an outline.
- **Problem solved:** Determines which specific pixels fall *inside* the outer circle but *outside* the inner circle.

#### ✅ Parameters
- `r1, r2`: Two radii (plural of radius).
- `xc, yc`: Center.

#### ✅ Internal Variables
- `rMin`, `rMax`: We figure out which radius is bigger and which is smaller so the code works regardless of the order you click.
- `x, y`: Loop counters representing the current pixel being checked relative to the center (from `-radius` to `+radius`).
- `d`: The "distance squared" of the current pixel from the center.

#### ✅ Step-by-Step Logic
1.  Determine `rMin` and `rMax`.
2.  Start a loop for `y` from `-rMax` to `rMax` (scanning top to bottom).
3.  Start a nested loop for `x` from `-rMax` to `rMax` (scanning left to right).
4.  Calculate `d = x*x + y*y`. 
	- This is the **squared distance** from the center to point (x, y):
$$d = x^2 + y^2$$
	- 	**Why squared?**
		- We compare with `rMin*rMin` and `rMax*rMax` (also squared)
		- **Avoids expensive `sqrt()` calculation!**
		- If $d \leq r^2$, then $\sqrt{d} \leq r$

5. **The Critical Check:** Is `d` between `rMin*rMin` and `rMax*rMax`?
    - Yes: Draw pixel.
    - No: Do nothing.
6.  Repeat until the whole square box around the circle is scanned.

#### ✅ Example Run
- `rMin` = 2, `rMax` = 4.
- We are checking pixel `(0, 3)`.
- `d = 0*0 + 3*3 = 9`.
- `rMin^2 = 4`, `rMax^2 = 16`.
- Is `4 <= 9 <= 16`? Yes. Draw pixel.

---

### 5. `GetRadius`

#### ✅ Function Purpose
- **Why it exists:** To calculate the distance between the center point (first click) and an edge point (second or third click).
- **Problem solved:** Applies the Pythagorean theorem to find the length of the hypotenuse of a triangle.

#### ✅ Parameters
- `Point a, Point b`: Two structs containing `x` and `y`.

#### ✅ Internal Variables
- None (the calculation is returned directly).

#### ✅ Step-by-Step Logic
1.  Calculate horizontal difference: `(a.x - b.x)`.
2.  Square it.
3.  Calculate vertical difference: `(a.y - b.y)`.
4.  Square it.
5.  Add them together.
6.  Return the square root (`sqrt`) of the sum. Cast to `int` because pixels must be whole numbers.

---

### 6. `WndProc` (Window Procedure)

#### ✅ Function Purpose
- **Why it exists:** This is the "brain" of the window. It listens for events (clicks, closing, moving) and tells the program what to do.
- **Problem solved:** Handles user interaction logic.

#### ✅ Parameters
- `HWND hwnd`: Handle to the specific window receiving the message.
- `UINT msg`: The ID of the message (e.g., `WM_LBUTTONDOWN` means Left Mouse Click).
- `WPARAM wp, LPARAM lp`: Extra data containing details (like exact X/Y coordinates of the mouse).

#### ✅ Internal Variables
- `HDC hdc`: The device context (canvas) we use to draw.
- `Point p`: A temporary struct to store the clicked location.

#### ✅ Step-by-Step Logic
1.  **Switch(msg):** Check what event happened.
2.  **Case `WM_LBUTTONDOWN` (User Clicked):**
    a. Extract X and Y from `lp` using `LOWORD` and `HIWORD`.
    b. Add this point to the `clicks` vector (list).
    c. **Check if we have 3 clicks:**
       - If yes:
         i.  Get the canvas (`hdc`).
         ii. Retrieve the 3 stored points (Center, P1, P2).
         iii. Calculate radii using `GetRadius`.
         iv. Call `FillRing` to draw the red donut.
         v.  Call `DrawCircle` to draw the black outlines.
         vi. Call `DrawLine` to draw the blue and green spokes.
         vii. Clear the `clicks` list so we can start fresh.
3.  **Case `WM_DESTROY`:** User clicked the "X" button. Call `PostQuitMessage(0)` to end the program.
4.  **Default:** Let Windows handle any messages we ignored.

---

### 7. `WinMain`

#### ✅ Function Purpose
- **Why it exists:** This is the entry point of the application (like `main()` in standard C++).
- **Problem solved:** Sets up the window, registers it with Windows, and starts the "Message Loop".

#### ✅ Parameters
- `HINSTANCE hInst`: Handle to the current program instance.
- `LPSTR`: Command line arguments (unused here).
- `int nCmdShow`: How the window should appear (maximized, minimized, etc.).

#### ✅ Internal Variables
- `WNDCLASS wc`: A struct that defines the window's behavior (cursor, background, etc.).
- `HWND hwnd`: The handle to the window created.
- `MSG msg`: A struct to hold messages from the queue.

#### ✅ Step-by-Step Logic
1.  **Setup Class:** Set `wc.lpfnWndProc` to our `WndProc` so Windows knows which function to send events to.
2.  **Register:** Tell Windows, "Hey, I have a new class of window called 'CircleApp'."
3.  **Create Window:** Actually build the window on screen using `CreateWindow`.
4.  **Show Window:** Make it visible.
5.  **Message Loop:** `while(GetMessage(...))`
    - This runs forever.
    - It waits for a click or key press.
    - It translates and dispatches the message to `WndProc`.
6.  Return 0 when the program exits.

---

## 🧮 5. MATHEMATICAL CONCEPTS

### The Distance Formula & Optimization

In `GetRadius`, we calculate the distance between two points $(x_1, y_1)$ and $(x_2, y_2)$. This is derived from the **Pythagorean Theorem** ($a^2 + b^2 = c^2$).

$$ Distance = \sqrt{(x_2 - x_1)^2 + (y_2 - y_1)^2} $$

### Why compare `x*x + y*y` instead of `sqrt`?

In the `FillRing` function, we check if a pixel is within a radius.
Mathematically, we want to check: $\text{Distance} \le \text{Radius}$.
This looks like: $\sqrt{x^2 + y^2} \le r$.

**The Problem:** Calculating the square root (`sqrt`) is a very "expensive" operation for a computer. It takes many CPU cycles. If we do this for 10,000 pixels, the program will lag.

**The Solution:** We can square both sides of the inequality. If $a \le b$, then $a^2 \le b^2$.
So, $\sqrt{x^2 + y^2} \le r$ becomes $x^2 + y^2 \le r^2$.

- **Original:** Calculate square root (slow), then compare.
- **Optimized:** Multiply $r$ by $r$ once (fast), compare against $x^2 + y^2$.

This is why the code uses `d = x*x + y*y` and compares it to `rMax*rMax`.

### The Circle Equation
The equation of a circle centered at $(0,0)$ is:
$$ x^2 + y^2 = r^2 $$
- Any point $(x,y)$ that satisfies this equation is exactly on the line of the circle.
- If $x^2 + y^2 < r^2$, the point is **inside**.
- If $x^2 + y^2 > r^2$, the point is **outside**.

---

## 🔴 6. RING FILLING LOGIC (CRITICAL 🔥)

Let's dissect this specific line from `FillRing`:

```cpp
if (d >= rMin*rMin && d <= rMax*rMax)
```

### Why this creates a Ring (Annulus)

Imagine a target (archery board).
1.  **`d <= rMax*rMax`**: This checks if the pixel is inside the **Big Circle** (the outer edge of the target). If this was the only check, we would fill the whole circle.
2.  **`d >= rMin*rMin`**: This checks if the pixel is **OUTSIDE** the **Small Circle** (the bullseye).

By combining them with `&&` (AND), we are saying:
> "I want pixels that are inside the big circle **BUT NOT** inside the small circle."

This "punches a hole" in the middle of the filled circle, leaving only the ring (the annulus).

### Visual Explanation (Layers)

Imagine we are stacking sheets of colored paper:

1.  **Layer 1 (Base):** We place a giant **Red Square** paper (covering `rMax` by `rMax`).
2.  **Filter 1 (`d <= rMax*rMax`):** We cut away everything outside the large radius. Now we have a **Red Solid Circle**.
3.  **Filter 2 (`d >= rMin*rMin`):** We take a circular cookie cutter of size `rMin` and remove the center from our Red Solid Circle.

What is left? Only the red ring.

### Step-by-Step Reasoning

Let's trace the logic for a single pixel `P` at coordinates `(5, 5)` relative to the center. Let's say `rMin = 5` and `rMax = 10`.

1.  **Calculate `d`:**
    $$ d = 5^2 + 5^2 = 25 + 25 = 50 $$

2.  **Calculate Boundaries:**
    $$ \text{InnerBoundary} = rMin^2 = 5^2 = 25 $$
    $$ \text{OuterBoundary} = rMax^2 = 10^2 = 100 $$

3.  **Evaluate Condition:**
    *   Is $d \ge 25$? Is $50 \ge 25$? **TRUE.** (Pixel is outside the inner hole).
    *   Is $d \le 100$? Is $50 \le 100$? **TRUE.** (Pixel is inside the outer edge).
    *   **Result:** `TRUE && TRUE` is **TRUE**.

4.  **Action:** `DrawPixel` is called. The pixel turns Red.

**Counter-example:** Let's check pixel `(2, 2)` (close to center).
1.  $$ d = 2^2 + 2^2 = 8 $$
2.  Is $8 \ge 25$? **FALSE.** It's inside the "hole".
3.  Action: Nothing happens. The pixel remains the background color.