#include "A_config.h"

static lv_obj_t *scr_class;
static lv_obj_t *timetable;
static uint8_t selected = 0;
static uint8_t total = 0;
static uint8_t curr_week = 0;
static alarm_t *class_p[50]; //指向课程的指针
static volatile bool exit_f = false;
static bool week_changed = false; //星期已修改标志，需要在主循环中判断并刷新课程表
static void add_button(const char *str, uint16_t sleep = 0, lv_color_t color = lv_palette_main(LV_PALETTE_BLUE))
{
    REQUESTLV();
    lv_obj_t *obj = lv_btn_create(timetable);
    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(obj, color, 0);
    lv_obj_t *label = lv_label_create(obj);
    lv_label_set_text(label, str);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_font(label, &lv_font_chinese_16, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(label);
    lv_obj_fade_in(obj, 300, sleep);
    RELEASELV();
}

static void insert_button(const char *str)
{
    REQUESTLV();
    lv_obj_del(lv_obj_get_child(timetable, total - 1));
    RELEASELV();
    --total;
    add_button(str, 0, lv_palette_main(LV_PALETTE_RED));
    add_button("添加...", 200);
}
/**
 * @brief 修改当前选择的菜单项
 * @param curr 要选择的项目
 */
static void change_selected(uint8_t curr)
{
    REQUESTLV();
    if (curr == -1)
        curr = total - 1;
    else if (curr >= total)
        curr = 0;
    lv_obj_clear_state(lv_obj_get_child(timetable, selected), LV_STATE_FOCUS_KEY);
    lv_obj_add_state(lv_obj_get_child(timetable, curr), LV_STATE_FOCUS_KEY);
    lv_obj_scroll_to_view(lv_obj_get_child(timetable, curr), LV_ANIM_ON);
    selected = curr;
    RELEASELV();
}
/**
 * @brief 选择星期菜单
 */
static void change_week()
{
    menu_create();
    for (uint8_t i = 0; i < CLASS_NAMES_COUNT; ++i)
    {
        menu_add(week_name[i]);
    }
    uint8_t tmp = menu_show(1) - 1;
    if (tmp != 0xff)
    {
        if (msgbox_yn("修改星期会自动保存并刷新当前课程表，是否继续？"))
            curr_week = tmp;
    }
}
/**
 *  @brief 排序、保存数据并提示
 */
static void class_save()
{
    alarm_sort();
    alarm_save();
    full_screen_msgbox(BIG_SYMBOL_CHECK, "课程管理", "保存成功", FULL_SCREEN_BG_CHECK, 2000);
}
/**
 *  @brief 添加课程对话框
 *  @return 指向添加的课程的指针
 */
static alarm_t *add_class_dialog()
{
    alarm_t a;
    if (total == 50)
    {
        msgbox("提示", "此处显示的课程最多50个，请保存后重试");
        return NULL;
    }
start:
    menu_create();
    menu_add("添加课程");
    menu_add("添加闹钟");
    menu_add("添加倒计时");
    menu_add("切换星期");
    switch (menu_show(1))
    {
    case 0:
        return NULL;
        break;
    case 1:
        a.type = ALARM_CLASS;
        break;
    case 2:
        a.type = ALARM_USER;
        break;
    case 3:
        a.type = ALARM_COUNTDOWN;
        break;
    case 4:
    {
        change_week();
        return NULL;
        break;
    }
    default:
        return NULL;
    }
    if (a.type == ALARM_CLASS)
    {
        menu_create();
        for (uint8_t i = 0; i < CLASS_NAMES_COUNT; ++i)
        {
            menu_add(class_names[i]);
        }
        a.subtype = menu_show(1) - 1;
        if (a.subtype == 0xff)
            goto start;
    }
    a.week = curr_week;
}
/**
 *  @brief 修改课程对话框
 *  @param p 指向闹钟的指针
 */
static void modify_class_dialog(uint8_t num)
{
}

static void class_reload()
{
    uint8_t i = 0;
    REQUESTLV();
    lv_obj_t *obj = lv_obj_get_child(timetable, i);
    while (obj)
    {
        lv_obj_del(obj);
        obj = lv_obj_get_child(timetable, ++i);
    }
    total = 0;
    add_button("返回");
    alarm_t *n = alarm_get_next(hal.rtc.getDoW(), hal.rtc.getHour() * 60 + hal.rtc.getMinute());
    while (1)
    {
        alarm_t *c = alarm_get_today(curr_week, selected);
        if (c == NULL)
            break;
        if (total == 50)
        {
            msgbox("提示", "此处显示的课程最多50个，部分课程无法显示", 2000);
            break;
        }
        class_p[selected - 1] = c;
        char buf[40];
        if (c->type == ALARM_CLASS)
        {
            sprintf(buf, "%s\n%d:%d", class_names[c->subtype], c->time_start / 60, c->time_start % 60);
            if (c == n)
                add_button(buf, total * 50, lv_palette_main(LV_PALETTE_BROWN));
            else
                add_button(buf, total * 50);
        }
        else if (c->type == ALARM_USER)
        {
            sprintf(buf, "闹钟\n%d:%d", c->time_start / 60, c->time_start % 60);
            if (c == n)
                add_button(buf, total * 50, lv_palette_main(LV_PALETTE_BROWN));
            add_button(buf, total * 50, lv_palette_main(LV_PALETTE_YELLOW));
        }
        else if (c->type == ALARM_COUNTDOWN)
        {
            sprintf(buf, "倒计时\n%d:%d", c->time_start / 60, c->time_start % 60);
            if (c == n)
                add_button(buf, total * 50, lv_palette_main(LV_PALETTE_BROWN));
            add_button(buf, total * 50, lv_palette_main(LV_PALETTE_ORANGE));
        }
        else
        {
            sprintf(buf, "系统\n%d:%d", c->time_start / 60, c->time_start % 60);
            add_button(buf, total * 50, lv_palette_main(LV_PALETTE_GREY));
        }
        ++selected;
    }
    add_button("添加...", total * 50);
    selected = 0;
    RELEASELV();
}

static void wf_class_loop()
{
    if (week_changed)
    {
        week_changed = false;
        class_save();
        class_reload();
    }
    if (hal.btnDown.isPressedRaw())
    {
        change_selected(selected + 1);
        vTaskDelay(300);
    }
    if (hal.btnUp.isPressedRaw())
    {
        change_selected(selected - 1);
        vTaskDelay(300);
    }
    if (hal.btnEnter.isPressedRaw())
    {
        if (selected == 0)
        {
            //TODO: 退出watchface
            hal.fLoop = NULL;
            class_save();
            exit_f = true;
            return;
        }
        else if (selected == total - 1)
        {
            if (total == 50)
            {
                msgbox("提示", "此处显示的课程最多50个，请保存后重试");
                return;
            }
            alarm_t *c = add_class_dialog();
            if (c == NULL)
                return;
            class_p[selected - 1] = c;
            char buf[40];
            if (c->type == ALARM_CLASS)
            {
                sprintf(buf, "%s\n%d:%d", class_names[c->subtype], c->time_start / 60, c->time_start % 60);
            }
            else if (c->type == ALARM_USER)
            {
                sprintf(buf, "闹钟\n%d:%d", c->time_start / 60, c->time_start % 60);
            }
            else if (c->type == ALARM_COUNTDOWN)
            {
                sprintf(buf, "倒计时\n%d:%d", c->time_start / 60, c->time_start % 60);
            }
            else
            {
                sprintf(buf, "系统\n%d:%d", c->time_start / 60, c->time_start % 60);
            }
            insert_button(buf);
        }
        else
        {
            modify_class_dialog(selected - 1);
        }

        vTaskDelay(300);
    }
    vTaskDelay(50);
}

void wf_class_load(void)
{
    REQUESTLV();
    if (lv_scr_act())
        lv_obj_del(lv_scr_act());

    scr_class = lv_obj_create(NULL);
    lv_scr_load(scr_class);
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_flex_flow(&style, LV_FLEX_FLOW_ROW_WRAP);
    lv_style_set_flex_main_place(&style, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_style_set_layout(&style, LV_LAYOUT_FLEX);

    lv_obj_t *timetable = lv_obj_create(scr_class);
    lv_obj_set_size(timetable, 240, 240);
    lv_obj_align(timetable, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(timetable, &style, 0);
    total = 0;
    curr_week = hal.rtc.getDoW();
    class_reload();
    RELEASELV();
    hal.fLoop = wf_class_loop;
    exit_f = false;
    while (exit_f == false)
        vTaskDelay(100);
}