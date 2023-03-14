#include <WiFi.h>
#include <esp_wifi.h>

#include <arduino_event_names.h>

#include <screen_connecting.h>

void screen_connecting::OnNext(arduino_event_t* event)
{
  log_i("arduino_event_id_t: %d - %s", event->event_id, arduino_event_names[event->event_id]);

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  lv_label_set_text(_info, arduino_event_names[event->event_id]);

  switch (event->event_id)
  {
  case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    lv_label_set_text(_ip, WiFi.localIP().toString().c_str());
    break;
  case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
    lv_label_set_text(_ip, WiFi.localIPv6().toString().c_str());
    break;
  case ARDUINO_EVENT_WIFI_STA_LOST_IP:
    lv_label_set_text(_ip, "");
    break;
  }
}

void screen_connecting::OnComplete()
{
}

screen_connecting::screen_connecting(ObservableProperty<arduino_event_t*> *observable_wifi_status)
{
  log_i("screen_connecting::screen_connecting");

  _observable_arduino_status = observable_wifi_status;

  const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);

  static const lv_coord_t col_dsc[] = {240, LV_GRID_TEMPLATE_LAST};                 // 240
  static const lv_coord_t row_dsc[] = {20, 20, 240, 20, 20, LV_GRID_TEMPLATE_LAST}; // 320
  lv_obj_set_grid_dsc_array(_screen, col_dsc, row_dsc);

  wifi_config_t config;
  auto ssid = esp_wifi_get_config((wifi_interface_t)ESP_IF_WIFI_STA, &config) == ESP_OK
                  ? String(reinterpret_cast<char const *>(config.sta.ssid), sizeof(config.sta.ssid))
                  : String();

  auto title = lv_label_create(_screen);
  lv_obj_set_grid_cell(title, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 0, 1);
  lv_label_set_text(title, "Connecting to access point");

  auto title_ssid = lv_label_create(_screen);
  lv_obj_set_grid_cell(title_ssid, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 1, 1);
  lv_label_set_text(title_ssid, ssid.c_str());

  auto spinner = lv_spinner_create(_screen, 750, 30);
  lv_obj_set_grid_cell(spinner, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
  lv_obj_center(spinner);

  _info = lv_label_create(_screen);
  lv_obj_set_grid_cell(_info, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 3, 1);
  lv_label_set_text(_info, "Initializing");

  _ip = lv_label_create(_screen);
  lv_obj_set_grid_cell(_ip, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 4, 1);
  lv_label_set_text(_ip, "");

  _observable_arduino_status->Subscribe(*this);
}

screen_connecting::~screen_connecting()
{
  log_i("screen_connecting::~screen_connecting");
  _observable_arduino_status->UnSubscribe(*this);
}