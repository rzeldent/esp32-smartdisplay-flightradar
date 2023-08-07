# ESP32-SMARTDISPLAY-FLIGHTRADAR

New version of the [flightradar](https://github.com/rzeldent/esp32-flightradar24-ttgo).
The original version was running on an TTGO display. This version uses LVGL.

The most interesting part is that this project uses [reactive programming](https://reactivex.io/).
The observer pattern is used to keep track of the states and update the display.