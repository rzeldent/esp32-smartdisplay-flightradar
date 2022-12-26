#include <WiFi.h>
#include <list>

#include <esp32_smartdisplay.h>
#include <screen_settings.h>

enum state
{
    none,
    searching,
    connecting,
    connected,
    connect_failed
};

struct wifi_info
{
    std::string ssid;
    uint32_t rssi;
    wifi_auth_mode_t encryption;

    wifi_info(const wifi_info &other)
    {
        ssid = other.ssid;
        rssi = other.rssi;
        encryption = other.encryption;
    }

    wifi_info(WiFiScanClass &_wifi_class, byte index)
    {
        ssid = std::string(_wifi_class.SSID(index).c_str());
        rssi = _wifi_class.RSSI(index);
        encryption = _wifi_class.encryptionType(index);
    }
};

lv_obj_t *create_settings_screen()
{
    auto container = lv_obj_create(nullptr);
    lv_obj_set_size(container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_ROW);
    // auto container = lv_menu_cont_create(container);
    // lv_obj_set_aut
    // lv_obj_set_auto_realign(cont, true);                    /*Auto realign when the size changes*/
    // lv_obj_align_origo(cont, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
    // lv_cont_set_fit(cont, LV_FIT_TIGHT);
    // lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_MID);

    auto title = lv_label_create(container);
    lv_label_set_text(title, "WIFI Settings");
    //    lv_obj_align(title, LV_ALIGN_TOP_RIGHT, 0, 0);
    //    lv_obj_set_size(title, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_layout(title, LV_LAYOUT_FLEX);

    // auto font_height = 20;

    auto wifi_list = lv_list_create(container);
    // lv_obj_align(wifi_list, LV_ALIGN_TOP_LEFT, 0, 0);
    // lv_obj_set_size(wifi_list, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_layout(wifi_list, LV_LAYOUT_GRID);

    auto networks = WiFi.scanNetworks();
    std::list<wifi_info> scanned_networks;
    for (auto i = 0; i < networks; ++i)
        scanned_networks.push_back(wifi_info(WiFi, i));

    //    for (auto it=scanned_networks.cbegin(); it != scanned_networks.cend(); ++it)

    // Keep highest rssid

    //

    for (auto it = scanned_networks.cbegin(); it != scanned_networks.cend(); ++it)
    {
        auto btn = lv_list_add_btn(wifi_list, LV_SYMBOL_WIFI, it->ssid.c_str());
        // lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, NULL);
        //  Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
    }

    return container;
}