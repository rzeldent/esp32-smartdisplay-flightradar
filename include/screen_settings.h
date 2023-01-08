#pragma once

#include <screen.h>
#include <access_points_info.h>

class screen_settings : public screen
{
public:
    screen_settings();

private:
    lv_obj_t* wifi_list_;

    access_points_info access_points;
    static void list_event_handler(lv_event_t * e);
    static void button_refresh_handler(lv_event_t * e);
    static void button_event_handler(lv_event_t * e);

        void refresh();   
};