/**
 * ============================================================
 *  Non-Convex Polygon Filling — Scanline + Active Edge List
 *  Platform : Linux (SDL2)
 *  Author   : Bahr – CG Study Project
 * ============================================================
 *
 * Algorithm Overview (from your notes):
 *   1. Build Edge Table (ET): array of linked lists indexed by y_min.
 *   2. For each scanline y (bottom to top of polygon):
 *       a. Move edges whose y_min == y from ET → AEL
 *       b. Sort AEL by current x-intersection
 *       c. Fill pixels between pairs: (AEL[0]↔AEL[1]), (AEL[2]↔AEL[3]), …
 *       d. Remove edges where y_max == y (edge is finished)
 *       e. Update x for surviving edges: x += invSlope (i.e. 1/m)
 */

#include <SDL2/SDL.h>
#include <algorithm>
#include <vector>
#include <list>
#include <cmath>
#include <iostream>
#include <string>

/* ─── Window / rendering constants ────────────────────────────────────── */
constexpr int WIN_W  = 900;
constexpr int WIN_H  = 700;
constexpr int FPS    = 60;

/* How many milliseconds to pause between scanlines in animation mode */
constexpr int ANIM_DELAY_MS = 8;

/* ─── Color helpers ───────────────────────────────────────────────────── */
struct Color { Uint8 r, g, b, a; };

constexpr Color COL_BG         = {15,  15,  25,  255};
constexpr Color COL_OUTLINE    = {255, 255, 255, 255};
constexpr Color COL_FILL       = {80,  160, 255, 200};
constexpr Color COL_SCANLINE   = {255, 100,  50, 255};
constexpr Color COL_FILL2      = {100, 220, 130, 200};  // second demo polygon
constexpr Color COL_OUTLINE2   = {200, 255, 200, 255};

/* ─── Data structures ─────────────────────────────────────────────────── */

/**
 * Edge entry stored in the Edge Table (ET) and Active Edge List (AEL).
 * Matches the structure from your lecture notes:
 *   [x, y_max, 1/m, next_pointer]
 */
struct Edge {
    int    y_max;       // scanline at which this edge expires
    double x;           // current x-intersection with the scanline
    double invSlope;    // Δx / Δy  — how much x moves per y step
};

/** One 2-D point (integer screen coordinates). */
struct Point { int x, y; };

/* ─── Edge Table builder ──────────────────────────────────────────────── */

/**
 * buildEdgeTable()
 *
 * For every non-horizontal edge of the polygon, create an Edge record
 * and insert it into ET[ y_min ] (the bucket for the edge's lower y).
 *
 * @param verts   Polygon vertices in order
 * @param ET      Output: vector of buckets, one per scanline row
 * @param yMin    Output: lowest y among all vertices
 * @param yMax    Output: highest y among all vertices
 */
void buildEdgeTable(const std::vector<Point>& verts,
                    std::vector<std::list<Edge>>& ET,
                    int& yMin, int& yMax)
{
    int n = static_cast<int>(verts.size());
    yMin  = WIN_H;
    yMax  = 0;

    /* Find overall y extent first */
    for (auto& p : verts) {
        if (p.y < yMin) yMin = p.y;
        if (p.y > yMax) yMax = p.y;
    }

    ET.assign(WIN_H + 1, std::list<Edge>());

    /* Process each edge: vertex[i] → vertex[(i+1) % n] */
    for (int i = 0; i < n; i++) {
        Point p1 = verts[i];
        Point p2 = verts[(i + 1) % n];

        /* Skip horizontal edges — they don't contribute intersections */
        if (p1.y == p2.y) continue;

        /* Ensure p1 is the bottom point (smaller y in screen coords) */
        if (p1.y > p2.y) std::swap(p1, p2);

        /*
         * y_min of this edge is p1.y
         * y_max is p2.y  (the edge is "alive" while y < y_max)
         * Starting x is p1.x (the x at the bottom of the edge)
         * invSlope = (x2 - x1) / (y2 - y1)
         */
        Edge e;
        e.y_max    = p2.y;
        e.x        = static_cast<double>(p1.x);
        e.invSlope = static_cast<double>(p2.x - p1.x) /
                     static_cast<double>(p2.y - p1.y);

        ET[p1.y].push_back(e);
    }
}

/* ─── AEL update helpers ──────────────────────────────────────────────── */

/**
 * updateAEL()
 *
 * Called once per scanline y. It:
 *   1. Moves all edges from ET[y] into AEL
 *   2. Sorts AEL by current x-intersection (left → right)
 *   3. Removes edges that have expired (y == y_max)
 *   4. Updates x for surviving edges by adding invSlope
 *
 * @param AEL     The active edge list (modified in place)
 * @param ET      Edge table (ET[y] is consumed)
 * @param y       Current scanline
 */
void updateAEL(std::list<Edge>& AEL,
               std::vector<std::list<Edge>>& ET,
               int y)
{
    /* Step 1: add edges whose y_min == y */
    for (auto& e : ET[y])
        AEL.push_back(e);
    ET[y].clear();

    /* Step 2: sort AEL by current x-intersection */
    AEL.sort([](const Edge& a, const Edge& b) {
        return a.x < b.x;
    });

    /* Step 3: remove edges where y_max == y (edge ends here) */
    AEL.remove_if([&](const Edge& e) {
        return e.y_max == y;
    });

    /* Step 4: advance x for the next scanline */
    for (auto& e : AEL)
        e.x += e.invSlope;
}

/* ─── Fill a single scanline ──────────────────────────────────────────── */

/**
 * fillScanline()
 *
 * Draw horizontal spans between consecutive pairs in the AEL:
 *   fill [AEL[0].x , AEL[1].x]  then  [AEL[2].x , AEL[3].x]  …
 *
 * For a convex polygon the AEL always has exactly 2 entries.
 * For a non-convex polygon it can have 4, 6, … entries — that is the
 * whole point of the AEL algorithm.
 */
void fillScanline(SDL_Renderer* ren,
                  const std::list<Edge>& AEL,
                  int y,
                  const Color& col)
{
    auto it = AEL.begin();
    while (it != AEL.end()) {
        int x_left = static_cast<int>(std::ceil(it->x));
        ++it;
        if (it == AEL.end()) break;          // safety — should be even
        int x_right = static_cast<int>(std::floor(it->x));
        ++it;

        /* Draw the filled span */
        SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.a);
        SDL_RenderDrawLine(ren, x_left, y, x_right, y);
    }
}

/* ─── Draw polygon outline ────────────────────────────────────────────── */
void drawOutline(SDL_Renderer* ren,
                 const std::vector<Point>& verts,
                 const Color& col)
{
    int n = static_cast<int>(verts.size());
    SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.a);
    for (int i = 0; i < n; i++) {
        const Point& a = verts[i];
        const Point& b = verts[(i + 1) % n];
        SDL_RenderDrawLine(ren, a.x, a.y, b.x, b.y);
    }
}

/* ─── Full scanline-AEL fill (static, no animation) ──────────────────── */
void scanlineAELFill(SDL_Renderer* ren,
                     const std::vector<Point>& verts,
                     const Color& fillCol)
{
    std::vector<std::list<Edge>> ET;
    int yMin, yMax;
    buildEdgeTable(verts, ET, yMin, yMax);

    std::list<Edge> AEL;

    for (int y = yMin; y <= yMax; y++) {
        /* Add edges that start at this scanline */
        for (auto& e : ET[y]) AEL.push_back(e);
        ET[y].clear();

        /* Sort by x */
        AEL.sort([](const Edge& a, const Edge& b){ return a.x < b.x; });

        /* Fill between pairs */
        fillScanline(ren, AEL, y, fillCol);

        /* Remove expired edges */
        AEL.remove_if([&](const Edge& e){ return e.y_max == y; });

        /* Advance x values */
        for (auto& e : AEL) e.x += e.invSlope;
    }
}

/* ─── Animated scanline-AEL fill ─────────────────────────────────────── */
/**
 * Renders the fill scanline-by-scanline so you can see the algorithm
 * "paint" the polygon from bottom to top.
 * Returns false if the user closed the window during animation.
 */
bool animatedAELFill(SDL_Renderer* ren,
                     SDL_Window*   /*win*/,
                     const std::vector<Point>& verts,
                     const Color&  fillCol,
                     const Color&  outlineCol)
{
    std::vector<std::list<Edge>> ET;
    int yMin, yMax;
    buildEdgeTable(verts, ET, yMin, yMax);

    std::list<Edge> AEL;
    SDL_Event ev;

    for (int y = yMin; y <= yMax; y++) {
        /* Handle quit events during animation */
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT)    return false;
            if (ev.type == SDL_KEYDOWN &&
                ev.key.keysym.sym == SDLK_ESCAPE) return false;
        }

        for (auto& e : ET[y]) AEL.push_back(e);
        ET[y].clear();

        AEL.sort([](const Edge& a, const Edge& b){ return a.x < b.x; });

        /* Draw current scanline indicator */
        SDL_SetRenderDrawColor(ren,
            COL_SCANLINE.r, COL_SCANLINE.g,
            COL_SCANLINE.b, COL_SCANLINE.a);
        SDL_RenderDrawLine(ren, 0, y, WIN_W, y);

        /* Fill spans */
        fillScanline(ren, AEL, y, fillCol);

        /* Re-draw outline on top so it stays visible */
        drawOutline(ren, verts, outlineCol);

        SDL_RenderPresent(ren);
        SDL_Delay(ANIM_DELAY_MS);

        /* Erase scanline indicator for next frame */
        SDL_SetRenderDrawColor(ren, COL_BG.r, COL_BG.g, COL_BG.b, 255);
        SDL_RenderDrawLine(ren, 0, y, WIN_W, y);

        AEL.remove_if([&](const Edge& e){ return e.y_max == y; });
        for (auto& e : AEL) e.x += e.invSlope;
    }

    /* Final outline pass */
    drawOutline(ren, verts, outlineCol);
    return true;
}

/* ─── Render a text label (SDL2 has no built-in font, use simple dot-matrix) */
void drawLabel(SDL_Renderer* ren, int x, int y, const std::string& text,
               const Color& col)
{
    /* Very minimal: just mark the position with a small cross */
    SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.a);
    SDL_RenderDrawLine(ren, x - 5, y, x + 5, y);
    SDL_RenderDrawLine(ren, x, y - 5, x, y + 5);
    (void)text; // actual text rendering needs SDL_ttf; omitted for portability
}

/* ─── Polygon definitions ─────────────────────────────────────────────── */

/**
 * A classic non-convex (concave) polygon — looks like an arrow or star-notch.
 * The re-entrant vertex at the top creates a situation where some scanlines
 * cross the boundary 4 times, which the AEL handles correctly.
 *
 *       A (top-left)          B (top-right)
 *        \                   /
 *         \                 /
 *          C (inner notch) D  ← concave dent
 *         /                 \
 *        /                   \
 *       E (bottom-left)      F (bottom-right)
 */
std::vector<Point> makeConcaveArrow(int cx, int cy, int scale)
{
    /* All coordinates relative to center (cx,cy) */
    return {
        {cx - 3*scale, cy - 4*scale},   // 0  A  top-left
        {cx + 3*scale, cy - 4*scale},   // 1  B  top-right
        {cx + 1*scale, cy           },  // 2  C  right inner
        {cx + 3*scale, cy + 4*scale},   // 3  D  bottom-right
        {cx           , cy + 2*scale},  // 4  E  bottom center
        {cx - 3*scale, cy + 4*scale},   // 5  F  bottom-left
        {cx - 1*scale, cy           },  // 6  G  left inner
    };
    /*
     * This 7-vertex shape produces scanlines that cross 4 edges in the
     * middle region — impossible to handle with the simple convex method.
     */
}

/**
 * An "M"-shaped non-convex polygon — two interior notches at the top.
 * Even more edges may be active simultaneously.
 */
std::vector<Point> makeMShape(int cx, int cy, int scale)
{
    return {
        {cx - 4*scale, cy + 3*scale},   // 0 bottom-left
        {cx - 4*scale, cy - 3*scale},   // 1 top-left
        {cx - 2*scale, cy           },  // 2 left inner notch
        {cx           , cy - 3*scale},  // 3 top-center
        {cx + 2*scale, cy           },  // 4 right inner notch
        {cx + 4*scale, cy - 3*scale},   // 5 top-right
        {cx + 4*scale, cy + 3*scale},   // 6 bottom-right
    };
}

/* ─── HUD overlay ─────────────────────────────────────────────────────── */
void drawHUD(SDL_Renderer* ren)
{
    /* Draw a simple legend box outline */
    SDL_SetRenderDrawColor(ren, 60, 60, 80, 255);
    SDL_Rect box = {10, 10, 320, 90};
    SDL_RenderDrawRect(ren, &box);

    /* Color swatches */
    SDL_SetRenderDrawColor(ren,
        COL_FILL.r, COL_FILL.g, COL_FILL.b, 255);
    SDL_Rect s1 = {20, 25, 18, 12}; SDL_RenderFillRect(ren, &s1);

    SDL_SetRenderDrawColor(ren,
        COL_FILL2.r, COL_FILL2.g, COL_FILL2.b, 255);
    SDL_Rect s2 = {20, 50, 18, 12}; SDL_RenderFillRect(ren, &s2);

    SDL_SetRenderDrawColor(ren,
        COL_SCANLINE.r, COL_SCANLINE.g, COL_SCANLINE.b, 255);
    SDL_Rect s3 = {20, 75, 18, 12}; SDL_RenderFillRect(ren, &s3);
    /* (Labels omitted — SDL_ttf not linked for portability) */
}

/* ─── Main ────────────────────────────────────────────────────────────── */
int main(int argc, char* argv[])
{
    /* Parse optional flag: --static  disables animation */
    bool animMode = true;
    for (int i = 1; i < argc; i++)
        if (std::string(argv[i]) == "--static") animMode = false;

    /* ── Init SDL2 ── */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow(
        "Scanline Fill — Active Edge List  |  CG Demo — Bahr",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!win) { SDL_Quit(); return 1; }

    SDL_Renderer* ren = SDL_CreateRenderer(
        win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) { SDL_DestroyWindow(win); SDL_Quit(); return 1; }

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    /* ── Define the two demo polygons ── */
    auto arrow = makeConcaveArrow(WIN_W / 4,     WIN_H / 2, 55);
    auto mshape = makeMShape     (3 * WIN_W / 4, WIN_H / 2, 55);

    /* ── Clear screen ── */
    SDL_SetRenderDrawColor(ren, COL_BG.r, COL_BG.g, COL_BG.b, 255);
    SDL_RenderClear(ren);

    /* ── Draw static version if requested, else animate ── */
    if (!animMode) {
        /* Polygon 1 — concave arrow */
        scanlineAELFill(ren, arrow,  COL_FILL);
        drawOutline    (ren, arrow,  COL_OUTLINE);

        /* Polygon 2 — M-shape */
        scanlineAELFill(ren, mshape, COL_FILL2);
        drawOutline    (ren, mshape, COL_OUTLINE2);

        drawHUD(ren);
        SDL_RenderPresent(ren);
    } else {
        /* Animated fill — polygon 1 */
        drawOutline(ren, arrow, COL_OUTLINE);
        SDL_RenderPresent(ren);
        SDL_Delay(400);

        if (!animatedAELFill(ren, win, arrow, COL_FILL, COL_OUTLINE))
            goto cleanup;

        /* Brief pause between polygons */
        SDL_Delay(800);

        /* Animated fill — polygon 2 */
        drawOutline(ren, mshape, COL_OUTLINE2);
        SDL_RenderPresent(ren);
        SDL_Delay(400);

        if (!animatedAELFill(ren, win, mshape, COL_FILL2, COL_OUTLINE2))
            goto cleanup;

        drawHUD(ren);
        SDL_RenderPresent(ren);
    }

    /* ── Event loop: stay open until user closes or presses ESC / Q ── */
    {
        bool running = true;
        SDL_Event ev;
        while (running) {
            SDL_WaitEvent(&ev);
            if (ev.type == SDL_QUIT) running = false;
            if (ev.type == SDL_KEYDOWN) {
                if (ev.key.keysym.sym == SDLK_ESCAPE ||
                    ev.key.keysym.sym == SDLK_q)
                    running = false;
                /* Press 'R' to re-run animation */
                if (ev.key.keysym.sym == SDLK_r && animMode) {
                    SDL_SetRenderDrawColor(ren, COL_BG.r, COL_BG.g,
                                          COL_BG.b, 255);
                    SDL_RenderClear(ren);
                    drawOutline(ren, arrow, COL_OUTLINE);
                    SDL_RenderPresent(ren);
                    SDL_Delay(400);
                    if (!animatedAELFill(ren, win, arrow,
                                        COL_FILL, COL_OUTLINE))
                        break;
                    SDL_Delay(600);
                    drawOutline(ren, mshape, COL_OUTLINE2);
                    SDL_RenderPresent(ren);
                    SDL_Delay(400);
                    if (!animatedAELFill(ren, win, mshape,
                                        COL_FILL2, COL_OUTLINE2))
                        break;
                    drawHUD(ren);
                    SDL_RenderPresent(ren);
                }
            }
        }
    }

cleanup:
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
