#include <Arduino.h>
#include <WiFi.h>

#include <string>
#include <list>
#include <vector>

#include <esp32_smartdisplay.h>
#include <ui/ui.h>

#include <settings.h>
#include <wifi_scan.h>

#include <airport.h>
#include <city.h>

#include <format_gps.h>
#include <format_number.h>

#include <flight_info.h>

// forward declaration
void display_flights();

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

// put your main code here, to run repeatedly:
void loop()
{
  // Handle display/touch
  lv_timer_handler();

  if (WiFi.isConnected())
  {
    display_flights();
  }
  else
    ;
}

// Variables for flight info
city_t *city;
unsigned long next_update;
location_info location;
// List of flights
std::list<flight_info> flights;
// Flight to display
std::list<flight_info>::const_iterator it = flights.cend();

bool airborne = true;
bool grounded = false;
bool gliders = true;
bool vehicles = false;

// Refresh screen every 10 seconds
constexpr auto flight_milliseconds = 10 * 1000ul;
constexpr auto flight_milliseconds_error = 60 * 1000ul;
constexpr auto flight_milliseconds_noflights = 30 * 1000ul;
// Allow 4 cycles of each flight
constexpr auto display_cycles = 4;

void display_flight(std::list<flight_info>::const_iterator it)
{
  log_i("flight: %s", it->toString().c_str());
  lv_label_set_text(ui_Flight, it->flight.c_str());

  lv_label_set_text(ui_Altitude, String(it->altitude).c_str());

  lv_label_set_text(ui_VerticalSpeed, String(it->vertical_speed).c_str());

  lv_label_set_text(ui_GroundSpeed, String(it->ground_speed).c_str());

  lv_label_set_text(ui_LatLon, format_gps_location(it->latitude, it->longitude).c_str());

  lv_label_set_text(ui_Heading, String(it->heading).c_str());

  lv_label_set_text(ui_Registration, it->registration.c_str());

  lv_label_set_text(ui_AircraftType, it->aircraft_code.c_str());

  lv_label_set_text(ui_Airline, it->icao_airline.c_str());

  lv_label_set_text(ui_Origin, it->iata_origin_airport.c_str());

  lv_label_set_text(ui_Destination, it->iata_destination_airport.c_str());
}

void display_flights()
{
  auto now = millis();
  if (now > next_update)
  {
    if (it == flights.cend())
    {
      log_i("Updating flights");
      flights = get_flights(location, airborne, grounded, gliders, vehicles);

      log_i("Number of flights: %d", flights.size());
      if (flights.empty())
      {
        log_d("No flights in range");

        next_update = now + flight_milliseconds_noflights;
        return;
      }

      it = flights.cbegin();
    }

    display_flight(it++);

    next_update = now + flight_milliseconds;
  }
}

#ifdef __cplusplus
extern "C"
{
#endif
  void OnCalibrateClicked(lv_event_t *e)
  {
    log_i("OnClicked");
  }

  void OnSplashScreenLoaded(lv_event_t *e)
  {
    log_i("OnSplashScreenLoaded");

    settings nvram;
    // Check if settings are valid
    if (nvram.isValid())
    {
      // Calculate location
      city = settings::lookup_city(nvram.location_city_.c_str());
      location = location_info(city->latitude, city->longitude, nvram.settings_range_, nvram.settings_imperial_units_);

      WiFi.begin(nvram.wifi_ssid_, nvram.wifi_password_);
      if (WiFi.waitForConnectResult() == WL_CONNECTED)
      {
        log_i("Connected");
        _ui_screen_change(&ui_Main, LV_SCR_LOAD_ANIM_NONE, 500, 0, &ui_Main_screen_init);
        return;
      }
    }

    _ui_screen_change(&ui_Settings, LV_SCR_LOAD_ANIM_NONE, 500, 0, &ui_Settings_screen_init);
  }

  void OnSettingsScreenLoaded(lv_event_t *e)
  {
    log_i("OnSettingsScreenLoaded");

    settings nvram;

    std::list<std::string> ssids;
    // Make sure original ssid is at the top
    if (!nvram.wifi_ssid_.isEmpty())
      ssids.push_back(nvram.wifi_ssid_.c_str());

    // Do a network scan
    auto access_points = wifi_scan();
    // Create options for dropdown
    std::string options;
    for (const auto &it : access_points)
    {
      if (!options.empty())
        options.append("\n");

      options.append(it.ssid_.c_str());
    }

    // WiFi
    lv_dropdown_set_options(ui_WifiSsidDropdown, options.c_str());
    lv_textarea_set_text(ui_WifiPasswordTextArea, nvram.wifi_password_.c_str());
    lv_textarea_set_text(ui_LocationSearchTextArea, nvram.location_city_.c_str());
    lv_slider_set_value(ui_SettingsRangeSlider, nvram.settings_range_, LV_ANIM_OFF);
    nvram.settings_imperial_units_ ? lv_obj_add_state(ui_SettingsImperialSwitch, LV_STATE_CHECKED) : lv_obj_clear_state(ui_SettingsImperialSwitch, LV_STATE_CHECKED);
  }

  void OnSettingsOkClicked(lv_event_t *e)
  {
    log_i("OnSettingsOkClicked");

    WiFi.setAutoConnect(false);
    WiFi.disconnect();

    settings settings(true);
    char buf[sizeof(wifi_ap_config_t::ssid) + 1];
    lv_dropdown_get_selected_str(ui_WifiSsidDropdown, buf, sizeof(buf));
    settings.wifi_ssid_ = buf;
    settings.wifi_password_ = lv_textarea_get_text(ui_WifiPasswordTextArea);
    settings.location_city_ = lv_textarea_get_text(ui_LocationSearchTextArea);
    settings.settings_range_ = lv_slider_get_value(ui_SettingsRangeSlider);
    settings.settings_imperial_units_ = (lv_obj_get_state(ui_SettingsImperialSwitch) & LV_STATE_CHECKED) > 0;

    _ui_screen_change(&ui_Spash, LV_SCR_LOAD_ANIM_NONE, 500, 0, &ui_Spash_screen_init);
  }

  void OnCitySearchValueChanged(lv_event_t *e)
  {
    // Get value
    auto value = lv_textarea_get_text(ui_LocationSearchTextArea);
    log_i("Value: %s", value);
    auto city = settings::lookup_city(value);
    if (city)
    {
      auto cityFound = String(city->name) + " (" + String(countries[city->country].name) + ")";
      lv_label_set_text(ui_LocationFoundValueLabel, cityFound.c_str());
      auto latlong = format_gps_location(city->latitude, city->longitude);
      lv_label_set_text(ui_LabelLatLong, latlong.c_str());
    }
    else
    {
      lv_label_set_text(ui_LocationFoundValueLabel, "");
      lv_label_set_text(ui_LabelLatLong, "");
    }
  }

  void OnSliderRangeClicked(lv_event_t *e) {}
  void OnSwitchImperialClicked(lv_event_t *e) {}

#ifdef __cplusplus
} /*extern "C"*/
#endif
