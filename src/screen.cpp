#include <screen.h>

screen::screen() {
  screen_ = lv_obj_create(nullptr);
  lv_obj_set_size(screen_, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  // Remove border etc... to prevent having margins
  lv_obj_remove_style_all(screen_);
  lv_scr_load(screen_);
}

screen::~screen() {
  lv_obj_del_async(screen_);
}

const uint16_t screen::screen_height() const {
  return lv_area_get_height(&screen_->coords);
}

const uint16_t screen::screen_width() const {
  return lv_area_get_width(&screen_->coords);
}
