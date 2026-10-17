#ifndef ECHO_UI_H
#define ECHO_UI_H

#include "app.h"
#include "gfx.h"

void ui_init(Image *logo);
void ui_shutdown(void);
void ui_draw(void);

void ui_input_up(void);
void ui_input_down(void);
void ui_input_left(void);
void ui_input_right(void);
void ui_input_confirm(void);
void ui_input_back(void);

#endif
