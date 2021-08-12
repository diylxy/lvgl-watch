#include "A_config.h"

static lv_obj_t *scr_weather;
static lv_obj_t *lblheader;
static lv_obj_t *weather_image;
static lv_obj_t *lbltemperature;
static uint8_t currentTime = 0;
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
    currentTime = hal.rtc.getHour();
    REQUESTLV();
    scr_weather = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_weather, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_scr_load_anim(scr_weather, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0, true);

    weather_image = lv_img_create(scr_weather);
    lv_obj_align(weather_image, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(weather_image, 69, 61);
    lv_img_set_src(weather_image, weather.weatherNumtoImage(weather.hour24[currentTime].weathernum));

    lblheader = lv_label_create(scr_weather);
    lv_label_set_text(lblheader, weather.hour24[currentTime].weatherName.c_str());
    lv_obj_set_style_text_font(lblheader, &font_weather_32, 0);
    lv_obj_align(lblheader, LV_ALIGN_CENTER, 0, -80);
    lv_obj_set_style_text_color(lblheader, lv_color_white(), 0);

    lbltemperature = lv_label_create(scr_weather);
    lv_label_set_text_fmt(lbltemperature, "%d℃", weather.hour24[currentTime].temperature);
    lv_obj_set_style_text_font(lbltemperature, &font_weather_num_24, 0);
    lv_obj_align(lbltemperature, LV_ALIGN_CENTER, 0, 30);
    RELEASELV();
    hal.fLoop = wf_weather_loop;
}