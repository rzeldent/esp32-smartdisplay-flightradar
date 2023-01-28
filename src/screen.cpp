#include <screen.h>
#include <esp32-hal-log.h>

lv_obj_t* screen::_screen = nullptr;

screen::screen()
{
  log_i("screen::screen");
  auto screen = lv_obj_create(nullptr);
  lv_obj_set_size(screen, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  // Remove border etc... to prevent having margins
  lv_obj_remove_style_all(screen);
  lv_scr_load(screen);
  if (_screen != nullptr)
    lv_obj_del_async(_screen);

    _screen = screen;
}

const uint16_t screen::screen_height() 
{
  return lv_area_get_height(&_screen->coords);
}

const uint16_t screen::screen_width()
{
  return lv_area_get_width(&_screen->coords);
}
