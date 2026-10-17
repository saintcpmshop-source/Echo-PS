#include "ui.h"
#include "registry.h"
#include "store.h"
#include <string.h>

static Image *g_logo = NULL;
static int g_store_sel = 0;

static const char *SECTION_NAMES[SECTION_COUNT] = { "HOME", "STORE", "SYSTEM" };

void ui_init(Image *logo) { g_logo = logo; }
void ui_shutdown(void) { }

static void draw_splash(void) {
    gfx_clear(COL_BG);
    if (g_logo) {
        int w = g_logo->w, h = g_logo->h;
        if (w > 720) { h = h * 720 / w; w = 720; }
        if (h > 480) { w = w * 480 / h; h = 480; }
        int x = (SCREEN_W - w) / 2;
        int y = (SCREEN_H - h) / 2 - 40;

        float t = g_app.splash_t;
        float alpha = t < 0.4f ? (t / 0.4f) : 1.0f;
        if (alpha > 1.0f) alpha = 1.0f;

        for (int j = 0; j < h; j++) {
            int sy = (int)((long)j * g_logo->h / h);
            for (int i = 0; i < w; i++) {
                int sx = (int)((long)i * g_logo->w / w);
                uint32_t p = g_logo->px[sy * g_logo->w + sx];
                uint8_t a = (uint8_t)(((p >> 24) & 0xFF) * alpha);
                if (a == 0) continue;
                uint8_t fr = (p >> 16) & 0xFF, fg = (p >> 8) & 0xFF, fb = p & 0xFF;
                uint8_t rr = (uint8_t)((fr * a + 0x0E * (255 - a)) / 255);
                uint8_t gg = (uint8_t)((fg * a + 0x10 * (255 - a)) / 255);
                uint8_t bl = (uint8_t)((fb * a + 0x16 * (255 - a)) / 255);
                gfx_rect(x + i, y + j, 1, 1, (Color){ rr, gg, bl, 255 });
            }
        }
    }
    int tw = gfx_text_width("ECHO PS", 6);
    gfx_text((SCREEN_W - tw) / 2, SCREEN_H / 2 + 180, "ECHO PS", COL_TEXT, 6);

    int bar_w = 500, bar_h = 6;
    int bx = (SCREEN_W - bar_w) / 2;
    int by = SCREEN_H - 140;
    gfx_rect(bx, by, bar_w, bar_h, COL_CARD_HI);
    gfx_rect(bx, by, (int)(bar_w * g_app.splash_t), bar_h, COL_ACCENT);
    gfx_text(bx, by + 24, "V" ECHO_VERSION, COL_TEXT_DIM, 2);
}

static void draw_sidebar(void) {
    gfx_rect(0, 0, 220, SCREEN_H, COL_CARD);
    gfx_text(28, 32, "ECHO PS", COL_ACCENT, 4);
    gfx_text(28, 70, "V" ECHO_VERSION, COL_TEXT_DIM, 2);
    for (int i = 0; i < SECTION_COUNT; i++) {
        int y = 140 + i * 64;
        int active = (i == (int)g_app.current);
        if (active) gfx_rect(20, y - 12, 180, 48, COL_CARD_HI);
        gfx_text(36, y, SECTION_NAMES[i], active ? COL_ACCENT : COL_TEXT, 3);
    }
}

static void draw_home(void) {
    int x = 260;
    gfx_text(x, 60, "HOME", COL_TEXT, 5);
    gfx_text(x, 110, "WELCOME TO ECHO PS", COL_TEXT_DIM, 3);
    gfx_rect(x, 160, SCREEN_W - x - 60, 2, COL_CARD_HI);
    gfx_text(x, 190, "A POLISHED HUB FOR YOUR PS3.", COL_TEXT, 3);
    gfx_text(x, 226, "USE STORE TO ADD APPS FROM GITHUB.", COL_TEXT_DIM, 3);
    gfx_text(x, 262, "BRIDGE ON PC HANDLES DOWNLOADS.", COL_TEXT_DIM, 3);
    gfx_text(x, 340, g_app.usb_mounted ? "USB: MOUNTED" : "USB: NOT FOUND",
             g_app.usb_mounted ? COL_ACCENT : COL_TEXT_DIM, 3);
    gfx_text(x, 380, "DIR: /EchoPS", COL_TEXT_DIM, 3);
}

static void draw_store(void) {
    int x = 260;
    gfx_text(x, 60, "STORE", COL_TEXT, 5);
    int n = registry_count();
    if (n == 0) {
        gfx_text(x, 120, "NO STORE INDEX FOUND.", COL_TEXT_DIM, 3);
        gfx_text(x, 150, "PLACE INDEX.JSON IN /ECHOPS.", COL_TEXT_DIM, 3);
        return;
    }
    int start = g_store_sel > 5 ? g_store_sel - 5 : 0;
    for (int i = start; i < n && i < start + 6; i++) {
        AppEntry *e = registry_get(i);
        int y = 120 + (i - start) * 88;
        int active = (i == g_store_sel);
        gfx_rect(x, y, SCREEN_W - x - 60, 76, active ? COL_CARD_HI : COL_CARD);
        if (active) gfx_rect(x, y, 4, 76, COL_ACCENT);
        gfx_text(x + 24, y + 16, e->name[0] ? e->name : e->id, COL_TEXT, 3);
        gfx_text(x + 24, y + 46, e->repo[0] ? e->repo : e->desc, COL_TEXT_DIM, 2);
        gfx_text(SCREEN_W - 200, y + 16, e->free_app ? "FREE" : "PAID",
                 e->free_app ? COL_ACCENT : COL_TEXT_DIM, 3);
        gfx_text(SCREEN_W - 200, y + 46, e->version, COL_TEXT_DIM, 2);
    }
    gfx_text(x, SCREEN_H - 60, "X: INSTALL   UP/DOWN: SELECT   O: BACK", COL_TEXT_DIM, 2);
}

static void draw_system(void) {
    int x = 260;
    gfx_text(x, 60, "SYSTEM", COL_TEXT, 5);
    gfx_text(x, 120, "ECHO PS VERSION: " ECHO_VERSION, COL_TEXT, 3);
    gfx_text(x, 160, g_app.usb_mounted ? "USB: MOUNTED" : "USB: NOT FOUND", COL_TEXT, 3);
    gfx_text(x, 200, "REQUESTS DIR: /EchoPS/REQUESTS", COL_TEXT_DIM, 2);
    gfx_text(x, 230, "PACKAGES DIR: /EchoPS/PKGS", COL_TEXT_DIM, 2);
    gfx_text(x, 260, "REGISTRY: /EchoPS/APPS/REGISTRY.JSON", COL_TEXT_DIM, 2);
}

void ui_draw(void) {
    if (g_app.stage == BOOT_SPLASH) { draw_splash(); return; }
    gfx_clear(COL_BG);
    draw_sidebar();
    switch (g_app.current) {
        case SECTION_HOME:   draw_home(); break;
        case SECTION_STORE:  draw_store(); break;
        case SECTION_SYSTEM: draw_system(); break;
        default: break;
    }
}

void ui_input_up(void) {
    if (g_app.current == SECTION_STORE && g_store_sel > 0) g_store_sel--;
}
void ui_input_down(void) {
    if (g_app.current == SECTION_STORE && g_store_sel < registry_count() - 1) g_store_sel++;
}
void ui_input_left(void) {
    if (g_app.current > 0) g_app.current = (Section)(g_app.current - 1);
}
void ui_input_right(void) {
    if (g_app.current < SECTION_COUNT - 1) g_app.current = (Section)(g_app.current + 1);
}
void ui_input_confirm(void) {
    if (g_app.current == SECTION_STORE && registry_count() > 0) {
        AppEntry *e = registry_get(g_store_sel);
        if (e) store_request_install(e->id[0] ? e->id : e->repo);
    }
}
void ui_input_back(void) { g_app.running = 0; }
