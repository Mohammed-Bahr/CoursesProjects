// ============================================================
//  Filling Task - Computer Graphics  (GTK3 + Cairo port)
//  Draw a 4-point polygon, then click inside to flood fill
//  Based on: Prof. Reda A. El-Khoribi - Filling Algorithms
//
//  Install deps (Fedora):
//    sudo dnf install gtk3-devel gcc-c++
//
//  Build:
//    g++ polygon_fill_gtk.cpp -o polygon_fill_gtk \
//        $(pkg-config --cflags --libs gtk+-3.0)
//
//  Run:
//    ./polygon_fill_gtk
// ============================================================

#include <gtk/gtk.h>
#include <cairo.h>
#include <stack>
#include <cstring>
#include <cstdio>
using namespace std;

// -------------------------------------------------------
//  Constants
// -------------------------------------------------------
#define WINDOW_W   820
#define WINDOW_H   640
#define MAX_POINTS 4
#define STATUS_H   30

// Colors (r, g, b) in 0-255
struct Color { int r, g, b; };

static const Color BORDER_COLOR = { 229,  57,  53 };  // red
static const Color FILL_COLOR   = {  66, 165, 245 };  // blue
static const Color BG_COLOR     = { 255, 255, 255 };  // white

// -------------------------------------------------------
//  Pixel buffer  (WINDOW_W x WINDOW_H, RGB)
//  This is our "back-buffer" — Cairo draws FROM this.
// -------------------------------------------------------
static guchar  g_pixels[WINDOW_H][WINDOW_W][3];  // [y][x][r,g,b]

static void buf_clear()
{
    for (int y = 0; y < WINDOW_H; y++)
        for (int x = 0; x < WINDOW_W; x++)
        {
            g_pixels[y][x][0] = BG_COLOR.r;
            g_pixels[y][x][1] = BG_COLOR.g;
            g_pixels[y][x][2] = BG_COLOR.b;
        }
}

static void buf_set(int x, int y, Color c)
{
    if (x < 0 || x >= WINDOW_W || y < 0 || y >= WINDOW_H) return;
    g_pixels[y][x][0] = c.r;
    g_pixels[y][x][1] = c.g;
    g_pixels[y][x][2] = c.b;
}

static Color buf_get(int x, int y)
{
    if (x < 0 || x >= WINDOW_W || y < 0 || y >= WINDOW_H)
        return BG_COLOR;
    return { g_pixels[y][x][0], g_pixels[y][x][1], g_pixels[y][x][2] };
}

static bool color_eq(Color a, Color b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

// -------------------------------------------------------
//  Globals
// -------------------------------------------------------
static int    g_points_x[MAX_POINTS];
static int    g_points_y[MAX_POINTS];
static int    g_count  = 0;
static bool   g_closed = false;
static bool   g_filled = false;

static GtkWidget* g_canvas  = nullptr;
static GtkWidget* g_status  = nullptr;

// -------------------------------------------------------
//  Bresenham line into pixel buffer
// -------------------------------------------------------
static void draw_line_buf(int x0, int y0, int x1, int y1, Color col)
{
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        buf_set(x0, y0, col);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

// -------------------------------------------------------
//  Draw polygon into pixel buffer
// -------------------------------------------------------
static void draw_polygon_buf()
{
    if (g_count == 0) return;

    for (int i = 1; i < g_count; i++)
        draw_line_buf(g_points_x[i-1], g_points_y[i-1],
                      g_points_x[i],   g_points_y[i],   BORDER_COLOR);

    if (g_closed)
        draw_line_buf(g_points_x[g_count-1], g_points_y[g_count-1],
                      g_points_x[0],          g_points_y[0],          BORDER_COLOR);

    // vertex dots (filled circle r=3)
    for (int i = 0; i < g_count; i++)
    {
        int px = g_points_x[i], py = g_points_y[i];
        for (int dy = -3; dy <= 3; dy++)
            for (int dx = -3; dx <= 3; dx++)
                if (dx*dx + dy*dy <= 9)
                    buf_set(px+dx, py+dy, BORDER_COLOR);
    }
}

// -------------------------------------------------------
//  Non-Recursive Flood Fill (boundary fill, 4-connected)
// -------------------------------------------------------
struct Pt { int x, y; };

static void flood_fill(int startX, int startY, Color Cb, Color Cf)
{
    stack<Pt> S;
    S.push({ startX, startY });

    while (!S.empty())
    {
        Pt v = S.top(); S.pop();

        if (v.x < 0 || v.x >= WINDOW_W ||
            v.y < 0 || v.y >= WINDOW_H) continue;

        Color c = buf_get(v.x, v.y);
        if (color_eq(c, Cb) || color_eq(c, Cf)) continue;

        buf_set(v.x, v.y, Cf);

        S.push({ v.x + 1, v.y });
        S.push({ v.x - 1, v.y });
        S.push({ v.x,     v.y + 1 });
        S.push({ v.x,     v.y - 1 });
    }
}

// -------------------------------------------------------
//  GTK draw callback — blit pixel buffer via Cairo
// -------------------------------------------------------
static gboolean on_draw(GtkWidget* widget, cairo_t* cr, gpointer)
{
    // Wrap our pixel buffer in a Cairo surface (stride = W*3, RGB24)
    // Cairo's RGB24 is actually stored as BGRX in native-endian 32-bit words,
    // so we build a temporary BGRX buffer for Cairo.
    static guchar cairo_buf[WINDOW_H * WINDOW_W * 4];

    for (int y = 0; y < WINDOW_H; y++)
        for (int x = 0; x < WINDOW_W; x++)
        {
            int idx = (y * WINDOW_W + x) * 4;
            cairo_buf[idx + 0] = g_pixels[y][x][2]; // B
            cairo_buf[idx + 1] = g_pixels[y][x][1]; // G
            cairo_buf[idx + 2] = g_pixels[y][x][0]; // R
            cairo_buf[idx + 3] = 0xFF;               // X (unused)
        }

    cairo_surface_t* surf = cairo_image_surface_create_for_data(
        cairo_buf,
        CAIRO_FORMAT_RGB24,
        WINDOW_W, WINDOW_H,
        WINDOW_W * 4   // stride
    );

    cairo_set_source_surface(cr, surf, 0, 0);
    cairo_paint(cr);
    cairo_surface_destroy(surf);
    return FALSE;
}

// -------------------------------------------------------
//  Mouse click handler
// -------------------------------------------------------
static gboolean on_button_press(GtkWidget* widget, GdkEventButton* event, gpointer)
{
    int mx = (int)event->x;
    int my = (int)event->y;

    if (event->button == GDK_BUTTON_PRIMARY)   // left click
    {
        if (!g_closed)
        {
            g_points_x[g_count] = mx;
            g_points_y[g_count] = my;
            g_count++;

            buf_clear();
            draw_polygon_buf();

            if (g_count == MAX_POINTS)
            {
                g_closed = true;
                buf_clear();
                draw_polygon_buf();
                gtk_label_set_text(GTK_LABEL(g_status),
                    "Polygon ready!  Click INSIDE to flood fill");
            }
            else
            {
                char buf[64];
                snprintf(buf, sizeof(buf),
                    "Click to place point %d of %d",
                    g_count + 1, MAX_POINTS);
                gtk_label_set_text(GTK_LABEL(g_status), buf);
            }

            gtk_widget_queue_draw(g_canvas);
        }
        else if (!g_filled)
        {
            gtk_label_set_text(GTK_LABEL(g_status), "Filling...");
            // Force label repaint before the (slow) fill
            while (gtk_events_pending()) gtk_main_iteration();

            flood_fill(mx, my, BORDER_COLOR, FILL_COLOR);
            g_filled = true;

            // Redraw border on top so it stays crisp
            draw_polygon_buf();

            gtk_label_set_text(GTK_LABEL(g_status),
                "Done!  Right-click to reset");
            gtk_widget_queue_draw(g_canvas);
        }
    }
    else if (event->button == GDK_BUTTON_SECONDARY)  // right click = reset
    {
        g_count  = 0;
        g_closed = false;
        g_filled = false;
        buf_clear();
        gtk_label_set_text(GTK_LABEL(g_status),
            "Click to place point 1 of 4");
        gtk_widget_queue_draw(g_canvas);
    }

    return TRUE;
}

// -------------------------------------------------------
//  Main
// -------------------------------------------------------
int main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    // ---- Window ----
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),
        "Filling Task — Draw 4-Point Polygon & Click to Fill");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_W,
                                WINDOW_H + STATUS_H + 10);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // ---- Vertical box: status label + drawing area ----
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Status label
    g_status = gtk_label_new("Click to place point 1 of 4");
    gtk_widget_set_size_request(g_status, WINDOW_W, STATUS_H);
    // Style it
    GtkCssProvider* css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css,
        "label { background: #f0f0f0; color: #323232; "
        "font-size: 13px; padding: 4px; }",
        -1, NULL);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(g_status),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_box_pack_start(GTK_BOX(vbox), g_status, FALSE, FALSE, 0);

    // Drawing area
    g_canvas = gtk_drawing_area_new();
    gtk_widget_set_size_request(g_canvas, WINDOW_W, WINDOW_H);
    gtk_box_pack_start(GTK_BOX(vbox), g_canvas, TRUE, TRUE, 0);

    // Enable mouse events on canvas
    gtk_widget_add_events(g_canvas,
        GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

    g_signal_connect(g_canvas, "draw",
                     G_CALLBACK(on_draw), NULL);
    g_signal_connect(g_canvas, "button-press-event",
                     G_CALLBACK(on_button_press), NULL);

    // ---- Init pixel buffer ----
    buf_clear();

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
