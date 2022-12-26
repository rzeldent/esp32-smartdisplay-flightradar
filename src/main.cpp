#include <esp_wifi.h>
#include <Arduino.h>
#include <ArduinoOTA.h>

#include <esp32_smartdisplay.h>

#include <screen_settings.h>

enum
{
  disconnected,
  wifi_settings,
  connecting,
  connected
} main_state;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  smartdisplay_init();

  // Clear screen
  lv_obj_clean(lv_scr_act());

  auto screen = create_settings_screen();

  wifi_config_t current_config;
  if (esp_wifi_get_config((wifi_interface_t)ESP_IF_WIFI_STA, &current_config) == ESP_OK)
  {
    // Configuration present. Try to connect
    main_state = disconnected;
    WiFi.begin();
  }
  else
    main_state = wifi_settings;

  // Allow over the air updates
  ArduinoOTA.begin();

  // Set the time servers
  configTime(0, 0, "pool.ntp.org");
}

lv_obj_t *main_screen()
{
  static auto container = lv_obj_create(nullptr);
  lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
}

lv_obj_t *screen_connecting()
{
  static auto container = lv_obj_create(nullptr);
  lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

  wifi_config_t current_config;
  esp_wifi_get_config((wifi_interface_t)ESP_IF_WIFI_STA, &current_config);
  String ssid(current_config.sta.ssid, sizeof(current_config.sta.ssid));

  static auto title = lv_label_create(container);
  lv_label_set_text(title, ("Connecting to: " + ssid).c_str());
  lv_obj_align(title, LV_ALIGN_CENTER, 0, -50);

  static auto spinner = lv_spinner_create(container, 1000, 50);
  lv_obj_set_size(spinner, 50, 50);
  lv_obj_center(spinner);

  return container;
}

lv_obj_t *screen_connected()
{
  static auto container = lv_obj_create(nullptr);
  lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

  static auto title = lv_label_create(container);
  lv_label_set_text(title, "Connected!");
  lv_obj_align(title, LV_ALIGN_CENTER, 0, -50);

  return container;
}

void loop()
{
  // put your main code here, to run repeatedly:

  switch (main_state)
  {
    case wifi_settings:
    break;
    
  case disconnected:
    main_state = connecting;
    lv_scr_load(screen_connecting());
    break;
  case connecting:
    if (WiFi.isConnected())
    {
      main_state = connected;
      lv_scr_load(screen_connected());
    }
    break;
  case connected:
    if (!WiFi.isConnected())
    {
      main_state = disconnected;
    }
    break;
  }

  // Red if no wifi, otherwise green
  //  bool connected = WiFi.isConnected();
  //  smartdisplay_set_led_color(connected ? 0 : LED_PWM_MAX, connected ? LED_PWM_MAX : 0, 0);

  // Handle display/touch
  lv_timer_handler();
  // Handle OTA
  ArduinoOTA.handle();
}