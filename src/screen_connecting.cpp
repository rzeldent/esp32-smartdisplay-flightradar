#include <WiFi.h>
#include <esp_wifi.h>
#include <screen_connecting.h>

lv_obj_t *screen_connecting::info_;
lv_obj_t *screen_connecting::ip_;

void screen_connecting::wifi_station_event_callback(WiFiEvent_t event, WiFiEventInfo_t info)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_READY:
        lv_label_set_text(info_, "WiFi Ready");
        break;
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
        lv_label_set_text(info_, "Scan Done");
        break;
    case ARDUINO_EVENT_WIFI_STA_START:
        lv_label_set_text(info_, "Start");
        break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
        lv_label_set_text(info_, "Stop");
        break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        lv_label_set_text(info_, "Connected");
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        lv_label_set_text(info_, "Disconnected");
        break;
    case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
        lv_label_set_text(info_, "Authentication mode change");
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        lv_label_set_text(info_, "Got IP Address");
        lv_label_set_text(ip_, WiFi.localIP().toString().c_str());
        break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        lv_label_set_text(info_, "Got IP Address");
        lv_label_set_text(ip_, WiFi.localIPv6().toString().c_str());
        break;
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        lv_label_set_text(info_, "Lost IP Address");
        lv_label_set_text(ip_, "");
        break;
    }
}

screen_connecting::screen_connecting()
{
    static const lv_coord_t col_dsc[] = {240, LV_GRID_TEMPLATE_LAST};                 // 240
    static const lv_coord_t row_dsc[] = {20, 20, 240, 20, 20, LV_GRID_TEMPLATE_LAST}; // 320
    lv_obj_set_grid_dsc_array(screen_, col_dsc, row_dsc);

    wifi_config_t config;
    auto ssid = esp_wifi_get_config((wifi_interface_t)ESP_IF_WIFI_STA, &config) == ESP_OK ? std::string(reinterpret_cast<char const *>(config.sta.ssid), sizeof(config.sta.ssid)) : std::string("");

    auto title = lv_label_create(screen_);
    lv_obj_set_grid_cell(title, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_label_set_text(title, "Connecting to access point");

    auto title_ssid = lv_label_create(screen_);
    lv_obj_set_grid_cell(title_ssid, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_label_set_text(title_ssid, ssid.c_str());

    auto spinner = lv_spinner_create(screen_, 1000, 30);
    lv_obj_set_grid_cell(spinner, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_center(spinner);

    info_ = lv_label_create(screen_);
    lv_obj_set_grid_cell(info_, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 3, 1);
    lv_label_set_text(info_, "Initializing");

    ip_ = lv_label_create(screen_);
    lv_obj_set_grid_cell(ip_, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 4, 1);
    lv_label_set_text(ip_, "");

    wifi_event_id = WiFi.onEvent(wifi_station_event_callback);
}

screen_connecting::~screen_connecting()
{
    WiFi.removeEvent(wifi_event_id);
    info_ = nullptr;
    ip_ = nullptr;
}