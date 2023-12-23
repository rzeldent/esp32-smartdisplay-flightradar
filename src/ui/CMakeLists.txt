SET(SOURCES screens/ui_Spash.c
    screens/ui_Settings.c
    screens/ui_Main.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.cpp
    images/ui_img_radar320x240_png.c)

add_library(ui ${SOURCES})
