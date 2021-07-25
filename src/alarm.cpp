#include "A_config.h"

enum alarm_type
{
    ALARM_NONE = 0,
    ALARM_CLASS,
    ALARM_USER,
    ALARM_COUNTDOWN
};
typedef struct
{
    enum alarm_type type;
    uint8_t subtype; //子类型，alarm_class专用
    bool enable;
    uint8_t week;
    uint16_t time_start;
    uint16_t time_end; //结束时间，alarm_class专用
    uint16_t q;        //排序专用
} alarm_t;

typedef struct
{
    uint8_t alarm_count;
    alarm_t alarm[200];
} alarm_file_t;

alarm_file_t alarms;

void alarm_sort()
{
    int i, j;
    alarm_t alarm;
    uint16_t q1;
    alarms.alarm_count = 0;
    for (i = 0; i < 200; ++i)
    {
        if (alarms.alarm[i].type != ALARM_NONE)
        {
            ++alarms.alarm_count;
            alarms.alarm[i].q = (int)alarms.alarm[i].week * 24 * 60 + (int)alarms.alarm[i].time_start;
        }
        else
        {
            alarms.alarm[i].q = 0;
        }
    }
    for (i = 1; i < 200; i++)
    {
        alarm = alarms.alarm[i];
        q1 = alarms.alarm[i].q;
        for (j = i - 1; j >= 0 && alarms.alarm[j].q < q1; j--)
        {
            alarms.alarm[j + 1] = alarms.alarm[j];
            alarms.alarm[j + 1].q = alarms.alarm[j].q;
        }
        alarms.alarm[j + 1] = alarm;
        alarms.alarm[j + 1].q = q1;
    }
}

alarm_t *alarm_add(enum alarm_type type, uint8_t subtype, uint8_t week, uint16_t time_start, uint16_t time_end, bool enable)
{
    alarms.alarm[alarms.alarm_count].enable = enable;
    alarms.alarm[alarms.alarm_count].type = type;
    alarms.alarm[alarms.alarm_count].subtype = subtype;
    alarms.alarm[alarms.alarm_count].week = week;
    alarms.alarm[alarms.alarm_count].time_start = time_start;
    alarms.alarm[alarms.alarm_count].time_end = time_end;
    alarms.alarm_count++;
    return &alarms.alarm[alarms.alarm_count - 1];
}

void alarm_del(uint8_t week, uint16_t time_start, uint16_t time_end)
{
    if (alarms.alarm_count == 0)
    {
        return;
    }
    for (uint8_t i = 0; i < alarms.alarm_count; ++i)
    {
        if (alarms.alarm[i].type != ALARM_NONE)
        {
            if (alarms.alarm[i].week == week && alarms.alarm[i].time_start == time_start && alarms.alarm[i].time_end == time_end)
            {
                alarms.alarm[i].type = ALARM_NONE;
                alarm_sort();
                return;
            }
        }
    }
}

alarm_t *alarm_get_next(uint8_t week, uint16_t now)
{
    uint16_t q1 = week * 24*60 + now;
    alarm_t *nearest = NULL;
    for(uint8_t i = 0; i < alarms.alarm_count; ++i)
    {
        if(alarms.alarm[i].q < nearest->q && alarms.alarm[i].q > q1 && alarms.alarm[i].type != ALARM_NONE)
        {
            nearest = &alarms.alarm[i];
        }
    }
    return nearest;
}


alarm_t *alarm_get_today(uint8_t week, uint8_t num)
{
    ++num;
    for(uint8_t i = 0; i < alarms.alarm_count; ++i)
    {
        if(alarms.alarm[i].week == week)
        {
            --num;
            if(num == 0)
            {
                return &alarms.alarm[i];
            }
        }
    }
    return NULL;
}

