#pragma once

#include <access_points_info.h>
#include <screen.h>

class screen_settings : public screen
{
public:
  screen_settings();
  ~screen_settings();

private:
  lv_obj_t *_wifi_list;
  lv_obj_t *_wifi_password;
  lv_obj_t *_connect_button;

  access_points_info _access_points;
  static void list_event_handler(lv_event_t *e);
  static void button_cancel_handler(lv_event_t *e);
  static void button_scan_handler(lv_event_t *e);

  // Connect screen
  static void button_connect_connect_handler(lv_event_t *e);
  static void button_connect_cancel_handler(lv_event_t *e);
  static void password_focus_handler(lv_event_t *e);

  void refresh();
  void connect(const String &ssid);
  void done(const String &ssid, const String &password);
};