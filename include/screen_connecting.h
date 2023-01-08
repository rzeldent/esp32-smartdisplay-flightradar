#pragma once

#include <screen.h>

class screen_connecting : public screen
{
public:
    screen_connecting();
    ~screen_connecting();

private:
    // Static because no user call callback information possible
    static lv_obj_t *info_;
    static lv_obj_t *ip_;
    static void wifi_station_event_callback(WiFiEvent_t event, WiFiEventInfo_t info);

    wifi_event_id_t wifi_event_id;
};