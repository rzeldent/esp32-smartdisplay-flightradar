#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ReactiveArduinoLib.h>
using namespace Reactive;
#include <esp32-hal-log.h>
#include <esp32_smartdisplay.h>
#include <esp_wifi.h>

#include <screen_connecting.h>
#include <screen_settings.h>

typedef enum : byte
{
  state_disconnected,
  state_connecting,
  state_connect_failed,
  state_configure,
  state_ready
} main_state;

const char *main_state_string[] = {"Disconnected", "Connecting", "Connect failed", "Configure", "Ready"};

typedef enum : byte
{
  event_disconnected,
  event_connected,
  event_connect_failed,
  event_configured
} main_event;

const char *main_event_string[] = {"Disconnected", "Connected", "Connect failed", "Configured"};

ObservableProperty<wl_status_t> observable_wifi_status;
ObservableProperty<main_state> observable_main_state;
ObservableProperty<main_event> observable_main_event;

std::unique_ptr<screen> screen;

void on_wifi_event(arduino_event_id_t event)
{
  observable_wifi_status = WiFi.status();
}

void setup()
{

  // put your setup code here, to run once:
  Serial.begin(115200);

  log_i("CPU Freq: %d Mhz", getCpuFrequencyMhz());
  log_i("Free heap: %d bytes", ESP.getFreeHeap());

  smartdisplay_init();

  // Start monitoring the connection
  WiFi.onEvent(on_wifi_event);
  // Led
  observable_wifi_status
      .Map<bool>([](wl_status_t status)
                 { return status == WL_CONNECTED; })
      .Distinct()
      .Do([](bool connected)
          {
        log_d("Connected: %d", connected);
        smartdisplay_set_led_color(connected ? lv_color32_t({.ch = {.green = 0xFF}}) : lv_color32_t({.ch = {.red = 0xFF}})); });

  // Wifi Events
  observable_wifi_status.Do([](wl_status_t status)
                            {
    switch (status) {
      case WL_DISCONNECTED:
        log_i("wifi_status: WL_DISCONNECTED");
        observable_main_event = main_event::event_disconnected;
        break;
      case WL_CONNECTED:
        log_i("wifi_status: WL_CONNECTED");
        observable_main_event = main_event::event_connected;
        break;
      case WL_CONNECT_FAILED:
        log_i("wifi_status: WL_CONNECT FAILED");
        observable_main_event = main_event::event_connect_failed;
        break;
    } });

  // State machine
  observable_main_event
      .Reduce<main_state>(
          [](main_state state, main_event event)
          {
            log_i("State: %s, Event: %s", main_state_string[state], main_event_string[event]);
            switch (state)
            {
            case state_disconnected:
              screen.reset(new screen_connecting(&observable_wifi_status));
              state = state_connecting;
              break;
            case state_connecting:
              switch (event)
              {
              case event_connected:
                state = state_ready;
                break;
              case event_connect_failed:
                state = state_configure;
                break;
              }
              break;
            case state_connect_failed:
              screen.reset(new screen_settings());
              state = state_configure;
              break;
            case state_configure:
              switch (event)
              {
              case event_configured:
                state = state_disconnected;
                break;
              }
              break;
            case state_ready:
              break;
            }
            log_i("New state: %s", main_state_string[state]);
            return state;
          },
          state_disconnected)
      .Do([](main_state state)
          { observable_main_state = state; });

  WiFi.begin();

  // Allow over the air updates
  // ArduinoOTA.begin();

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
}