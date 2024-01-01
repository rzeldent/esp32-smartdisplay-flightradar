#pragma once

#include <city.h>

struct settings
{
    bool readonly_;
    
    String wifi_ssid_;
    String wifi_password_;
    String location_city_;
    short settings_range_;
    bool settings_imperial_units_;

    settings(bool writable = false);
    ~settings();

    bool isValid();
   
    static city_t* lookup_city(const char* city);
};