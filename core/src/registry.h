#ifndef ECHO_REGISTRY_H
#define ECHO_REGISTRY_H

typedef struct {
    char id[64];
    char name[64];
    char version[16];
    char type[16];
    int  free_app;
    char repo[128];
    char desc[128];
} AppEntry;

#define MAX_APPS 64

int  registry_load_from_file(const char *path);
int  registry_count(void);
AppEntry *registry_get(int idx);

#endif
