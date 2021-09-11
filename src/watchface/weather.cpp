#include "A_config.h"

static lv_obj_t *scr_weather;
static lv_obj_t *lblheader;
static lv_obj_t *weather_image;
static lv_obj_t *lbltemperature;
static weatherInfo24H* currentWeather;
static void wf_weather_loop()
{
    if(hal.btnUp.isPressedRaw())
    {
        popWatchFace();
        return;
    }
    vTaskDelay(100);
}

void wf_weather_load()
{
    currentWeather = weather.getWeather(hal.rtc.getMonth(), hal.rtc.getDate(),hal.rtc.getHour());
    REQUESTLV();
    scr_weather = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_weather, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_scr_load_anim(scr_weather, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0, true);

    weather_image = lv_label_create(scr_weather);
    lv_obj_align(weather_image, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(weather_image, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(weather_image, &font_weather_symbol_96, 0);
    lv_label_set_text(weather_image, weather_icons[currentWeather->weathernum]);
    lv_obj_set_style_text_color(weather_image, lv_color_white(), 0);

    lblheader = lv_label_create(scr_weather);
    lv_label_set_text(lblheader, weather_names[currentWeather->weathernum]);
    lv_obj_set_style_text_font(lblheader, &font_weather_32, 0);
    lv_obj_align(lblheader, LV_ALIGN_CENTER, 0, -70);
    lv_obj_set_style_text_color(lblheader, lv_color_white(), 0);

    lbltemperature = lv_label_create(scr_weather);
    lv_label_set_text_fmt(lbltemperature, "%d℃", currentWeather->temperature/10);
    lv_obj_set_style_text_font(lbltemperature, &font_weather_num_24, 0);
    lv_obj_align(lbltemperature, LV_ALIGN_CENTER, 0, 70);
    lv_obj_set_style_text_color(lbltemperature, lv_color_white(), 0);
    RELEASELV();
    hal.fLoop = wf_weather_loop;
}