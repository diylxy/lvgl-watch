/*
 * @Author: your name
 * @Date: 2021-07-24 21:21:00
 * @LastEditTime: 2021-08-07 22:13:01
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \open-smartwatch-lvgl\include\alarm.h
 */
#ifndef __ALARM_H__
#define __ALARM_H__


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
void alarm_format();
void alarm_sort();
alarm_t *alarm_add(enum alarm_type type, uint8_t subtype, uint8_t week, uint16_t time_start, uint16_t time_end);
void alarm_del(uint8_t week, uint16_t time_start, uint16_t time_end);
alarm_t *alarm_get_next(uint8_t week, uint16_t now);
alarm_t *class_get_next(uint8_t week, uint16_t now);
alarm_t * class_get_curr(uint8_t week, uint16_t now);
alarm_t *class_get_next_no_curr(uint8_t week, uint16_t now);
alarm_t *alarm_get_today(uint8_t week, uint8_t num);
void alarm_update();
void alarm_check();
bool alarm_save();
bool alarm_load();
extern const char *class_names[];
extern const char *week_name[];
#define CLASS_NAMES_COUNT 11
extern alarm_t *current_alarm;
#endif