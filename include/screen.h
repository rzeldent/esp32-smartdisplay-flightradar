#pragma once

#include <lvgl.h>

class screen
{
public:
  screen();

  static const uint16_t screen_height();
  static const uint16_t screen_width();

protected:
  static lv_obj_t *_screen;
};