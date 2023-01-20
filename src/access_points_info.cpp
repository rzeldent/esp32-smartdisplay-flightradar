#include <access_points_info.h>
#include <esp32-hal-log.h>

#include <iterator>

std::string wifi_ap_record::ssid_string() const {
  return std::string((const char *)ssid, sizeof(ssid));
}

void access_points_info::refresh() {
  log_i("Scanning for networks");
  clear();
  auto scan_count = WiFi.scanNetworks();
  if (scan_count == 0) {
    log_w("No networks found!");
    return;
  }

  auto scan_results = static_cast<wifi_ap_record *>(WiFi.getScanInfoByIndex(0));
  std::vector<wifi_ap_record> scan_info(scan_results, scan_results + scan_count);
  log_i("Found %d networks", scan_info.size());

  for (auto it = scan_info.cbegin(); it != scan_info.cend(); ++it) {
    auto ssid = it->ssid_string();
    auto pos = std::find_if(scan_info.begin(), scan_info.end(), [&ssid, it](const wifi_ap_record &e) { return ssid == e.ssid_string(); });
    // Only insert or replace if not present or rssi is higher
    if (pos != scan_info.end() && pos->rssi > it->rssi)
      continue;

    push_back(*it);
  }

  // Sort descending on signal strength
  std::sort(begin(), end(), [](const wifi_ap_record &first, const wifi_ap_record &second) { return first.rssi > second.rssi; });
}