#pragma once

#include <WiFi.h>
#include <esp_wifi.h>
#include <vector>

struct wifi_ap_record : wifi_ap_record_t
{
    std::string ssid_string() const;
};

class access_poins_info : public std::vector<wifi_ap_record>
{
public:
    access_poins_info();
};