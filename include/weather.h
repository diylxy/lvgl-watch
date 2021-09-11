#ifndef __WEATHER_H__
#define __WEATHER_H__
#include <ArduinoJson.h>
#include <Arduino.h>
#include <lvgl.h>
extern const char *weather_codes[];
extern const char *weather_names[];
extern const char *weather_icons[];
//weather字体：有雹中晴扬大云雨特冰浓雪多冻霾雷夹重严尘无阵暴伴浮沙雾小强阴度
typedef struct
{
    char date[9];
    uint16_t weathernum;    //天气类型序号
    int16_t temperature;    //温度，定点1位小数
    uint16_t windspeed;     //风速，定点1位小数，单位km/h
    uint16_t winddirection; //风向，整数，指面向北方顺时针旋转的角度
} weatherInfo24H;

class Weather
{
private:
    uint8_t updateDate;
    bool started = false;
public:
    void begin();
    void save();
    int8_t refresh(String location);
    bool available(uint8_t date);
    uint16_t codeToNum(const char *code);
    weatherInfo24H *getWeather(uint8_t month, uint8_t date, uint8_t hour);
    weatherInfo24H hour24[48];
    uint16_t rain[120]; //降水量，定点2位小数，单位mm/h
    char desc1[60];     //自然语言天气描述·综合
    char desc2[60];     //自然语言天气描述·分钟级
};
extern Weather weather;

#endif