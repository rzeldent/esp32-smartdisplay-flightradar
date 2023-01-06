#include <WiFi.h>
#include <esp_wifi.h>
#include <screen_connecting.h>

screen_connecting::screen_connecting(const std::string& ssid)
{
    auto title = lv_label_create(screen_);
    lv_label_set_text(title, ("Connecting to: " + ssid).c_str());
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -50);

    auto spinner = lv_spinner_create(screen_, 1000, 50);
    lv_obj_set_size(spinner, 50, 50);
    lv_obj_center(spinner);

    info_ = lv_label_create(screen_);
    lv_obj_align(info_, LV_ALIGN_CENTER, 0, 60);
}

void screen_connecting::set_status(const std::string& status) const
{
    lv_label_set_text(info_, status.c_str());
}
