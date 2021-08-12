#ifndef __MENU_H__
#define __MENU_H__
#include <Arduino.h>

void menu_create(void);
void menu_add(const char *str);
int16_t menu_show(int16_t startAt = 0);
void menu_slide(int8_t direction);
void menu_add(const char *str);
void menu_create(void);
void msgbox(const char *prompt, const char *msg, uint32_t auto_back = 0);
lv_obj_t *label(const char *str, uint16_t x, uint16_t y, bool animation = false, uint16_t anim_delay = 0);
lv_obj_t *full_screen_msgbox_create(const char *icon, const char *title, const char *str, lv_color_t bg_color = lv_palette_main(LV_PALETTE_BLUE));
void full_screen_msgbox_del(lv_obj_t *mbox);
void full_screen_msgbox_wait_del(lv_obj_t *mbox, uint32_t auto_back = 0);
void full_screen_msgbox(const char *icon, const char *title, const char *str, lv_color_t bg_color = lv_palette_main(LV_PALETTE_BLUE), uint32_t auto_back = 0);

/**
 * @brief 
 * 倒计时到下一分钟0秒
 */
void countdown(void);

/**
 * @brief 信息提示-选择
 * @return true：确认，false：取消
 */
bool msgbox_yn(const char *str);

/**
 * 输入框——时间
 * @param str 提示字符串
 * @param value_pre 预设值，单位分钟，默认为0
 * @return 时间，单位分钟
 */
uint16_t msgbox_time(const char *str, uint16_t value_pre = 0);

/**
 * @brief 数字输入框
 * @param str 提示字符串
 * @param digits 数字位数，如100为3位，99为2位，099为3位
 * @param dotat 小数点位置，从右向左数，如123.45小数点位置为2，1234小数点位置为0
 * @param max 最大输入数，超过后显示会自动变为0
 * @param min 最小输入数，低于此数显示会自动变为最大输入数
 * @param value_pre 预设值
 * @return 一个有符号整数，代表输入的值，定点数，如输入12.345，返回12345
 */
int msgbox_number(const char *str, uint16_t digits, uint16_t dotat, int max, int min, int value_pre);

//字体定义
extern const lv_font_t lv_font_chinese_16;
extern const lv_font_t icon_64px;
extern const lv_font_t num_32px;
extern const lv_font_t num_48px;
extern const lv_font_t num_64px;
extern const lv_font_t font_weather_32;//weather字体：有雹中晴扬大云雨特冰浓雪多冻霾雷夹重严尘无阵暴伴浮沙雾小强阴度
extern const lv_font_t font_weather_num_24;//0123456789-/℃
//按顺序：0xf129,0xf00d, 0xf00c, 0xf128, 0xf023, 0xf0f3, 0xf021
#define BIG_SYMBOL_INFO "\xEF\x84\xA9"
#define BIG_SYMBOL_CROSS "\xEF\x80\x8D"
#define BIG_SYMBOL_CHECK "\xEF\x80\x8C"
#define BIG_SYMBOL_QUESTION "\xEF\x84\xA8"
#define BIG_SYMBOL_LOCK "\xEF\x80\xA3"
#define BIG_SYMBOL_BELL "\xEF\x83\xB3"
#define BIG_SYMBOL_SYNC "\xEF\x80\xA1"


#define FULL_SCREEN_BG_INFO lv_palette_main(LV_PALETTE_BLUE)
#define FULL_SCREEN_BG_BELL lv_palette_main(LV_PALETTE_YELLOW)
#define FULL_SCREEN_BG_LOCK lv_palette_main(LV_PALETTE_ORANGE)
#define FULL_SCREEN_BG_CROSS lv_palette_main(LV_PALETTE_RED)
#define FULL_SCREEN_BG_CHECK lv_palette_main(LV_PALETTE_GREEN)
#define FULL_SCREEN_BG_QUESTION lv_palette_main(LV_PALETTE_LIGHT_BLUE)
#define FULL_SCREEN_BG_SYNC lv_palette_main(LV_PALETTE_BROWN)
#endif