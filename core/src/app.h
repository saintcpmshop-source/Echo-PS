#ifndef ECHO_APP_H
#define ECHO_APP_H

#include <stdint.h>
#include <stdio.h>

#define ECHO_VERSION "0.1.0"
#define USB_ROOT     "/dev_usb000"
#define ECHO_DIR     USB_ROOT "/EchoPS"
#define ECHO_REQ_DIR ECHO_DIR "/requests"
#define ECHO_PKG_DIR ECHO_DIR "/pkgs"
#define ECHO_STORE   ECHO_DIR "/index.json"
#define ECHO_APPS    ECHO_DIR "/apps"
#define ECHO_REG     ECHO_APPS "/registry.json"

typedef enum {
    SECTION_HOME = 0,
    SECTION_STORE,
    SECTION_SYSTEM,
    SECTION_COUNT
} Section;

typedef enum {
    BOOT_SPLASH = 0,
    BOOT_MAIN
} BootStage;

typedef struct {
    Section   current;
    BootStage stage;
    int       running;
    int       usb_mounted;
    float     splash_t;
} AppState;

extern AppState g_app;

void app_init(void);
void app_shutdown(void);
void app_tick(void);

#endif
