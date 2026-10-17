#ifndef ECHO_GFX_H
#define ECHO_GFX_H

#include <stdint.h>

#define SCREEN_W 1280
#define SCREEN_H 720

typedef struct { uint8_t r, g, b, a; } Color;

#define COL_BG       ((Color){ 14, 16, 22, 255 })
#define COL_CARD     ((Color){ 26, 30, 40, 255 })
#define COL_CARD_HI  ((Color){ 38, 44, 58, 255 })
#define COL_ACCENT   ((Color){ 90, 200, 250, 255 })
#define COL_TEXT     ((Color){ 240, 244, 250, 255 })
#define COL_TEXT_DIM ((Color){ 150, 158, 172, 255 })

typedef struct {
    int w, h;
    uint32_t *px;
} Image;

int  gfx_init(void);
void gfx_shutdown(void);
void gfx_begin(void);
void gfx_end(void);
void gfx_clear(Color c);
void gfx_rect(int x, int y, int w, int h, Color c);
void gfx_text(int x, int y, const char *s, Color c, int scale);
int  gfx_text_width(const char *s, int scale);

void gfx_draw_image(const Image *img, int x, int y, int w, int h);
void gfx_draw_image_centered(const Image *img);
void gfx_free_image(Image *img);

#endif
