#pragma once

#include <WiFi.h>
#include <esp_wifi.h>

#include <vector>

typedef struct
{
  String _ssid;
  int32_t _rssi;
  wifi_auth_mode_t _encryption;
} access_point_t;

class access_points_info
{
public:
  void scan(bool clear = true);

  std::vector<access_point_t> _access_points;
};