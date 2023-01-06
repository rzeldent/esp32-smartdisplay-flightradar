#include <screen_settings.h>

enum state
{
    none,
    searching,
    connecting,
    connected,
    connect_failed
};

void screen_settings::list_event_handler(lv_event_t *e)
{
    auto that = static_cast<screen_settings*>(lv_event_get_param(e));
    lv_event_code_t code = lv_event_get_code(e);
     lv_obj_t *current_target = lv_event_get_current_target(e);
     lv_obj_t *target = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        auto ssid = lv_list_get_btn_text(current_target, target);
        log_i("Clicked: %s", ssid);
        //auto info = static_cast<wifi_ap_record *>(lv_event_get_param(e));
        //log_i("SSID=%s", info->ssid_string().c_str());
    }
}

screen_settings::screen_settings()
{
    auto title = lv_label_create(screen_);
    lv_label_set_text(title, "Available Access points");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(title, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    auto font_height = 20;

    auto wifi_list = lv_list_create(screen_);
    lv_obj_align(wifi_list, LV_ALIGN_TOP_LEFT, 0, font_height);
    lv_obj_set_size(wifi_list, lv_area_get_width(&screen_->coords), lv_area_get_height(&screen_->coords) - font_height);

    for (auto it = access_points.begin(); it != access_points.end(); ++it)
    {
        auto btn = lv_list_add_btn(wifi_list, LV_SYMBOL_WIFI, it->ssid_string().c_str());
        lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, static_cast<void *>(this));
        //  Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
    }
}