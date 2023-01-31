#pragma once

#include <esp32_smartdisplay.h>

class screen
{
public:
  screen();
  virtual ~screen();

  const uint16_t screen_height();
  const uint16_t screen_width();

protected:
  lv_obj_t *_screen;
};