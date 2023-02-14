#include <screen.h>
#include <esp32-hal-log.h>

screen::screen()
{
  log_i("screen::screen");

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  _screen = lv_obj_create(nullptr);
  lv_obj_set_size(_screen, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  // Remove border etc... to prevent having margins
  lv_obj_remove_style_all(_screen);

  lv_obj_clean(lv_scr_act());
  lv_scr_load(_screen);
}

screen::~screen()
{
  log_i("~screen::screen");

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  lv_obj_del_async(_screen);
}

const uint16_t screen::screen_height()
{
  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  return lv_area_get_height(&_screen->coords);
}

const uint16_t screen::screen_width()
{
  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  return lv_area_get_width(&_screen->coords);
}
