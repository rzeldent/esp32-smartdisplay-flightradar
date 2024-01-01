#pragma once

#include <WiFi.h>
#include <string>
#include <vector>

struct access_point
{
    std::string ssid_;
    unsigned char rssi_;
    wifi_auth_mode_t authmode_;

    access_point();
    access_point(const wifi_ap_record_t *);
    const access_point &operator=(const access_point &other);
};

 std::vector<access_point> wifi_scan();
