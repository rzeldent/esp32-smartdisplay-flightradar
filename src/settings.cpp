#include <Preferences.h>

#include <settings.h>

#define PREFERENCES_NAMESPACE "flightradar"
#define PREFERENCE_WIFI_SSID "wifi_ssid"
#define PREFERENCE_WIFI_PASSWORD "wifi_password"
#define PREFERENCE_LOCATION_CITY "location_city"
#define PREFERENCE_SETTINGS_RANGE "settings_range"
#define PREFERENCE_SETTINGS_IMPERIAL "settings_mile"

settings::settings(bool readonly /* = false*/)
{
    log_i("loading preferences");
    readonly_ = readonly;
    Preferences preferences;
    if (preferences.begin(PREFERENCES_NAMESPACE, true))
    {
        wifi_ssid_ = preferences.getString(PREFERENCE_WIFI_SSID);
        log_i("Preference loaded " PREFERENCE_WIFI_SSID ": %s", wifi_ssid_.c_str());
        wifi_password_ = preferences.getString(PREFERENCE_WIFI_PASSWORD);
        log_i("Preference loaded " PREFERENCE_WIFI_PASSWORD ": %s", wifi_password_.c_str());
        location_city_ = preferences.getString(PREFERENCE_LOCATION_CITY);
        log_i("Preference loaded " PREFERENCE_LOCATION_CITY ": %s", location_city_.c_str());
        settings_range_ = preferences.getUShort(PREFERENCE_SETTINGS_RANGE);
        log_i("Preference loaded " PREFERENCE_SETTINGS_RANGE ": %d", settings_range_);
        settings_imperial_units_ = preferences.getBool(PREFERENCE_SETTINGS_IMPERIAL);
        log_i("Preference loaded " PREFERENCE_SETTINGS_IMPERIAL ": %d", settings_imperial_units_);
    }
}

settings::~settings()
{
    log_i("saving preferences");
    if (!readonly_)
    {
        Preferences preferences;
        preferences.begin(PREFERENCES_NAMESPACE);

        preferences.putString(PREFERENCE_WIFI_SSID, wifi_ssid_);
        log_i("Preference saved " PREFERENCE_WIFI_SSID ": %s", wifi_ssid_.c_str());
        preferences.putString(PREFERENCE_WIFI_PASSWORD, wifi_password_);
        log_i("Preference saved " PREFERENCE_WIFI_PASSWORD ": %s", wifi_password_.c_str());
        preferences.putString(PREFERENCE_LOCATION_CITY, location_city_);
        log_i("Preference saved " PREFERENCE_LOCATION_CITY ": %s", location_city_.c_str());
        preferences.putUShort(PREFERENCE_SETTINGS_RANGE, settings_range_);
        log_i("Preference saved " PREFERENCE_SETTINGS_RANGE ": %d", settings_range_);
        preferences.putBool(PREFERENCE_SETTINGS_IMPERIAL, settings_imperial_units_);
        log_i("Preference saved " PREFERENCE_SETTINGS_IMPERIAL ": %d", settings_imperial_units_);
    }
}

bool settings::isValid()
{
    return !wifi_ssid_.isEmpty() &&  !wifi_password_.isEmpty() && settings_range_ > 0 && lookup_city(location_city_.c_str());
}

city_t *settings::lookup_city(const char *location)
{
    city_t key{.name = location};
    // Search case insensitive for the city
    auto city = (city_t *)std::bsearch(&key, &cities, sizeof(cities) / sizeof(city_t), sizeof(city_t), [](const void *e1, const void *e2)
                                       { return strcasecmp(((city_t *)e1)->name, ((city_t *)e2)->name); });
    log_i("City found: %s", city ? city->name : "<none>");
    return city;
}