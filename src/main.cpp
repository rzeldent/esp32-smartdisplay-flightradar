#include <esp32-hal-log.h>
#include <esp_wifi.h>
#include <Arduino.h>
#include <ArduinoOTA.h>

#include <esp32_smartdisplay.h>
//#include <ReactiveArduinoLib.h>

#include <screen_connecting.h>
#include <screen_settings.h>

enum
{
  idle,
  connecting,
  connected,
  disconnected,
  wifi_settings
} main_state;

std::unique_ptr<screen> screen;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  log_i("CPU Freq: %d Mhz", getCpuFrequencyMhz());
  log_i("Free heap: %d bytes", ESP.getFreeHeap());

  smartdisplay_init();

  // Clear screen
  //  lv_obj_clean(lv_scr_act());

  if (WiFi.begin() != WL_CONNECT_FAILED)
    screen.reset(new screen_connecting());
  else
    screen.reset(new screen_settings());

  // Allow over the air updates
  ArduinoOTA.begin();

  // Set the time servers
  configTime(0, 0, "pool.ntp.org");
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Handle display/touch
  lv_timer_handler();
  // Handle OTA
  ArduinoOTA.handle();

  switch (main_state)
  {
  case wifi_settings:
    break;

  case disconnected:
    main_state = connecting;
    //    lv_scr_load(screen_connecting());
    break;
  case connecting:
    if (WiFi.isConnected())
    {
      main_state = connected;
      //    lv_scr_load(screen_connected());
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
  smartdisplay_set_led_color(WiFi.isConnected() ? 0 : LED_PWM_MAX, connected ? LED_PWM_MAX : 0, 0);
}