#include "registry.h"
#include "json.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static AppEntry g_apps[MAX_APPS];
static int g_count = 0;

int registry_load_from_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0 || sz > 512 * 1024) { fclose(f); return -1; }
    char *buf = malloc(sz + 1);
    fread(buf, 1, sz, f);
    buf[sz] = 0;
    fclose(f);

    g_count = 0;
    const char *p = buf;
    while (g_count < MAX_APPS) {
        const char *obj = strchr(p, '{');
        if (!obj) break;
        int depth = 0; const char *q = obj;
        for (; *q; q++) { if (*q == '{') depth++; else if (*q == '}') { depth--; if (!depth) break; } }
        if (!*q) break;
        char slice[2048]; int len = (int)(q - obj + 1);
        if (len >= (int)sizeof(slice)) len = sizeof(slice) - 1;
        memcpy(slice, obj, len); slice[len] = 0;

        AppEntry *e = &g_apps[g_count];
        memset(e, 0, sizeof(*e));
        json_get_string(slice, "id", e->id, sizeof(e->id));
        json_get_string(slice, "name", e->name, sizeof(e->name));
        json_get_string(slice, "version", e->version, sizeof(e->version));
        json_get_string(slice, "type", e->type, sizeof(e->type));
        json_get_string(slice, "repo", e->repo, sizeof(e->repo));
        json_get_string(slice, "description", e->desc, sizeof(e->desc));
        const char *fr = strstr(slice, "\"free\"");
        e->free_app = fr && strstr(fr, "true");
        if (e->id[0] || e->repo[0]) g_count++;
        p = q + 1;
    }
    free(buf);
    return 0;
}

int registry_count(void) { return g_count; }
AppEntry *registry_get(int idx) {
    if (idx < 0 || idx >= g_count) return 0;
    return &g_apps[idx];
}
