#include "A_config.h"
#define WEATHER_SAND_STORM "\xEF\x9D\xA4"
#define WEATHER_WATER "\xEF\x9D\xB3"
#define WEATHER_WIND "\xEF\x9C\xAE"
#define WEATHER_SUN "\xEF\x86\x85"
#define WEATHER_EYE_SLASH "\xEF\x81\xB0"
#define WEATHER_FLOOD "\xEF\x9D\x8F"
#define WEATHER_RAIN_SNOW "\xEF\x9D\x81"
#define WEATHER_SUN_CLOUD "\xEF\x9D\xA3"
#define WEATHER_SHOWER_HEAVY "\xEF\x9D\x80"
#define WEATHER_SHOWER "\xEF\x9C\xBF"
#define WEATHER_RAIN "\xEF\x9C\xBD"
#define WEATHER_CLOUD "\xEF\x83\x82"
#define WEATHER_LIGHTNING "\xEF\x83\xA7"
#define WEATHER_SNOW "\xEF\x8B\x9C"
#define WEATHER_SNOW_HEAVY "\xEF\x9F\x8F"

#define WEATHER_TYPE_COUNT 20
const char *weather_codes[WEATHER_TYPE_COUNT] = 
{
    "CLEAR_DAY",
    "CLEAR_NIGHT",
    "PARTLY_CLOUDY_DAY",
    "PARTLY_CLOUDY_NIGHT",
    "CLOUDY",
    "LIGHT_HAZE",
    "MODERATE_HAZE",
    "HEAVY_HAZE",
    "LIGHT_RAIN",
    "MODERATE_RAIN",
    "HEAVY_RAIN",
    "STORM_RAIN",
    "FOG",
    "LIGHT_SNOW",
    "MODERATE_SNOW",
    "HEAVY_SNOW",
    "STORM_SNOW",
    "DUST",
    "SAND",
    "WIND",
};
const char *weather_names[WEATHER_TYPE_COUNT] = {
    "晴",
    "晴",
    "多云",
    "多云",
    "阴",
    "轻霾",
    "中霾",
    "重霾",
    "小雨",
    "中雨",
    "大雨",
    "暴雨",
    "雾",
    "小雪",
    "中雪",
    "大雪",
    "暴雪",
    "浮尘",
    "沙尘",
    "大风"
};
const char *weather_icons[WEATHER_TYPE_COUNT] = {
    WEATHER_SUN,
    WEATHER_SUN,
    WEATHER_SUN_CLOUD,
    WEATHER_SUN_CLOUD,
    WEATHER_CLOUD,
    WEATHER_WATER,
    WEATHER_WATER,
    WEATHER_EYE_SLASH,
    WEATHER_RAIN,
    WEATHER_SHOWER,
    WEATHER_SHOWER_HEAVY,
    WEATHER_FLOOD,
    WEATHER_WATER,
    WEATHER_SNOW,
    WEATHER_SNOW,
    WEATHER_SNOW_HEAVY,
    WEATHER_SNOW_HEAVY,
    WEATHER_SAND_STORM,
    WEATHER_SAND_STORM,
    WEATHER_WIND
};

uint16_t Weather::codeToNum(const char *code)
{
    for(uint8_t i = 0; i < WEATHER_TYPE_COUNT; ++i)
    {
        if(strcmp(weather_codes[i], code) == 0)
        {
            return i;
        }
    }
    return 0;
}