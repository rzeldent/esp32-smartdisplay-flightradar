#pragma once

#define PREFERENCES_NAMESPACE "flightradar"
#define PREFERENCE_WIFI_SSID "wifi_ssid"
#define PREFERENCE_WIFI_PASSWORD "wifi_password"

class settings
{
    String wifi_ssid;
    String wifi_password;
    String location;
    short range_km;
    bool imperial_units;
};