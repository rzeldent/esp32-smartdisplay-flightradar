#pragma once

#include <lvgl.h>

class screen
{
public:
    screen();
    ~screen();

protected:
    lv_obj_t *screen_;
};