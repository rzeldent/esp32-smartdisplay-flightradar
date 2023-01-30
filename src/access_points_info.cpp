#include <access_points_info.h>
#include <esp32-hal-log.h>

#include <iterator>

void access_points_info::scan(bool clear /*=true*/)
{
  log_i("Scanning for networks");
  if (clear)
    _access_points.clear();

  auto scan_count = WiFi.scanNetworks();
  if (scan_count == 0)
  {
    log_w("No networks found!");
    return;
  }

  for (int16_t i = 0; i < scan_count; ++i)
  {
    log_i("%d", i);
    auto found_ap = access_point_t({ WiFi.SSID(i), WiFi.RSSI(i), WiFi.encryptionType(i)});
//    auto pos = std::find_if(_access_points.begin(), _access_points.end(), [&found_ap](const access_point_t &ap)
    //                        { return ap._ssid == found_ap._ssid; });
  //  if (pos == _access_points.cend())
      _access_points.push_back(found_ap);
    //else
    //{
      //if (pos->_rssi < found_ap._rssi)
        //pos->_rssi = found_ap._rssi;
   // }
  }
  log_i("Networks found: %d", _access_points.size());

  // Sort descending on signal strength
  std::sort(_access_points.begin(), _access_points.end(), [](const access_point_t &first, const access_point_t &second)
            { return first._rssi > second._rssi; });
}