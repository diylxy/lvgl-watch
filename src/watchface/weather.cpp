#include "A_config.h"

#define WEATHER_PAGES 4
static uint8_t currentPage = 0;
static weatherInfo24H *currentWeather;
///////////////////////////////////////////////////////////
static lv_obj_t *scr_weather;
static lv_obj_t *lblheader;
static lv_obj_t *lbltemperature;
static lv_obj_t *lblUpdateTime;
//page-0
static lv_obj_t *weather_image;
//page-1
static lv_obj_t *lbldesc1;
static lv_obj_t *lbldesc2;
//page-2
static lv_obj_t *chart_rain;
//page-3
static lv_obj_t *lbl_hours[3][2];
static lv_obj_t *lbl_tip_3hour;
static void wf_weather_loop()
{
    if (hal.btnUp.isPressedRaw())
    {
        popWatchFace();
        return;
    }
    if (hal.btnEnter.isPressedRaw())
    {
        switch (currentPage)
        {
        case 0:
            lv_obj_move_anim(lblheader, -40, -70);
            lv_obj_move_anim(lbltemperature, 40, -70);
            lv_obj_fade_out(weather_image, 300, 0);
            lv_obj_set_pos(lbldesc2, 30, 140);
            lv_obj_fade_in(lbldesc1, 300, 0);
            lv_obj_fade_in(lbldesc2, 300, 0);
            break;
        case 1:
            lv_obj_fade_out(lbldesc1, 300, 0);
            lv_obj_move_anim(lbldesc2, 30, 60);
            lv_obj_pop_up(chart_rain);
            break;
        case 2:
            lv_obj_fall_down(lbldesc2);
            lv_obj_fade_out(chart_rain, 300, 0);
            for (uint8_t i = 0; i < 3; ++i)
            {
                lv_obj_pop_up(lbl_hours[i][0], 24, 300, i * 100);
                lv_obj_pop_up(lbl_hours[i][1], 24, 300, i * 100 + 50);
            }
            lv_obj_fade_in(lbl_tip_3hour, 500, 0);
            break;
        case 3:
            lv_obj_move_anim(lblheader, 0, -70);
            lv_obj_move_anim(lbltemperature, 0, 70);
            lv_obj_fade_in(weather_image, 300, 0);
            for (uint8_t i = 0; i < 3; ++i)
            {
                lv_obj_fade_out(lbl_hours[i][0], 300, i * 100);
                lv_obj_fade_out(lbl_hours[i][1], 300, i * 100 + 50);
            }
            lv_obj_fade_out(lbl_tip_3hour, 300, 0);
            break;
        default:
            currentPage = 0;
            break;
        }
        ++currentPage;
        if (currentPage == WEATHER_PAGES)
            currentPage = 0;
        while (hal.btnEnter.isPressedRaw())
            vTaskDelay(100);
    }
    vTaskDelay(100);
}

void wf_weather_load()
{
    currentWeather = weather.getWeather(hal.rtc.getMonth(), hal.rtc.getDate(), hal.rtc.getHour());
    if (currentWeather == NULL)
    {
        msgbox(LV_SYMBOL_WARNING " 警告", "天气信息已过时", 3000);
        popWatchFace();
        return;
    }
    currentPage = 0;
    REQUESTLV();
    scr_weather = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_weather, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_scr_load_anim(scr_weather, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300, 0, true);
    lblUpdateTime = lv_label_create(scr_weather);
    lv_label_set_text(lblUpdateTime, weather.hour24[0].date);
    lv_obj_set_style_text_color(lblUpdateTime, lv_color_white(), 0);
    lv_obj_align(lblUpdateTime, LV_ALIGN_CENTER, 0, 85);
    ////Page-0
    lbltemperature = lv_label_create(scr_weather);
    lblheader = lv_label_create(scr_weather);
    weather_image = lv_label_create(scr_weather);
    lv_obj_set_size(weather_image, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(weather_image, &font_weather_symbol_96, 0);
    lv_obj_set_style_text_color(weather_image, lv_color_white(), 0);
    lv_obj_set_style_text_font(lblheader, &font_weather_32, 0);
    lv_obj_set_style_text_color(lblheader, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbltemperature, &font_weather_num_24, 0);
    lv_obj_set_style_text_color(lbltemperature, lv_color_white(), 0);
    lv_obj_align(weather_image, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(lblheader, LV_ALIGN_CENTER, 0, -70);
    lv_obj_align(lbltemperature, LV_ALIGN_CENTER, 0, 70);
    lv_label_set_text(weather_image, weather_icons[currentWeather->weathernum]);
    lv_label_set_text(lblheader, weather_names[currentWeather->weathernum]);
    lv_label_set_text_fmt(lbltemperature, "%d℃", currentWeather->temperature / 10);
    ////Page-1
    lbldesc1 = lv_label_create(scr_weather);
    lbldesc2 = lv_label_create(scr_weather);
    lv_obj_set_style_text_font(lbldesc1, &lv_font_chinese_16, 0);
    lv_obj_set_style_text_font(lbldesc2, &lv_font_chinese_16, 0);
    lv_obj_set_style_text_color(lbldesc1, lv_color_white(), 0);
    lv_obj_set_style_text_color(lbldesc2, lv_color_white(), 0);
    lv_obj_set_width(lbldesc1, 180);
    lv_obj_set_width(lbldesc2, 180);
    lv_label_set_long_mode(lbldesc1, LV_LABEL_LONG_WRAP);
    lv_label_set_long_mode(lbldesc2, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_opa(lbldesc1, 0, 0);
    lv_obj_set_style_opa(lbldesc2, 0, 0);
    lv_obj_set_pos(lbldesc1, 30, 80);
    lv_obj_set_pos(lbldesc2, 30, 140);
    lv_label_set_text(lbldesc1, weather.desc1);
    lv_label_set_text(lbldesc2, weather.desc2);
    //Page-2
    chart_rain = lv_chart_create(scr_weather);
    lv_obj_set_size(chart_rain, 140, 80);
    lv_obj_set_pos(chart_rain, 50, 110);
    lv_chart_set_range(chart_rain, LV_CHART_AXIS_PRIMARY_Y, 0, 2600);
    int16_t m = 0;
    for (int8_t i = 0; i < 120; ++i)
    {
        if (weather.rain[i] > m)
        {
            m = weather.rain[i];
        }
    }
    if (m > 2600)
        lv_chart_set_range(chart_rain, LV_CHART_AXIS_PRIMARY_Y, 0, m);
    lv_obj_set_style_size(chart_rain, 0, LV_PART_INDICATOR);
    lv_chart_series_t *ser = lv_chart_add_series(chart_rain, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_point_count(chart_rain, 120); //点数
    lv_chart_set_ext_y_array(chart_rain, ser, (lv_coord_t *)weather.rain);
    lv_obj_set_style_opa(chart_rain, 0, 0);
    ////Page-3
    uint8_t date = hal.rtc.getDate();
    uint8_t month = hal.rtc.getMonth();
    uint8_t hour = hal.rtc.getHour();
    for (uint8_t i = 0; i < 3; ++i)
    {
        weatherInfo24H *weather1;
        ++hour;
        if (hour == 24)
        {
            hour = 0;
            ++date;
            if (date > 30)
            {
                date = 1;
                ++month; //只要不在每月最后一天21:00后查看天气就不会有问题，还能减少不少判断步骤
            }
        }
        weather1 = weather.getWeather(month, date, hour);
        lbl_hours[i][0] = lv_label_create(scr_weather);
        lbl_hours[i][1] = lv_label_create(scr_weather);
        lv_obj_set_style_text_font(lbl_hours[i][0], &font_weather_32, 0);
        lv_obj_set_style_text_color(lbl_hours[i][0], lv_color_white(), 0);
        lv_obj_set_style_text_font(lbl_hours[i][1], &font_weather_num_24, 0);
        lv_obj_set_style_text_color(lbl_hours[i][1], lv_color_white(), 0);
        lv_obj_set_pos(lbl_hours[i][0], 20 + 70 * i, 90);
        lv_obj_set_pos(lbl_hours[i][1], 30 + 70 * i, 150);
        lv_obj_set_style_opa(lbl_hours[i][0], 0, 0);
        lv_obj_set_style_opa(lbl_hours[i][1], 0, 0);
        if (weather1 == NULL)
        {
            lv_label_set_text(lbl_hours[i][0], "");
            lv_label_set_text(lbl_hours[i][1], "--");
            continue;
        }
        lv_label_set_text(lbl_hours[i][0], weather_names[weather1->weathernum]);
        lv_label_set_text_fmt(lbl_hours[i][1], "%d℃", weather1->temperature / 10);
    }
    lbl_tip_3hour = lv_label_create(scr_weather);
    lv_obj_set_style_text_font(lbl_tip_3hour, &lv_font_chinese_16, 0);
    lv_label_set_text(lbl_tip_3hour, "未来三小时");
    lv_obj_set_style_text_color(lbl_tip_3hour, lv_color_white(), 0);
    lv_obj_align(lbl_tip_3hour, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_opa(lbl_tip_3hour, 0, 0);
    lv_obj_floating_add(lbl_tip_3hour, 0);
    RELEASELV();
    hal.fLoop = wf_weather_loop;
}