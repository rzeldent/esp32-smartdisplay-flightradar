#pragma once

#include <screen.h>

class screen_connecting : public screen
{
public:
    screen_connecting(const std::string& ssid);
   
    void set_status(const std::string &status) const;

private:
    lv_obj_t *info_;
};