#pragma once

#include <WifiType.h>
#include <ReactiveArduinoLib.h>
using namespace Reactive;
#include <screen.h>

class screen_connecting : public IObserver<wl_status_t>, public screen
{
 public:
  screen_connecting(ObservableProperty<wl_status_t>* observable_wifi_status);
  virtual ~screen_connecting();

  void OnNext(wl_status_t value);
  void OnComplete();

 private:
    ObservableProperty<wl_status_t>* observable_wifi_status_;

  // Static because no user call callback information possible
  static lv_obj_t *_info;
  static lv_obj_t *_ip;
//  static void wifi_station_event_callback(WiFiEvent_t event, WiFiEventInfo_t info);

//  wifi_event_id_t wifi_event_id_;
};