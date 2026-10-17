#include "store.h"
#include "app.h"
#include <stdio.h>

int store_request_install(const char *app_id) {
    if (!g_app.usb_mounted) return -1;
    char path[256];
    snprintf(path, sizeof(path), ECHO_REQ_DIR "/install-%s.json", app_id);
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    fprintf(f, "{\"app\":\"%s\",\"requested\":\"manual\"}\n", app_id);
    fclose(f);
    return 0;
}
