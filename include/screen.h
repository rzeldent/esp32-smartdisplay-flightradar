#pragma once

#include <lvgl.h>
#include <mutex>

class screen
{
public:
  screen();
  virtual ~screen();

  const uint16_t screen_height();
  const uint16_t screen_width();

  static std::mutex _mutex;
protected:
  lv_obj_t *_screen;
};