#pragma once

#include <screen.h>
#include <access_poins_info.h>

class screen_settings : public screen
{
public:
    screen_settings();
   
private:
    access_poins_info access_points;
    static void list_event_handler(lv_event_t * e);
};