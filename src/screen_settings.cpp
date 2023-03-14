#include <screen_settings.h>

enum state
{
  none,
  searching,
  connecting,
  connected,
  connect_failed
};

void screen_settings::button_scan_handler(lv_event_t *e)
{
  log_i("Scan handler");

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  auto that = (screen_settings *)lv_event_get_user_data(e);
  auto code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED)
    that->refresh();
}

void screen_settings::button_cancel_handler(lv_event_t *e)
{
  log_i("Cancel handler");
  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  auto that = (screen_settings *)lv_event_get_user_data(e);
  auto code = lv_event_get_code(e);

  WiFi.begin();
}

void screen_settings::list_event_handler(lv_event_t *e)
{
  log_i("List handler");

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  auto that = (screen_settings *)lv_event_get_user_data(e);
  auto code = lv_event_get_code(e);
  auto current_target = lv_event_get_current_target(e);
  auto target = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED)
  {
    auto ssid = lv_list_get_btn_text(current_target, target);
    that->connect(that->_access_points.find_by_ssid(ssid));
  }
}

void screen_settings::button_connect_connect_handler(lv_event_t *e)
{
  log_i("Connect connect handler");

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  auto that = (screen_settings *)lv_event_get_user_data(e);
  auto code = lv_event_get_code(e);

  String password = lv_textarea_get_text(that->_wifi_password);
  password="hrxwcsWwn7yz";
/*
    if (_password.empty())
  {
        // Password required. Prompt for password
        static const char *btns[] = {"OK", "Cancel", ""};
        auto message = String("Please enter the password for ") + _ssid + ":";
        auto password_box = lv_msgbox_create(that->_screen, "Password required", message.c_str(), btns, false);
        lv_obj_align(password_box, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_event_cb(password_box, message_box_handler, LV_EVENT_CLICKED, that);
        }

*/  
  WiFi.begin(that->_ap_info->_ssid.c_str(), password.c_str());
}

void screen_settings::button_connect_cancel_handler(lv_event_t *e)
{
  log_i("Connect cancel handler");

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  auto that = (screen_settings *)lv_event_get_user_data(e);
  auto code = lv_event_get_code(e);

  WiFi.begin();
}

void screen_settings::password_focus_handler(lv_event_t *e)
{
  log_i("Password focus handler");

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  auto code = lv_event_get_code(e);
  auto ta = lv_event_get_target(e);
  auto kb = (lv_obj_t *)lv_event_get_user_data(e);
  switch (code)
  {
  case LV_EVENT_FOCUSED:
    lv_keyboard_set_textarea(kb, ta);
    lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    break;

  case LV_EVENT_DEFOCUSED:
    lv_keyboard_set_textarea(kb, nullptr);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    break;
  }
}

void screen_settings::connect(std::vector<access_point_t>::iterator ap)
{ 
  if (ap == _access_points.end())
  {
    log_e("Invalid iterator for AP");
    return;
  }

  _ap_info = ap;
  log_i("Connect to SSID: %s", ap->_ssid.c_str());

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  // create new screen
  auto screen = lv_obj_create(_screen);
  // lv_obj_remove_style_all(_screen);
  // lv_obj_set_size(screen, screen_width(), screen_height());
  lv_obj_set_size(screen, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

  static const lv_coord_t col_dsc[] = {100, 100, LV_GRID_TEMPLATE_LAST};                // 240
  static const lv_coord_t row_dsc[] = {15, 20, 15, 20, 140, 40, LV_GRID_TEMPLATE_LAST}; // 320
  lv_obj_set_grid_dsc_array(screen, col_dsc, row_dsc);

  auto label_connect_to = lv_label_create(screen);
  lv_obj_set_grid_cell(label_connect_to, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);
  lv_label_set_text(label_connect_to, "Connect to SSID:");

  auto label_ssid = lv_label_create(screen);
  lv_obj_set_grid_cell(label_ssid, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 1, 2);
  lv_label_set_text(label_ssid, ap->_ssid.c_str());

  auto password_label = lv_label_create(screen);
  lv_obj_set_grid_cell(password_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 2, 1);
  lv_label_set_text(password_label, "Password:");

  _wifi_password = lv_textarea_create(screen);
  lv_obj_set_grid_cell(_wifi_password, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 3, 1);
  lv_obj_set_size(_wifi_password, 200, LV_SIZE_CONTENT);
  lv_textarea_set_one_line(_wifi_password, true);
  lv_textarea_set_max_length(_wifi_password, 64);
  lv_textarea_set_password_mode(_wifi_password, true);

  log_i("Found AP info: %s", ap->_ssid);
  if (ap->_encryption == wifi_auth_mode_t::WIFI_AUTH_OPEN)
    lv_obj_add_state(_wifi_password, LV_STATE_DISABLED);

  auto keyboard = lv_keyboard_create(screen);
  lv_obj_set_grid_cell(keyboard, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 4, 1);
  lv_keyboard_set_textarea(keyboard, _wifi_password);
  // Size not set by lv_obj_set_grid_cell ?!
  lv_obj_set_size(keyboard, 200, 140);

  lv_obj_add_event_cb(_wifi_password, password_focus_handler, LV_EVENT_FOCUSED, keyboard);
  lv_obj_add_event_cb(_wifi_password, password_focus_handler, LV_EVENT_DEFOCUSED, keyboard);

  auto connect_button = lv_btn_create(screen);
  lv_obj_set_grid_cell(connect_button, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 5, 1);
  lv_obj_add_event_cb(connect_button, button_connect_connect_handler, LV_EVENT_CLICKED, this);
  auto connect_label = lv_label_create(connect_button);
  lv_label_set_text(connect_label, "Connect");

  auto cancel_button = lv_btn_create(screen);
  lv_obj_set_grid_cell(cancel_button, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 5, 1);
  lv_obj_add_event_cb(cancel_button, button_connect_cancel_handler, LV_EVENT_CLICKED, this);
  auto cancel_label = lv_label_create(cancel_button);
  lv_label_set_text(cancel_label, "Cancel");

  /*
    {
      // Password required. Prompt for password
      static const char *btns[] = {"OK", "Cancel", ""};
      auto message = String("Please enter the password for ") + ssid + ":";
      auto password_box = lv_msgbox_create(that->_screen, "Password required", message.c_str(), btns, false);
      lv_obj_align(password_box, LV_ALIGN_CENTER, 0, 0);
      lv_obj_add_event_cb(password_box, message_box_handler, LV_EVENT_CLICKED, that);
    }
  */
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

  auto scan_button = lv_btn_create(_screen);
  lv_obj_set_grid_cell(scan_button, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
  lv_obj_add_event_cb(scan_button, button_scan_handler, LV_EVENT_CLICKED, this);
  auto scan_label = lv_label_create(scan_button);
  lv_label_set_text(scan_label, "Scan");

  auto cancel_button = lv_btn_create(_screen);
  lv_obj_set_grid_cell(cancel_button, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
  lv_obj_add_event_cb(cancel_button, button_cancel_handler, LV_EVENT_CLICKED, nullptr);
  auto cancel_label = lv_label_create(cancel_button);
  lv_label_set_text(cancel_label, "Cancel");

  // refresh();
}

screen_settings::~screen_settings()
{
}

void screen_settings::refresh()
{
  _access_points.scan();

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  lv_obj_t *child;
  while ((child = lv_obj_get_child(_wifi_list, 0)) != nullptr)
  {
    log_i("Removing child: %d", child);
    lv_obj_del(child);
  }

  for (auto it = _access_points.begin(); it != _access_points.end(); ++it)
  {
    auto btn = lv_list_add_btn(_wifi_list, LV_SYMBOL_WIFI, it->_ssid.c_str());
    lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, this);
    log_i("Added list button: %s %d", it->_ssid.c_str(), btn);
  }
}
