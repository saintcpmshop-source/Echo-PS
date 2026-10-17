#include "json.h"
#include <string.h>
#include <stdio.h>

int json_get_string(const char *json, const char *key, char *out, int outsz) {
    char pat[64];
    snprintf(pat, sizeof(pat), "\"%s\"", key);
    const char *p = strstr(json, pat);
    if (!p) return 0;
    p = strchr(p + strlen(pat), ':');
    if (!p) return 0;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '"') return 0;
    p++;
    int i = 0;
    while (*p && *p != '"' && i < outsz - 1) out[i++] = *p++;
    out[i] = 0;
    return 1;
}
