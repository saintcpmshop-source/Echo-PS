#include "app.h"
#include <sys/stat.h>
#include <sys/timer.h>

AppState g_app;

static int dir_exists(const char *p) {
    struct stat st;
    return (stat(p, &st) == 0 && S_ISDIR(st.st_mode));
}

static void ensure_dirs(void) {
    mkdir(ECHO_DIR,     0777);
    mkdir(ECHO_REQ_DIR, 0777);
    mkdir(ECHO_PKG_DIR, 0777);
    mkdir(ECHO_APPS,    0777);
}

void app_init(void) {
    g_app.current     = SECTION_HOME;
    g_app.stage       = BOOT_SPLASH;
    g_app.running     = 1;
    g_app.splash_t    = 0.0f;
    g_app.usb_mounted = dir_exists(USB_ROOT);
    if (g_app.usb_mounted) ensure_dirs();
}

void app_shutdown(void) { g_app.running = 0; }

void app_tick(void) {
    if (g_app.stage == BOOT_SPLASH) {
        g_app.splash_t += 1.0f / 150.0f;
        if (g_app.splash_t >= 1.0f) {
            g_app.splash_t = 1.0f;
            g_app.stage = BOOT_MAIN;
        }
    }
    sys_timer_usleep(16000);
}
