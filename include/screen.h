#pragma once

#include <esp32_smartdisplay.h>

class screen
{
public:
  screen();
  virtual ~screen();

protected:
  lv_obj_t *_screen;
};