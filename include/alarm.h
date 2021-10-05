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

#define ALARM_NONE 0
#define ALARM_CLASS 1
#define ALARM_USER 2
#define ALARM_COUNTDOWN 3
typedef uint16_t alarm_type;
//注意：内存对齐，uint8_t占用两个字节
typedef struct
{
    alarm_type type;
    char subtype[16]; //子类型，alarm_class专用
    uint16_t week;
    uint16_t time_start;
    uint16_t time_end; //结束时间，alarm_class专用
    uint16_t q;        //排序专用
} alarm_t;

typedef struct
{
    uint8_t alarm_count;
    alarm_t alarm[200];
} alarm_file_t;
void alarm_erase();
void alarm_sort();
alarm_t *alarm_add(alarm_type type,const char *subtype, uint8_t week, uint16_t time_start, uint16_t time_end);
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
#define CLASS_NAMES_COUNT 15
extern alarm_t *current_alarm;
#endif