#include <wifi_scan.h>

access_point::access_point()
{
}

access_point::access_point(const wifi_ap_record_t *wapr)
{
    auto len = strnlen((const char *)wapr->ssid, sizeof(wifi_ap_record_t::ssid));
    ssid_ = std::string((const char *)wapr->ssid, len);
    rssi_ = wapr->rssi;
    authmode_ = wapr->authmode;
}

const access_point &access_point::operator=(const access_point &other)
{
    if (this == &other)
        return *this;

    ssid_ = other.ssid_;
    rssi_ = other.rssi_;
    authmode_ = other.authmode_;
    return *this;
}

std::vector<access_point> wifi_scan()
{
    auto num_networks = WiFi.scanNetworks();
    std::vector<access_point> raw_scan(num_networks);
    for (ushort i = 0; i < num_networks; i++)
        raw_scan[i] = access_point((const wifi_ap_record_t *)WiFi.getScanInfoByIndex(i));

    // Sort by highest rssi
    std::sort(raw_scan.begin(), raw_scan.end(), [](const access_point &e1, const access_point &e2)
              { return e1.rssi_ > e2.rssi_; });
    // Unique iterator
    auto it = std::unique(raw_scan.begin(), raw_scan.end(), [](const access_point &e1, const access_point &e2)
                            { return e1.ssid_ == e2.ssid_; });
    // Remove duplicates from the scan
    raw_scan.erase(it, raw_scan.end());
    return raw_scan;
}