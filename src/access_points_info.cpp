#include <access_points_info.h>
#include <esp32-hal-log.h>

#include <iterator>

void access_points_info::scan()
{
  log_i("Scanning for networks");

  clear();

  auto scan_count = WiFi.scanNetworks();
  if (scan_count <= 0)
  {
    log_w("No networks found!");
    return;
  }

  for (int16_t i = 0; i < scan_count; ++i)
  {
    auto found_ap = access_point_t({WiFi.SSID(i), WiFi.RSSI(i), WiFi.encryptionType(i)});
    auto pos = std::find_if(begin(), end(), [&found_ap](const access_point_t &ap)
                            { return ap._ssid == found_ap._ssid; });
    if (pos == end())
    {
      log_i("Add: %s", found_ap._ssid.c_str());
      push_back(found_ap);
    }
    else
    {
      if (pos->_rssi < found_ap._rssi)
      {
        pos->_rssi = found_ap._rssi;
        log_i("Updated RSSI");
      }
    }
  }

  // Sort descending on signal strength
  std::sort(begin(), end(), [](const access_point_t &first, const access_point_t &second)
            { return first._rssi > second._rssi; });

  log_i("Networks found: %d", size());
}