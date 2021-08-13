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
const char *Weather::weatherNumtoImage(uint16_t num)
{
    switch (num)
    {
    case 0:
        return WEATHER_SUN;
    case 1:
        return WEATHER_SUN_CLOUD;
    case 2:
        return WEATHER_CLOUD;
    case 3:
        return WEATHER_RAIN;
    case 4:
        return WEATHER_LIGHTNING;
    case 5:
        return WEATHER_LIGHTNING;
    case 6:
        return WEATHER_RAIN_SNOW;
    case 7:
        return WEATHER_RAIN;
    case 8:
        return WEATHER_RAIN;
    case 9:
        return WEATHER_SHOWER;
    case 10:
    case 11:
        return WEATHER_SHOWER_HEAVY;
        return WEATHER_SHOWER_HEAVY;
    case 12:
        return WEATHER_FLOOD;
    case 13:
    case 14:
    case 15:
        return WEATHER_SNOW;
    case 16:
    case 17:
        return WEATHER_SNOW_HEAVY;
    case 18:
        return WEATHER_WIND;
    case 19:
        return WEATHER_RAIN_SNOW;
    case 20:
        return WEATHER_SAND_STORM;
    case 21:
    case 22:
        return WEATHER_RAIN;
    case 23:
        return WEATHER_SHOWER;
    case 24:
        return WEATHER_SHOWER_HEAVY;
    case 25:
        return WEATHER_FLOOD;
    case 26:
    case 27:
        return WEATHER_SNOW;
    case 28:
        return WEATHER_SNOW_HEAVY;
    case 29:
    case 30:
        return WEATHER_SAND_STORM;
    case 31:
    case 32:
    case 49:
        return WEATHER_EYE_SLASH;
    case 53:
    case 54:
        return WEATHER_WATER; //应该是雾，但是没找到图标
    case 55:
    case 56:
        return WEATHER_EYE_SLASH;
    case 57:
        return WEATHER_WATER;
    case 58:
        return WEATHER_EYE_SLASH;
    case 99:
        return "";
    case 301:
        return WEATHER_RAIN;
    case 302:
        return WEATHER_SNOW;
    default:
        return "";
    }
}