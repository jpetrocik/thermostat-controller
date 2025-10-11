SET(SOURCES ui_Main.c
    ui_theme_manager.c
    ui_themes.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    ui_img_flame_off_png.c
    ui_img_down_arrow_png.c
    ui_img_up_arrow_png.c
    ui_img_wifi_off_png.c
    ui_img_wifi_png.c
    ui_img_flame_png.c
    fonts/ui_font_Ubuntu_Bold_180.c
    fonts/ui_font_Ubuntu_Bold__45.c
    fonts/ui_font_Ubuntu_Bold__75.c
    fonts/ui_font_Ubuntu_Regular_45.c
    fonts/ui_font_Ubuntu_Regular__20.c)

add_library(ui ${SOURCES})
