#ifndef __MENU_H__
#define __MENU_H__
#include <Arduino.h>
#define GUI_ANIM_FLOATING_RANGE 8
#define GUI_ANIM_FLOATING_TIME 1000
/**
 * @brief 向下弹出动画
 * @param obj lvgl对象指针
 * @param distance 动画移动距离
 * @param time 动画持续时长
 * @param delay 动画开始前延时
 */
void lv_obj_push_down(lv_obj_t *obj, uint16_t distance = 24,
                      uint16_t time = 300, uint16_t waitBeforeStart = 0);

/**
 * @brief 向上弹出动画
 * @param obj lvgl对象指针
 * @param distance 动画移动距离
 * @param time 动画持续时长
 * @param delay 动画开始前延时
 */
void lv_obj_pop_up(lv_obj_t *obj, uint16_t distance = 24,
                   uint16_t time = 300, uint16_t waitBeforeStart = 0);

void lv_obj_fall_down(lv_obj_t *obj, uint16_t distance = 24,
                      uint16_t time = 300, uint16_t waitBeforeStart = 0);

/**
 * @brief 浮动动画
 * @param obj lvgl对象指针
 * @param waitBeforeStart 动画开始前延时
 */                      
void lv_obj_floating_add(lv_obj_t *obj, uint16_t waitBeforeStart = 0);

/**
 * @brief 移动到指定位置，渐慢动画
 * @param obj lvgl对象指针
 * @param x 目标位置x坐标
 * @param y 目标位置y坐标
 * @param time 动画持续时长
 * @param delay 动画开始前延时
 */
void lv_obj_move_anim(lv_obj_t *obj, int16_t x, int16_t y,
                      uint16_t time = 500, uint16_t waitBeforeStart = 0);
/**
 * @brief 创建并初始化菜单控件，清空菜单数组，但是不会立刻显示菜单。创建菜单第一步
 */
void menu_create(void);

/**
 * @brief 添加菜单项到数组
 * @param str 菜单项名称
 */
void menu_add(const char *str);

/**
 * @brief 显示菜单函数。显示菜单，并等待用户选择
 * @param startAt 可选参数，从哪一项开始，默认为0，即“返回”
 * @return 用户选择的菜单项，如果为0则为“返回”
 */
int16_t menu_show(int16_t startAt = 0);

/**
 * @brief 向上/向下滚动菜单，传入正数代表向下滚动
 * @param direction 滚动方向和距离，如传入1则为滚动到当前项的下一项
 */
void menu_slide(int8_t direction);

/**
 * @brief 简单的信息提示框
 * @param prompt 标题
 * @param msg 信息，即内容
 * @param auto_back 自动返回时间，单位ms，即如果用户不按下确定键，会在这么长时间后自动返回
 */
void msgbox(const char *prompt, const char *msg, uint32_t auto_back = 0);

/**
 * @brief 简单的在当前screen创建一个label（但是基本用不到）
 * @param str label的初始内容
 * @param x x坐标
 * @param y y坐标
 * @param animation 是否需要渐入动画，默认不需要
 * @param anim_delay 动画延时启动
 * @return 创建好的label指针
 */
lv_obj_t *label(const char *str, uint16_t x, uint16_t y,
                bool animation = false, uint16_t anim_delay = 0);

/**
 * @brief 在当前screen上创建一个全屏信息提示，注意只是创建一个并显示
 * @param icon 图标，只能从BIG_SYMBOL_*里面选一个
 * @param title 标题
 * @param str 信息，即内容
 * @param bg_color 背景颜色，可以从FULL_SCREEN_BG_*选图标对应的，也可以是别的
 * @return 创建的msgbox对象
 */
lv_obj_t *full_screen_msgbox_create(const char *icon, const char *title,
                                    const char *str,
                                    lv_color_t bg_color = lv_palette_main(LV_PALETTE_BLUE));

/**
 * @brief 删除全屏信息提示，渐出动画
 * @param mbox 之前创建的msgbox
 */
void full_screen_msgbox_del(lv_obj_t *mbox);

/**
 * @brief 等待用户按下确定键，然后删除全屏信息提示，渐出动画
 * @param mbox 之前创建的msgbox
 * @param auto_back 自动返回时间，单位ms，即如果用户不按下确定键，会在这么长时间后删除msgbox
 */
void full_screen_msgbox_wait_del(lv_obj_t *mbox, uint32_t auto_back = 0);

/**
 * @brief 在当前screen上创建一个全屏信息提示，等待用户按下确定并删除这个提示
 * @param icon 图标，只能从BIG_SYMBOL_*里面选一个
 * @param title 标题
 * @param str 信息，即内容
 * @param bg_color 背景颜色，可以从FULL_SCREEN_BG_*选图标对应的，也可以是别的
 * @param auto_back 自动返回时间，单位ms，即如果用户不按下确定键，会在这么长时间后删除msgbox
 * @return 创建的msgbox对象
 */
void full_screen_msgbox(const char *icon, const char *title,
                        const char *str,
                        lv_color_t bg_color = lv_palette_main(LV_PALETTE_BLUE),
                        uint32_t auto_back = 0);

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
int msgbox_number(const char *str, uint16_t digits,
                  uint16_t dotat, int max,
                  int min, int value_pre);
/**
 * @brief morse文字输入框，支持特殊字符
 * @param msg 提示字符串
 * @param multiline 允许多行
 * @return 输入的字符串
 */
String msgbox_string(const char *msg, bool multiline=false);


//字体定义
extern const lv_font_t lv_font_chinese_16;
extern const lv_font_t num_32px;
extern const lv_font_t num_48px;
extern const lv_font_t num_64px;
extern const lv_font_t font_weather_32;        //weather字体：有雹中晴扬大云雨特冰浓雪多冻霾雷夹重严尘无阵暴伴浮沙雾小强阴度
extern const lv_font_t font_weather_num_24;    //0123456789-/℃
extern const lv_font_t font_weather_symbol_96; //0xf764,0xf773,0xf72e,0xf185,0xf070,0xf74f,0xf741,0xf763,0xf740,0xf73f,0xf73d,0xf0c2,0xf0e7,0xf2dc,0xf7cf
//按顺序：0xf129,0xf00d, 0xf00c, 0xf128, 0xf023, 0xf0f3, 0xf021
extern const lv_font_t icon_64px;
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