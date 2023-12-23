#include <Arduino.h>
#include <WiFi.h>

#include <vector>

#include <esp32_smartdisplay.h>
#include <ui/ui.h>

#include <settings.h>

#include <Preferences.h>

#include <airport.h>
#include <city.h>

#include <format_gps.h>
#include <format_number.h>

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  log_i("CPU: %s rev%d, CPU Freq: %d Mhz, %d core(s)", ESP.getChipModel(), ESP.getChipRevision(), getCpuFrequencyMhz(), ESP.getChipCores());
  log_i("Flash      : %s", format_memory(ESP.getFlashChipSize()).c_str());
  log_i("Free heap  : %s", format_memory(ESP.getFreeHeap()).c_str());
  log_i("Free PSRAM : %s", format_memory(ESP.getPsramSize()).c_str());
  log_i("SDK version: %s", ESP.getSdkVersion());

  smartdisplay_init();
  auto disp = lv_disp_get_default();
  lv_disp_set_rotation(disp, LV_DISP_ROT_90);

  // Start monitoring the connection
  // log_d("Connected: %d", connected);
  // digitalWrite(LED_R_GPIO, connected);  // on if disconnected
  // digitalWrite(LED_G_GPIO, !connected);  // on if connected
  // digitalWrite(LED_B_GPIO, true); // off

  ui_init();

  WiFi.setAutoReconnect(false);
  // WiFi.begin("xxx", "yy");
  // WiFi.begin();

  // Set the time servers
  configTime(0, 0, "pool.ntp.org");
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Handle display/touch
  lv_timer_handler();
}

#ifdef __cplusplus
extern "C"
{
#endif
  void OnSplashScreenLoaded(lv_event_t *e)
  {
    log_i("OnSplashScreenLoaded");
    Preferences preferences;
    preferences.begin(PREFERENCES_NAMESPACE, true);

    if (preferences.isKey(PREFERENCE_WIFI_SSID) && preferences.isKey(PREFERENCE_WIFI_PASSWORD))
    {
      auto wifi_ssid = preferences.getString(PREFERENCE_WIFI_SSID);
      log_i("Preference loaded " PREFERENCE_WIFI_SSID ": %s", wifi_ssid.c_str());
      auto wifi_password = preferences.getString(PREFERENCE_WIFI_PASSWORD);
      log_i("Preference loaded " PREFERENCE_WIFI_PASSWORD": %s", wifi_password.c_str());
      if (wifi_ssid.length() > 0)
      {
        // WiFi.begin(wifi_ssid, wifi_password);
        // if (WiFi.waitForConnectResult() == WL_CONNECTED)
        // {
        //   log_i("Connected");
        // }
        ;
      }
    }

    //     lv_disp_load_scr(ui_Settings);
    _ui_screen_change(&ui_Settings, LV_SCR_LOAD_ANIM_NONE, 500, 0, &ui_Settings_screen_init);
  }

  void OnSettingsScreenLoaded(lv_event_t *e)
  {
    log_i("OnSettingsScreenLoaded");

    Preferences preferences;
    preferences.begin(PREFERENCES_NAMESPACE, true);

    String values;
    auto num_networks = WiFi.scanNetworks();
    if (num_networks)
    {
      auto scan_info = (wifi_ap_record_t *)WiFi.getScanInfoByIndex(0);
      std::vector<wifi_ap_record_t> networks(scan_info, &scan_info[num_networks]);
      // Sort by highest rssi
      // std::sort(networks.begin(), networks.end(), [](const wifi_ap_record_t* e1, const wifi_ap_record_t* e2)
      //            { return e1->rssi > e2->rssi; });
      // Remove duplicates
      // std::remove_if(networks.begin(), networks.end(), []((const wifi_ap_record_t* e){std::find(networks.begin(), networks.end(), )} );

      for (auto network : networks)
      {
        if (values.length())
          values += "\n";

        values += String(reinterpret_cast<const char *>(network.ssid));
      }

      lv_dropdown_set_options(ui_SettingsWifiSsidDropdown, values.c_str());
    }
  }

  void OnSettingsOkClicked(lv_event_t *e)
  {
    log_i("OnSettingsOkClicked");

    WiFi.setAutoConnect(false);
    WiFi.disconnect();

    Preferences preferences;
    preferences.begin(PREFERENCES_NAMESPACE, false);

    char wifi_ssid[64];
    lv_dropdown_get_selected_str(ui_SettingsWifiSsidDropdown, wifi_ssid, sizeof(wifi_ssid));
    preferences.putString(PREFERENCE_WIFI_SSID, wifi_ssid);
    log_i("Preference saved " PREFERENCE_WIFI_SSID ": %s", wifi_ssid);
    auto wifi_password = lv_textarea_get_text(ui_SettingsWifiPassword);
    preferences.putString(PREFERENCE_WIFI_PASSWORD, wifi_password);
    log_i("Preference saved " PREFERENCE_WIFI_PASSWORD ": %s", wifi_ssid);

    _ui_screen_change(&ui_Spash, LV_SCR_LOAD_ANIM_NONE, 500, 0, &ui_Spash_screen_init);
  }

  void OnCitySearchValueChanged(lv_event_t *e)
  {
    // Get value
    auto value = lv_textarea_get_text(e->current_target);
    log_i("Value: %s", value);
    city_t key{.name = value};
    auto city = (city_t *)std::bsearch(&key, &cities, sizeof(cities) / sizeof(city_t), sizeof(city_t), [](const void *e1, const void *e2)
                                       { return strcasecmp(((city_t *)e1)->name, ((city_t *)e2)->name); });
    if (city)
    {
      log_i("Found city: %s", city->name);
      auto cityFound = String(city->name) + " (" + String(countries[city->country].name) + ")";
      lv_label_set_text(ui_LabelCityFound, cityFound.c_str());
      auto latlong = format_gps_location(city->latitude, city->longitude);
      lv_label_set_text(ui_LabelLatLong, latlong.c_str());
    }
    else
    {
      lv_label_set_text(ui_LabelCityFound, "");
      lv_label_set_text(ui_LabelLatLong, "");
    }
  }

  void OnSliderRangeClicked(lv_event_t *e) {}
  void OnSwitchImperialClicked(lv_event_t *e) {}

#ifdef __cplusplus
} /*extern "C"*/
#endif
