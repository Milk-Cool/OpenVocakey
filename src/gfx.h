#pragma once
#include <Arduino.h>

void gfx_init();
void gfx_set_jp(bool jp);
void gfx_set_inv(bool inv);
void gfx_print(int x, int y, String text);
void gfx_start_render();
void gfx_end_render();