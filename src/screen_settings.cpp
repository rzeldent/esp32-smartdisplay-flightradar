#include <screen_settings.h>

enum state
{
  none,
  searching,
  connecting,
  connected,
  connect_failed
};

void screen_settings::button_event_handler(lv_event_t *e)
{
}

void screen_settings::button_refresh_handler(lv_event_t *e)
{
  log_i("Refresh clicked");

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);
  auto that = (screen_settings *)lv_event_get_user_data(e);
  log_w("screen_settings that=0x%08x", that);
  that->refresh();
}

void screen_settings::list_event_handler(lv_event_t *e)
{
  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  auto code = lv_event_get_code(e);
  auto current_target = lv_event_get_current_target(e);
  auto target = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED)
  {
    auto ssid = lv_list_get_btn_text(current_target, target);
    log_i("Clicked: %s", ssid);

    auto info = static_cast<const access_point_t *>(lv_event_get_param(e));
    log_i("SSID: %s", info->_ssid);
  }
}

screen_settings::screen_settings()
{
  log_i("screen_settings::screen_settings");

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  static const lv_coord_t col_dsc[] = {120, 120, LV_GRID_TEMPLATE_LAST};    // 240
  static const lv_coord_t row_dsc[] = {20, 260, 40, LV_GRID_TEMPLATE_LAST}; // 320
  lv_obj_set_grid_dsc_array(_screen, col_dsc, row_dsc);

  auto title = lv_label_create(_screen);
  lv_obj_set_grid_cell(title, LV_GRID_ALIGN_CENTER, 0, 2, LV_GRID_ALIGN_START, 0, 1);
  lv_label_set_text(title, "Available access points");

  _wifi_list = lv_list_create(_screen);
  lv_obj_set_grid_cell(_wifi_list, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 1, 1);

  auto refresh_button = lv_btn_create(_screen);
  lv_obj_set_grid_cell(refresh_button, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
  lv_obj_add_event_cb(refresh_button, button_refresh_handler, LV_EVENT_CLICKED, this);
  log_w("screen_settings this=0x%08x", this);
  auto refesh_label = lv_label_create(refresh_button);
  lv_label_set_text(refesh_label, "Refresh");

  auto cancel_button = lv_btn_create(_screen);
  lv_obj_set_grid_cell(cancel_button, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
  lv_obj_add_event_cb(cancel_button, button_event_handler, LV_EVENT_CLICKED, nullptr);
  auto cancel_label = lv_label_create(cancel_button);
  lv_label_set_text(cancel_label, "Cancel");

  //  refresh();
}

screen_settings::~screen_settings()
{
}

void screen_settings::refresh()
{
  _access_points.scan();

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  for (auto it = _access_points.begin(); it != _access_points.end(); ++it)
  {
    auto btn = lv_list_add_btn(_wifi_list, LV_SYMBOL_WIFI, it->_ssid.c_str());
    lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, &(*it));
  }
}