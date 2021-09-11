//TODO: 秒表
#include "A_config.h"
////控件定义
static lv_obj_t * scr_stopwatch;
static lv_obj_t * lbl_time;
static lv_obj_t * lbl_start;
static lv_obj_t * lbl_stop;

////变量定义
RTC_DATA_ATTR uint32_t start_timestamp = 0;
RTC_DATA_ATTR uint8_t started = 0;
void wf_stopwatch_loop()
{

}

void wf_stopwatch_load()
{
    EASY_LOAD_WATCHFACE(scr_stopwatch);
}