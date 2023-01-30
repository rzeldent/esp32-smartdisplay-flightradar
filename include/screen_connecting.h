#pragma once

#include <WifiType.h>
#include <ReactiveArduinoLib.h>
using namespace Reactive;
#include <screen.h>

class screen_connecting : public screen, public IObserver<wl_status_t>
{
public:
  screen_connecting(ObservableProperty<wl_status_t> *observable_wifi_status);
  ~screen_connecting();

  void OnNext(wl_status_t value);
  void OnComplete();

private:
  ObservableProperty<wl_status_t> *observable_wifi_status_;

  lv_obj_t *_info;
  lv_obj_t *_ip;
};