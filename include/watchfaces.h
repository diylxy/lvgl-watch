#ifndef __WATCH_FACE_H__
#define __WATCH_FACE_H__

typedef void (* wfSetupFunc)(void);//表盘加载函数类型，加载函数，例如wf_*_load.

/**
 * @brief 表盘加载函数入栈，规定：    
 *        在wf_*_loop函数中如果要求加载新表盘，则 **在其之前** 调用此函数将当前表盘的加载函数入栈
 * @param setup 当前表盘的加载函数指针
 */
void pushWatchFace(wfSetupFunc setup);

/**
 * @brief 表盘加载函数出栈并执行，规定：  
 * 在wf_*_loop函数中如果要求退出当前表盘，则调用此函数将上一个表盘的加载函数出栈
 * 此函数执行完后新表盘scr已被加载，所以不要进行lvgl操作，尽快返回
 */
void popWatchFace();

void wf_clock_load(void);
void wf_class_load(void);
void wf_webserver_load(void);
void wf_sysinfo_load();
void wf_weather_load();
#endif