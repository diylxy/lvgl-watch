#include "A_config.h"
static lv_obj_t *scr_class;
static lv_obj_t *timetable;
static uint8_t selected = 0;
static uint8_t total = 0;
static uint8_t curr_week = 1;
static bool modified = false;     //课程是否添加/修改/删除标记
static alarm_t *class_p[50];      //指向课程的指针
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
    ++total;
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
    if (curr == 0xff)
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
    for (uint8_t i = 1; i <= 7; ++i)
    {
        menu_add(week_name[i]);
    }
    uint8_t tmp = menu_show(1);
    if (tmp != 0)
    {
        if (msgbox_yn("修改星期会自动保存并刷新当前课程表，是否继续？"))
        {
            curr_week = tmp;
            week_changed = true;
        }
    }
}
/**
 *  @brief 排序、保存数据并提示
 */
static void class_save()
{
    if (!modified)
        return;
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
    uint16_t t;
    if (total == 50)
    {
        msgbox("提示", "此处显示的课程最多50个，请保存后重试");
        return NULL;
    }
    memset(&a, 0, sizeof(alarm_t));
    menu_create();
    menu_add("添加课程");
    menu_add("添加闹钟");
    menu_add("添加倒计时");
    menu_add("切换当前星期");
    menu_add("删除全部闹钟");
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
        change_week();
        return NULL;
        break;
    case 5:
        if (msgbox_yn("将初始化闹钟存储[课程|闹钟|倒计时|其他]\n此操作不可撤销!"))
        {
            uint8_t rnd = rand() % 10;
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(50);
            vTaskDelay(100);
            msgbox(LV_SYMBOL_WARNING " 警告", "请再次确认，这是最后一次机会");
            menu_create();
            for (uint8_t i = 0; i < 10; ++i)
            {
                if (i == rnd)
                    menu_add("Yes");
                else
                    menu_add("No");
            }
            uint8_t c = rand() % 11;
            if (c == rnd)
                c = 0;
            if (menu_show(c) == rnd + 1)
            {
                alarm_erase();
                alarm_save();
                full_screen_msgbox(BIG_SYMBOL_CHECK, "完成", "已清除全部闹钟信息，将于三秒后重启", FULL_SCREEN_BG_CHECK, 3000);
                ESP.restart();
                return NULL;
            }
        }
        return NULL;
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
        t = menu_show(1) - 1;
        if (t == 0xffff)
            return NULL;
        else
            strcpy(a.subtype, class_names[t]);
    }
    a.week = curr_week;
    //闹钟、课程时间设置
    if (a.type == ALARM_CLASS)
    {
        a.time_start = msgbox_time("上课时间");
        if (msgbox_yn("是否快速设置40分钟课程时长？"))
        {
            a.time_end = a.time_start + 40;
        }
        else
        {
            a.time_end = msgbox_time("下课时间", a.time_start);
        }
    }
    else
    {
        a.time_end = a.time_start = msgbox_time("开始时间");
    }
    modified = true;
    return alarm_add(a.type, a.subtype, a.week, a.time_start, a.time_end);
}
/**
 * @brief 删除课程
 * @param num 要删除的课程位于class_p中的下标，总等于selected-1
 */
static void class_delete(uint8_t num)
{
    REQUESTLV();
    lv_obj_del(lv_obj_get_child(timetable, num + 1));
    lv_obj_add_state(lv_obj_get_child(timetable, selected), LV_STATE_FOCUS_KEY);
    RELEASELV();
    memset(class_p[num], 0, sizeof(alarm_t));
    for (uint8_t i = num; i < total - 2; ++i)
    {
        class_p[i] = class_p[i + 1];
    }
    modified = true;
    --total; //显示的课程总数-1
}

/**
 *  @brief 修改课程对话框
 *  @param p 指向闹钟的指针
 */
static void modify_class_dialog(uint8_t num)
{
    uint8_t r;
    alarm_t *a = class_p[num];
    menu_create();
    menu_add("修改星期");
    menu_add("修改类型");
    menu_add("修改时间");
    menu_add("删除课程");
    menu_add("切换当前星期");
    switch (menu_show())
    {
    case 0:
        return;
    case 1:
        menu_create();
        for (uint8_t i = 0; i < CLASS_NAMES_COUNT; ++i)
        {
            menu_add(week_name[i]);
        }
        r = menu_show();
        if (r)
            a->week = r - 1;
        break;
    case 2:
        menu_create();
        menu_add("改为课程");
        menu_add("改为闹钟");
        menu_add("改为倒计时");
        switch (menu_show())
        {
        case 0:
            return;
            break;
        case 1:
            if (a->type != ALARM_CLASS)
            {
                msgbox(LV_SYMBOL_WARNING " 警告", "已默认此课程为40分钟，开始时间与原时间相同");
                a->time_end = a->time_start + 40;
                if (a->time_end > 23 * 60 + 59)
                    a->time_end = 23 * 60 + 59;
                strcpy(a->subtype, class_names[0]);
            }
            a->type = ALARM_CLASS;
            menu_create();
            for (uint8_t i = 0; i < CLASS_NAMES_COUNT; ++i)
            {
                menu_add(class_names[i]);
            }
            r = menu_show(1) - 1;
            if (r != 0xff)
                strcpy(a->subtype, class_names[r]);
            else
                return;
            break;
        case 2:
            a->type = ALARM_USER;
            a->time_end = a->time_start;
            break;
        case 3:
            a->type = ALARM_COUNTDOWN;
            a->time_end = a->time_start;
            break;
        }
        break;
    case 3:
        if (a->type == ALARM_CLASS)
        {
            a->time_start = msgbox_time("上课时间", a->time_start);
            if (msgbox_yn("是否快速设置40分钟课程时长？"))
            {
                a->time_end = a->time_start + 40;
            }
            else
            {
                a->time_end = msgbox_time("下课时间", a->time_end);
            }
        }
        else
        {
            a->time_end = a->time_start = msgbox_time("开始时间", a->time_start);
        }
        break;
    case 4:
        if (msgbox_yn("确定删除？你只有一次机会"))
        {
            class_delete(num);
        }
        return;
        break;
    case 5:
        change_week();
        return;
        break;
    default:
        return;
    }
    REQUESTLV();
    modified = true;
    lv_obj_set_style_bg_color(lv_obj_get_child(timetable, num + 1), lv_palette_main(LV_PALETTE_RED), 0);
    RELEASELV();
}
static void createTimeTable()
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_flex_flow(&style, LV_FLEX_FLOW_ROW_WRAP);
    lv_style_set_flex_main_place(&style, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_style_set_layout(&style, LV_LAYOUT_FLEX);
    lv_style_set_pad_row(&style, 4);
    lv_style_set_pad_column(&style, 4);
    lv_style_set_pad_top(&style, 24);
    lv_style_set_pad_bottom(&style, 24);

    timetable = lv_obj_create(scr_class);
    lv_obj_set_size(timetable, 240, 240);
    lv_obj_align(timetable, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(timetable, &style, 0);
}
static void class_reload()
{
    REQUESTLV();
    lv_obj_del(timetable);
    RELEASELV();
    createTimeTable();
    total = 0;
    add_button("返回");
    alarm_t *n = alarm_get_next(hal.rtc.getDoW(), hal.rtc.getHour() * 60 + hal.rtc.getMinute());
    selected = 0;
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
        class_p[selected] = c;
        char buf[40];
        if (c->type == ALARM_CLASS)
        {
            sprintf(buf, "%s\n%02d:%02d,%d", c->subtype, c->time_start / 60, c->time_start % 60, c->time_end - c->time_start);
            if (c == n)
                add_button(buf, total * 50, lv_palette_main(LV_PALETTE_BROWN));
            else
                add_button(buf, total * 50);
        }
        else if (c->type == ALARM_USER)
        {
            sprintf(buf, "闹钟\n%02d:%02d", c->time_start / 60, c->time_start % 60);
            if (c == n)
                add_button(buf, total * 50, lv_palette_main(LV_PALETTE_BROWN));
            else
                add_button(buf, total * 50, lv_palette_main(LV_PALETTE_YELLOW));
        }
        else if (c->type == ALARM_COUNTDOWN)
        {
            sprintf(buf, "倒计时\n%02d:%02d", c->time_start / 60, c->time_start % 60);
            if (c == n)
                add_button(buf, total * 50, lv_palette_main(LV_PALETTE_BROWN));
            else
                add_button(buf, total * 50, lv_palette_main(LV_PALETTE_ORANGE));
        }
        ++selected;
    }
    add_button("添加...", total * 50);
    selected = 0;
}

static void wf_class_loop(void)
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
            REQUESTLV();
            lv_obj_clear_state(lv_obj_get_child(timetable, selected), LV_STATE_FOCUS_KEY);
            lv_obj_add_state(lv_obj_get_child(timetable, selected), LV_STATE_PRESSED);
            RELEASELV();
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(50);
            class_save();
            alarm_update();
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(50);
            vTaskDelay(30);
            hal.fLoop = NULL;
            popWatchFace();
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
            class_p[total - 2] = c;
            char buf[40];
            if (c->type == ALARM_CLASS)
            {
                sprintf(buf, "%s\n%02d:%02d,%d", c->subtype, c->time_start / 60, c->time_start % 60, c->time_end - c->time_start);
            }
            else if (c->type == ALARM_USER)
            {
                sprintf(buf, "闹钟\n%02d:%02d", c->time_start / 60, c->time_start % 60);
            }
            else if (c->type == ALARM_COUNTDOWN)
            {
                sprintf(buf, "倒计时\n%02d:%02d", c->time_start / 60, c->time_start % 60);
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
    week_changed = false;
    modified = false;
    scr_class = lv_obj_create(NULL);
    lv_scr_load(scr_class);
    total = 0;
    curr_week = hal.rtc.getDoW();
    createTimeTable();
    class_reload();
    selected = 0;
    RELEASELV();
    change_selected(1);
    hal.fLoop = wf_class_loop;
}