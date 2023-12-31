#pragma once

#define PREFERENCES_NAMESPACE "flightradar"
#define PREFERENCE_WIFI_SSID "wifi_ssid"
#define PREFERENCE_WIFI_PASSWORD "wifi_password"
#define PREFERENCE_LOCATION_CITY "location_city"
#define PREFERENCE_SETTINGS_RANGE "settings_range"
#define PREFERENCE_SETTINGS_IMPERIAL "settings_mile"

class settings
{
public:
    String wifi_ssid;
    String wifi_password;
    String location;
    short range_km;
    bool imperial_units;
};