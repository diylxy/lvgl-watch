#ifndef __WEATHER_H__
#define __WEATHER_H__
#include <ArduinoJson.h>
#include <Arduino.h>
#include <lvgl.h>
/*
0 晴
1 多云
2 阴
3 阵雨
4 雷阵雨
5 雷阵雨伴有冰雹
6 雨夹雪
7 小雨
8 中雨
9 大雨
10 暴雨
11 大暴雨
12 特大暴雨
13 阵雪
14 小雪
15 中雪
16 大雪
17 暴雪
18 雾
19 冻雨
20 沙尘暴
21 小雨-中雨
22 中雨-大雨
23 大雨-暴雨
24 暴雨-大暴雨
25 大暴雨-特大暴雨
26 小雪-中雪
27 中雪-大雪
28 大雪-暴雪
29 浮尘
30 扬沙
31 强沙尘暴
32 浓雾
49 强浓雾
53 霾
54 中度霾
55 重度霾
56 严重霾
57 大雾
58 特强浓雾
99 无
301 雨
302 雪
*/
//weather字体：有雹中晴扬大云雨特冰浓雪多冻霾雷夹重严尘无阵暴伴浮沙雾小强阴度
typedef struct
{
    uint16_t weathernum;
    String weatherName;
    int8_t temperature;
    bool isnextday;
}weatherInfo24H;

class Weather
{
private:
    uint8_t updateDate;
    bool started = false;
public:
    void begin();
    void save();
    int8_t refresh(String city);
    bool available(uint8_t date);
    const lv_img_dsc_t *weatherNumtoImage(uint16_t num);
    weatherInfo24H hour24[24];
    String cityName;
};
extern Weather weather;

#endif