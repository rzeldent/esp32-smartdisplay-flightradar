#include <screen.h>

screen::screen()
{
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_size(screen_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_scr_load(screen_);
}

screen::~screen()
{
    lv_obj_del_async(screen_);
}