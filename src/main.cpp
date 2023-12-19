#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ReactiveArduinoLib.h>
#include <esp32-hal-log.h>
#include <esp32_smartdisplay.h>
#include <esp_wifi.h>

#include <arduino_event_names.h>

#include <screen_connecting.h>
#include <screen_settings.h>
#include <screen_main.h>

typedef enum : byte
{
  state_disconnected,
  state_connecting,
  state_connect_error,
  state_configure,
  state_main
} main_state;

const char *main_state_string[] = {"Disconnected", "Connecting", "ConnectError", "Configure", "Main"};

typedef enum : byte
{
  event_disconnected,
  event_connected,
  event_connect_failed,
  event_configured
} main_event;

const char *main_event_string[] = {"Disconnected", "Connected", "Connect failed", "Configured"};

ObservableProperty<arduino_event_t *> observable_wifi_status;
ObservableProperty<main_state> observable_main_state;
ObservableProperty<main_event> observable_main_event;

std::unique_ptr<screen> screen;

void on_wifi_event(arduino_event_t *event)
{
  observable_wifi_status = event;
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
      .Map<bool>([](arduino_event_t *event)
                 { return event->event_id == ARDUINO_EVENT_WIFI_STA_CONNECTED; })
      .Distinct()
      .Do([](bool connected)
          {
        log_d("Connected: %d", connected);
        smartdisplay_set_led_color(connected ? lv_color32_t({.ch = {.green = 0xFF}}) : lv_color32_t({.ch = {.red = 0xFF}})); });

  // Wifi Events
  observable_wifi_status
      //.Distinct()
      .Do([](arduino_event_t *event)
          {
            log_i("ARDUINO_EVENT_%s", arduino_event_names[event->event_id]);
            switch (event->event_id) {
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
              switch (event->event_info.wifi_sta_disconnected.reason)
              {
                case WIFI_REASON_NO_AP_FOUND:
                case WIFI_REASON_AUTH_FAIL:
                  observable_main_event = main_event::event_connect_failed;
                  break;
                  case WIFI_REASON_ASSOC_LEAVE:
                  // TODO: Disconnected voluntary
                  break;
                default:
                  observable_main_event = main_event::event_disconnected;
                  break;
              }
              break;
            case ARDUINO_EVENT_WIFI_STA_CONNECTED:
              observable_main_event = main_event::event_connected;
              break;
            case ARDUINO_EVENT_WIFI_STA_STOP:
              observable_main_event = main_event::event_connect_failed;
              break;
          } });

  // State machine
  observable_main_event
      //.Distinct()
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
                screen.reset(new screen_main());
                state = state_main;
                break;
              case event_connect_failed:
                screen.reset(new screen_settings());
                state = state_configure;
                break;
              }
              break;
            case state_configure:
              switch (event)
              {
              case event_configured:
                state = state_disconnected;
                break;
              }
              break;
            case state_main:
              break;
            }
            log_i("State: %s", main_state_string[state]);
            return state;
          },
          state_disconnected)
      .Do([](main_state state)
          { observable_main_state = state; });

  WiFi.setAutoReconnect(false);
  // WiFi.begin("xxx", "yy");
  WiFi.begin();

  // Allow over the air updates
  ArduinoOTA.begin();

  // Set the time servers
  configTime(0, 0, "pool.ntp.org");
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Handle display/touch
  {
    const std::lock_guard<std::recursive_mutex> lock(lvgl_mutex);
    lv_timer_handler();
  }

  // Handle OTA
  ArduinoOTA.handle();
}