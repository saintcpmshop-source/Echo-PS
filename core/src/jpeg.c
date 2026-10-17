#include "jpeg.h"
#include <stdlib.h>
#include <string.h>

extern unsigned char echopslogo_jpg[];
extern unsigned int  echopslogo_jpg_len;

#ifdef ECHO_HAS_JPEGDEC
#include <cell/codec/jpeg.h>

static Image *decode_with_jpgdec(const uint8_t *data, int size) {
    CellJpgDecMainHandle   main;
    CellJpgDecThreadHandle thread;
    CellJpgDecOutput       out;
    CellJpgDecSrc          src = {0};

    src.srcType = CELL_JPGDEC_BUF;
    src.streamPtr = (void *)data;
    src.streamSize = size;

    if (cellJpgDecCreate(&main, NULL) != 0) return NULL;
    if (cellJpgDecCreateThread(&thread) != 0) return NULL;

    CellJpgDecInfo info;
    if (cellJpgDecReadHeader(&main, &info, &src) != 0) return NULL;

    int w = info.imageWidth, h = info.imageHeight;
    int stride = (w * 4 + 15) & ~15;
    uint8_t *buf = memalign(128, stride * h);
    if (!buf) return NULL;

    out.outputPtr = buf;
    out.outputSize = stride * h;

    CellJpgDecInParam in = {0};
    in.commandPtr = NULL;
    in.downScale = CELL_JPGDEC_1_1;
    in.method = CELL_JPGDEC_METHOD_2X2;
    in.outputMode = CELL_JPGDEC_OUTPUT_RGBA;

    CellJpgDecOutParam outParam;
    cellJpgDecDecode(&thread, &outParam, &out, &src, &in);
    uint32_t done = 0;
    while (!done) cellJpgDecWaitDecode(&thread, 0, &done);

    Image *img = malloc(sizeof(Image));
    img->w = w; img->h = h;
    img->px = malloc((size_t)w * h * 4);
    for (int y = 0; y < h; y++) {
        uint8_t *row = buf + y * stride;
        for (int x = 0; x < w; x++) {
            uint8_t r = row[x*4+0], g = row[x*4+1], b = row[x*4+2], a = row[x*4+3];
            img->px[y*w + x] = ((uint32_t)a << 24) | ((uint32_t)r << 16) |
                               ((uint32_t)g << 8)  |  (uint32_t)b;
        }
    }
    free(buf);
    cellJpgDecDestroyThread(&thread);
    cellJpgDecDestroy(&main);
    return img;
}
#endif

Image *jpeg_decode_rgba(const uint8_t *data, int size) {
#ifdef ECHO_HAS_JPEGDEC
    if (data && size > 0) {
        Image *img = decode_with_jpgdec(data, size);
        if (img) return img;
    }
#endif
    Image *img = malloc(sizeof(Image));
    img->w = SCREEN_W; img->h = SCREEN_H;
    img->px = malloc((size_t)img->w * img->h * 4);
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < img->w; x++) {
            uint8_t r = (uint8_t)(x * 255 / img->w);
            uint8_t b = (uint8_t)(y * 255 / img->h);
            img->px[y*img->w+x] = (0xFFu<<24)|((uint32_t)r<<16)|(0x40u<<8)|b;
        }
    }
    return img;
}
