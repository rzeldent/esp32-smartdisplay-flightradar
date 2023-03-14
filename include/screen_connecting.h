#pragma once

#include <WifiType.h>
#include <ReactiveArduinoLib.h>
using namespace Reactive;
#include <screen.h>

class screen_connecting : public screen, public IObserver<arduino_event_t*>
{
public:
  screen_connecting(ObservableProperty<arduino_event_t*> *observable_wifi_status);
  ~screen_connecting();

  void OnNext(arduino_event_t* value);
  void OnComplete();

private:
  ObservableProperty<arduino_event_t*> *_observable_arduino_status;
  lv_obj_t *_info;
  lv_obj_t *_ip;
};