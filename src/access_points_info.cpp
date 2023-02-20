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
    auto ap = access_point_t({WiFi.SSID(i), WiFi.RSSI(i), WiFi.encryptionType(i)});
    auto it = find_by_ssid(ap._ssid);
    if (it == end())
    {
      log_i("Add: %s", ap._ssid.c_str());
      push_back(ap);
    }
    else
    {
      if (it->_rssi < ap._rssi)
      {
        it->_rssi = ap._rssi;
        log_i("Updated RSSI");
      }
    }
  }

  // Sort descending on signal strength
  std::sort(begin(), end(), [](const access_point_t &first, const access_point_t &second)
            { return first._rssi > second._rssi; });

  log_i("Networks found: %d", size());
}

std::vector<access_point_t>::iterator access_points_info::find_by_ssid(const String &ssid)
{
  return std::find_if(begin(), end(), [&ssid](const access_point_t &ap)
                      { return ap._ssid == ssid; });
}