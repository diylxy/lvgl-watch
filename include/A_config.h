#ifndef __A_CONFIG_H__
#define __A_CONFIG_H__
//按键引脚定义
#define WATCH_BUTTON_UP 13
#define WATCH_BUTTON_DOWN 10
#define WATCH_BUTTON_ENTER 0

#define WATCH_TFT_LED 9
#define WATCH_RTC_INT 32
#define WATCH_BMA_INT_1 34
#define WATCH_BMA_INT_2 35
#define WATCH_SCL 22
#define WATCH_SDA 21
#define WATCH_STAT_PWR 15
#define WATCH_BAT_MON 25

//振动电机引脚定义
#define VIBRATE_MOTOR 2


#define DISPLAY_MAX_X 240
#define DISPLAY_MAX_Y 240

#define MENU_TIME_LONG 250                      //菜单按下间隔
#include <lvgl.h>
#include "gui.h"
#include "hal.h"
#endif