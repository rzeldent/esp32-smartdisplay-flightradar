#include <WiFi.h>
#include <esp_wifi.h>
#include <screen_connecting.h>

void screen_connecting::OnNext(wl_status_t status)
{
  log_i("wl_status: %d", status);

  {
    const std::lock_guard<std::mutex> lock(_mutex);

    switch (status)
    {
    case ARDUINO_EVENT_WIFI_READY:
      lv_label_set_text(_info, "WiFi ready");
      break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
      lv_label_set_text(_info, "Scan done");
      break;
    case ARDUINO_EVENT_WIFI_STA_START:
      lv_label_set_text(_info, "Start");
      break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
      lv_label_set_text(_info, "Stop");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      lv_label_set_text(_info, "Connected");
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      lv_label_set_text(_info, "Disconnected");
      break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
      lv_label_set_text(_info, "Authentication mode change");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      lv_label_set_text(_info, "Got IP address");
      lv_label_set_text(_ip, WiFi.localIP().toString().c_str());
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
      lv_label_set_text(_info, "Got IP address");
      lv_label_set_text(_ip, WiFi.localIPv6().toString().c_str());
      break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
      lv_label_set_text(_info, "Lost IP address");
      lv_label_set_text(_ip, "");
      break;
    }
  }
}

void screen_connecting::OnComplete()
{
}

screen_connecting::screen_connecting(ObservableProperty<wl_status_t> *observable_wifi_status)
{
  log_i("screen_connecting::screen_connecting");
  observable_wifi_status_ = observable_wifi_status;

  {
    const std::lock_guard<std::mutex> lock(_mutex);

    static const lv_coord_t col_dsc[] = {240, LV_GRID_TEMPLATE_LAST};                 // 240
    static const lv_coord_t row_dsc[] = {20, 20, 240, 20, 20, LV_GRID_TEMPLATE_LAST}; // 320
    lv_obj_set_grid_dsc_array(_screen, col_dsc, row_dsc);

    wifi_config_t config;
    auto ssid = esp_wifi_get_config((wifi_interface_t)ESP_IF_WIFI_STA, &config) == ESP_OK ? std::string(reinterpret_cast<char const *>(config.sta.ssid), sizeof(config.sta.ssid)) : std::string("");

    auto title = lv_label_create(_screen);
    lv_obj_set_grid_cell(title, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_label_set_text(title, "Connecting to access point");

    auto title_ssid = lv_label_create(_screen);
    lv_obj_set_grid_cell(title_ssid, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_label_set_text(title_ssid, ssid.c_str());

    auto spinner = lv_spinner_create(_screen, 500, 30);
    lv_obj_set_grid_cell(spinner, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_center(spinner);

    _info = lv_label_create(_screen);
    lv_obj_set_grid_cell(_info, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 3, 1);
    lv_label_set_text(_info, "Initializing");

    _ip = lv_label_create(_screen);
    lv_obj_set_grid_cell(_ip, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 4, 1);
    lv_label_set_text(_ip, "");
  }
  
  observable_wifi_status_->Subscribe(*this);
}

screen_connecting::~screen_connecting()
{
  log_i("screen_connecting::~screen_connecting");
  observable_wifi_status_->UnSubscribe(*this);
}