#pragma once

#include <lvgl.h>

class screen {
 public:
  screen();
  ~screen();

  const uint16_t screen_height() const;
  const uint16_t screen_width() const;

 protected:
  lv_obj_t *screen_;
};