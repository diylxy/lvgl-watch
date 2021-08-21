#include "A_config.h"

static lv_obj_t *curr_scr = NULL;
static lv_obj_t *scr_menu = NULL;
static lv_obj_t *obj_menu = NULL;
static int16_t selected = 0;
static int16_t total = 0;

static bool anim_ready_req = false;
static const char special_chars[] = {
    '-',
    '\\',
    '/',
    '(',
    ')',
    '*',
    '.',
    ',',
    '"',
    '_',
    '>',
    '|',
    '\'',
    ':',
    '<',
    '+',
    '=',
    '?',
    '@',
    '#',
    '$',
    '%',
    '^',
    '&',
    '{',
    '}',
    '[',
    ']',
    ';',
    '?',
    '~',
    '`',
    0};

void lv_obj_push_down(lv_obj_t *obj, uint16_t distance,
                      uint16_t time, uint16_t waitBeforeStart)
{
    lv_anim_t a;
    uint16_t p;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    p = lv_obj_get_style_y(obj, 0);
    lv_anim_set_values(&a, p - distance, p);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_time(&a, time);
    lv_anim_set_delay(&a, waitBeforeStart);
    lv_anim_start(&a);
    lv_obj_fade_in(obj, time, waitBeforeStart);
}

void lv_obj_pop_up(lv_obj_t *obj, uint16_t distance,
                   uint16_t time, uint16_t waitBeforeStart)
{
    lv_anim_t a;
    uint16_t p;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    p = lv_obj_get_style_y(obj, 0);
    lv_anim_set_values(&a, p + distance, p);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_time(&a, time);
    lv_anim_set_delay(&a, waitBeforeStart);
    lv_anim_start(&a);
    lv_obj_fade_in(obj, time, waitBeforeStart);
}

void lv_obj_fall_down(lv_obj_t *obj, uint16_t distance,
                      uint16_t time, uint16_t waitBeforeStart)
{
    lv_anim_t a;
    uint16_t p;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    p = lv_obj_get_style_y(obj, 0);
    lv_anim_set_values(&a, p, p - distance / 4);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_set_time(&a, time / 5 * 2);
    lv_anim_set_delay(&a, waitBeforeStart);
    lv_anim_start(&a);

    lv_anim_set_values(&a, p - distance / 4, p + distance);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in);
    lv_anim_set_time(&a, time / 5 * 3);
    lv_anim_set_delay(&a, waitBeforeStart + time / 5 * 2);
    lv_anim_start(&a);
    lv_obj_fade_out(obj, time, waitBeforeStart);
}

void lv_obj_floating_add(lv_obj_t *obj, uint16_t waitBeforeStart)
{
    lv_anim_t a;
    uint16_t p;

    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_time(&a, GUI_ANIM_FLOATING_TIME);
    lv_anim_set_delay(&a, waitBeforeStart);
    lv_anim_set_playback_time(&a, GUI_ANIM_FLOATING_TIME);
    lv_anim_set_playback_delay(&a, GUI_ANIM_FLOATING_TIME);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    p = lv_obj_get_style_y(obj, 0);
    lv_anim_set_values(&a, p, p + GUI_ANIM_FLOATING_RANGE);
    lv_anim_start(&a);
}

static void opa_set(lv_obj_t *a, int32_t opa)
{
    lv_obj_set_style_opa(a, opa, 0);
    /*
    for (i = 0; i < 255; ++i)
    {
        if (lv_obj_get_child(a, i))
        {
            lv_obj_set_style_opa(lv_obj_get_child(a, i), opa, 0);
        }
    }
    */
}

static void anim_ready_cb(lv_anim_t *a)
{
    anim_ready_req = false;
}

static void scroll_event_cb(lv_event_t *e)
{
    lv_area_t cont_a;
    lv_obj_get_coords(obj_menu, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = lv_obj_get_height(obj_menu) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(obj_menu);
    for (i = 0; i < child_cnt; i++)
    {
        lv_obj_t *child = lv_obj_get_child(obj_menu, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if (diff_y >= r)
        {
            x = r;
        }
        else
        {
            /*Use Pythagoras theorem to get x from radius and y*/
            lv_coord_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000); /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);
    }
}

void menu_create(void)
{
    REQUESTLV();
    scr_menu = lv_obj_create(NULL);
    obj_menu = lv_obj_create(scr_menu);
    lv_obj_set_size(obj_menu, 180, 180);
    lv_obj_set_x(obj_menu, 30);
    lv_obj_set_y(obj_menu, 30);
    lv_obj_set_style_bg_opa(obj_menu, 0, 0);
    lv_obj_set_flex_flow(obj_menu, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(obj_menu, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_radius(obj_menu, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(obj_menu, true, 0);
    lv_obj_set_scroll_dir(obj_menu, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(obj_menu, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(obj_menu, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *btn = lv_btn_create(obj_menu);
    lv_obj_set_width(btn, lv_pct(100));

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text_fmt(label, LV_SYMBOL_LEFT "  返回");
    lv_obj_set_style_text_font(label, &lv_font_chinese_16, 0);

    /*Update the buttons position manually for first*/
    lv_event_send(obj_menu, LV_EVENT_SCROLL, NULL);

    /*Be sure the fist button is in the middle*/
    total = 1;
    selected = 0;
    RELEASELV();
}

/**
 * 
 * @brief 添加菜单项
 * @param str 项目名称
 * @return 无
 */
void menu_add(const char *str)
{
    REQUESTLV();
    lv_obj_t *btn = lv_btn_create(obj_menu);
    lv_obj_set_width(btn, lv_pct(100));

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(label, &lv_font_chinese_16, 0);
    lv_label_set_text_fmt(label, str);
    lv_obj_scroll_to_view(lv_obj_get_child(obj_menu, 0), LV_ANIM_OFF);
    ++total;
    RELEASELV();
}

/**
 * @brief 显示菜单函数, 自动重新加载之前的screen
 * @return 选择的项目序号，如果为0则为[-返回-]
 */
int16_t menu_show(int16_t startAt)
{
    uint32_t last_millis_up = millis();
    uint32_t last_millis_down = millis();
    selected = startAt;
    REQUESTLV();
    curr_scr = lv_scr_act();
    lv_scr_load_anim(scr_menu, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, false);
    lv_obj_scroll_to_view(lv_obj_get_child(obj_menu, 0), LV_ANIM_OFF);
    lv_obj_add_state(lv_obj_get_child(obj_menu, selected), LV_STATE_FOCUS_KEY);
    /*
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj_menu);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)opa_set);
    lv_anim_set_time(&a, 200);
    lv_anim_start(&a);
    */
    lv_obj_scroll_to_view(lv_obj_get_child(obj_menu, selected), LV_ANIM_ON);
    RELEASELV();
    while (hal.btnEnter.isPressedRaw())
        vTaskDelay(10);
    while (1)
    {
        if (hal.btnUp.isPressedRaw())
        {
            if (millis() - last_millis_up > MENU_TIME_LONG)
            {
                last_millis_up = millis();
                menu_slide(-1);
            }
        }
        if (hal.btnDown.isPressedRaw())
        {
            if (millis() - last_millis_down > MENU_TIME_LONG)
            {
                last_millis_down = millis();
                menu_slide(1);
            }
        }
        if (hal.btnEnter.isPressedRaw())
        {
            REQUESTLV();
            lv_obj_add_state(lv_obj_get_child(obj_menu, selected), LV_STATE_PRESSED);
            RELEASELV();
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(100 / portTICK_PERIOD_MS);
            REQUESTLV();
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, scr_menu);
            lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)opa_set);
            lv_anim_set_time(&a, 200);
            lv_anim_set_ready_cb(&a, anim_ready_cb);
            lv_anim_start(&a);
            anim_ready_req = true;
            RELEASELV();
            while (anim_ready_req)
                vTaskDelay(40 / portTICK_PERIOD_MS);
            REQUESTLV();
            if (curr_scr != NULL)
            {
                lv_scr_load(curr_scr);
            }
            lv_obj_del(scr_menu);
            RELEASELV();
            vTaskDelay(10 / portTICK_PERIOD_MS);
            break;
        }
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
    return selected;
}

/**
 * @brief 菜单滑动函数
 * @param direction 方向，上划正下划负
 */
void menu_slide(int8_t direction)
{
    REQUESTLV();
    lv_obj_clear_state(lv_obj_get_child(obj_menu, selected), LV_STATE_FOCUS_KEY);
    selected += direction;
    if (selected < 0)
    {
        selected = total - 1;
    }
    if (selected >= total)
    {
        selected = 0;
    }
    lv_obj_scroll_to_view(lv_obj_get_child(obj_menu, selected), LV_ANIM_ON);
    lv_obj_add_state(lv_obj_get_child(obj_menu, selected), LV_STATE_FOCUS_KEY);
    RELEASELV();
}

/////////////////MSGBOX////////////////

/**
 * 显示一个信息提示
 * @param prompt 标题
 * @param msg 内容
 */
void msgbox(const char *prompt, const char *msg, uint32_t auto_back)
{
    uint32_t last_millis;
    REQUESTLV();
    lv_obj_t *mbox1 = lv_msgbox_create(lv_scr_act(), prompt, msg, NULL, false);
    lv_obj_set_style_text_font(mbox1, &lv_font_chinese_16, 0);
    lv_obj_center(mbox1);
    lv_obj_pop_up(mbox1);
    RELEASELV();
    last_millis = millis();
    while (1)
    {
        if (hal.btnEnter.isPressedRaw() || (auto_back && millis() - last_millis > auto_back))
        {
            REQUESTLV();
            lv_obj_fall_down(mbox1);
            RELEASELV();
            vTaskDelay(300 / portTICK_PERIOD_MS);
            REQUESTLV();
            lv_obj_del(mbox1);
            RELEASELV();
            vTaskDelay(10 / portTICK_PERIOD_MS);
            return;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

lv_obj_t *label(const char *str, uint16_t x, uint16_t y, bool animation, uint16_t anim_delay)
{
    lv_obj_t *label1;
    REQUESTLV();
    label1 = lv_label_create(lv_scr_act());
    lv_obj_set_x(label1, x);
    lv_obj_set_y(label1, y);
    lv_label_set_text(label1, str);
    lv_obj_set_style_text_font(label1, &lv_font_chinese_16, 0);
    RELEASELV();
    if (animation)
    {
        REQUESTLV();
        lv_obj_fade_in(label1, 300, anim_delay);
        RELEASELV();
    }
    return label1;
}

lv_obj_t *full_screen_msgbox_create(const char *icon, const char *title,
                                    const char *str, lv_color_t bg_color)
{
    REQUESTLV();
    lv_obj_t *msgbox_container = lv_obj_create(lv_scr_act());
    lv_obj_t *lblicon, *lbltitle, *lblstr;
    lv_obj_set_size(msgbox_container, 240, 240);
    lv_obj_set_style_bg_color(msgbox_container, bg_color, 0);
    lv_obj_set_x(msgbox_container, 0);
    lv_obj_set_y(msgbox_container, 0);
    lv_obj_set_style_border_width(msgbox_container, 0, 0);

    lblicon = lv_label_create(msgbox_container);
    lv_obj_set_style_text_font(lblicon, &icon_64px, 0);
    lv_obj_set_style_text_color(lblicon, lv_color_white(), 0);
    lv_obj_set_style_text_align(lblicon, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lblicon, icon);
    lv_obj_set_x(lblicon, 80);
    lv_obj_set_y(lblicon, 74 - 16 - 16);
    lv_obj_set_width(lblicon, 64);
    lv_obj_set_height(lblicon, 64);

    lbltitle = lv_label_create(msgbox_container);
    lv_label_set_long_mode(lbltitle, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(lbltitle, &lv_font_chinese_16, 0);
    lv_obj_set_style_text_color(lbltitle, lv_color_white(), 0);
    lv_obj_set_style_text_align(lbltitle, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lbltitle, title);
    lv_obj_set_x(lbltitle, 88 - 8);
    lv_obj_set_y(lbltitle, 20 - 8);
    lv_obj_set_width(lbltitle, 64);

    lblstr = lv_label_create(msgbox_container);
    lv_obj_set_style_text_font(lblstr, &lv_font_chinese_16, 0);
    lv_obj_set_style_text_color(lblstr, lv_color_white(), 0);
    lv_label_set_long_mode(lblstr, LV_LABEL_LONG_WRAP);
    lv_label_set_text(lblstr, str);
    lv_obj_set_x(lblstr, 39 - 16);
    lv_obj_set_y(lblstr, 144 - 16);
    lv_obj_set_width(lblstr, 163 + 16);
    lv_obj_set_height(lblstr, 64);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, lblicon);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)opa_set);
    lv_anim_set_time(&a, 500);
    lv_anim_set_delay(&a, 200);
    lv_anim_start(&a);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_values(&a, 74 - 12 - 24, 74 - 12 - 12);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_time(&a, 800);
    lv_anim_set_delay(&a, 200);
    lv_anim_start(&a);

    lv_obj_fade_in(msgbox_container, 300, 0);
    lv_obj_floating_add(lblicon, 1000);
    RELEASELV();
    return msgbox_container;
}

void full_screen_msgbox_del(lv_obj_t *mbox)
{
    lv_anim_t a;
    REQUESTLV();
    lv_anim_init(&a);
    lv_anim_set_var(&a, mbox);
    lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)opa_set);
    lv_anim_set_time(&a, 200);
    lv_anim_set_ready_cb(&a, anim_ready_cb);
    lv_anim_start(&a);
    anim_ready_req = true;
    RELEASELV();
    while (anim_ready_req)
        vTaskDelay(40 / portTICK_PERIOD_MS);
    REQUESTLV();
    lv_obj_del(mbox);
    RELEASELV();
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void full_screen_msgbox_wait_del(lv_obj_t *mbox, uint32_t auto_back)
{
    uint32_t last_millis;
    last_millis = millis();
    while (1)
    {
        if (hal.btnEnter.isPressedRaw() || (auto_back && millis() - last_millis > auto_back))
        {
            full_screen_msgbox_del(mbox);
            return;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void full_screen_msgbox(const char *icon, const char *title,
                        const char *str, lv_color_t bg_color,
                        uint32_t auto_back)
{
    full_screen_msgbox_wait_del(full_screen_msgbox_create(icon, title, str, bg_color), auto_back);
}

void countdown(void)
{
    uint8_t start_sec = hal.rtc.getSecond();
    uint8_t curr_sec = hal.rtc.getSecond() - 1;
    uint8_t curr_lbl = 0;
    while (hal.btnEnter.isPressedRaw())
        vTaskDelay(10);
    REQUESTLV();
    lv_obj_t *scr_countdown = lv_obj_create(NULL);
    lv_obj_t *cur_scr; //当前screen
    lv_obj_set_style_bg_color(scr_countdown, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);

    lv_obj_t *bar1 = lv_bar_create(scr_countdown);
    lv_obj_set_size(bar1, 200, 10);
    lv_obj_align(bar1, LV_ALIGN_CENTER, 0, 40);
    lv_bar_set_value(bar1, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(bar1, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(bar1, LV_OPA_COVER, 0);
    lv_obj_set_style_anim_time(bar1, 800, 0);
    lv_obj_fade_in(bar1, 300, 200);

    cur_scr = lv_scr_act();
    lv_scr_load(scr_countdown);

    lv_obj_align(label("倒计时", 0, 0, true, 300), LV_ALIGN_CENTER, 0, 70);

    lv_obj_t *lbl[2] = {label("", 88, 0, true, 0), label("", 88, 0, false, 0)};
    lv_obj_set_style_text_font(lbl[0], &num_64px, 0);
    lv_obj_set_style_text_font(lbl[1], &num_64px, 0);
    lv_obj_set_style_text_color(lbl[0], lv_color_white(), 0);
    lv_obj_set_style_text_color(lbl[1], lv_color_white(), 0);
    RELEASELV();
    hal.motorAdd(MOTOR_RUN, 50);
    hal.DoNotSleep = true;
    while (1)
    {
        if (hal.rtc.getSecond() != curr_sec)
        {
            curr_sec = hal.rtc.getSecond();
            if (curr_sec == 0)
            {
                hal.motorAdd(MOTOR_RUN, 100);
                break;
            }
            REQUESTLV();
            if (curr_sec > 50)
            {
                lv_obj_set_x(lbl[!curr_lbl], 104);
            }
            else
            {
                lv_obj_set_x(lbl[!curr_lbl], 88);
            }
            lv_label_set_text_fmt(lbl[!curr_lbl], "%d", 60 - curr_sec);
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, lbl[!curr_lbl]);
            lv_anim_set_values(&a, 0, 48 + 16);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_path_cb(&a, lv_anim_path_bounce);
            lv_anim_set_time(&a, 500);
            lv_anim_start(&a);
            lv_obj_fade_in(lbl[!curr_lbl], 400, 0);

            lv_anim_set_var(&a, lbl[curr_lbl]);
            lv_anim_set_values(&a, 48 + 16, 96 + 32);
            lv_anim_start(&a);
            lv_obj_fade_out(lbl[curr_lbl], 400, 0);

            curr_lbl = !curr_lbl;

            lv_bar_set_value(bar1, map(curr_sec, start_sec, 59, 0, 100), LV_ANIM_ON);
            if (curr_sec > 54)
                hal.motorAdd(MOTOR_RUN, 5);
            RELEASELV();
        }
        if (hal.btnEnter.isPressedRaw())
        {
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(20 / portTICK_PERIOD_MS);
            break;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    lv_scr_load_anim(cur_scr, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, true);
    hal.DoNotSleep = false;
}

bool msgbox_yn(const char *str)
{
    REQUESTLV();
    lv_obj_t *cur_scr = lv_scr_act();
    lv_obj_t *window = lv_obj_create(NULL);
    lv_obj_t *lbl = lv_label_create(window);
    uint8_t btnSel = 1;
    lv_obj_t *btns[2];

    lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(lbl, 180);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, -60);
    lv_obj_set_style_text_font(lbl, &lv_font_chinese_16, 0);
    lv_label_set_text(lbl, str);

    btns[0] = lv_btn_create(window);
    btns[1] = lv_btn_create(window);
    lv_obj_set_width(btns[0], 60);
    lv_obj_set_height(btns[0], 20);
    lv_obj_set_width(btns[1], 60);
    lv_obj_set_height(btns[1], 20);

    lv_obj_align(btns[0], LV_ALIGN_CENTER, -60, 20);
    lv_obj_align(btns[1], LV_ALIGN_CENTER, 60, 20);

    lv_obj_t *lbls[2];
    lbls[0] = lv_label_create(btns[0]);
    lbls[1] = lv_label_create(btns[1]);
    lv_obj_set_style_text_font(lbls[0], &lv_font_chinese_16, 0);
    lv_obj_set_style_text_font(lbls[1], &lv_font_chinese_16, 0);
    lv_label_set_text(lbls[0], "取消");
    lv_label_set_text(lbls[1], "确定");
    lv_obj_center(lbls[0]);
    lv_obj_center(lbls[1]);
    lv_obj_add_state(btns[btnSel], LV_STATE_FOCUS_KEY);
    lv_scr_load_anim(window, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
    RELEASELV();
    hal.DoNotSleep = true;
    while (1)
    {
        if (hal.btnDown.isPressedRaw() || hal.btnUp.isPressedRaw())
        {
            REQUESTLV();
            lv_obj_clear_state(btns[btnSel], LV_STATE_FOCUS_KEY);
            btnSel = !btnSel;
            lv_obj_add_state(btns[btnSel], LV_STATE_FOCUS_KEY);
            RELEASELV();
            vTaskDelay(300);
        }
        if (hal.btnEnter.isPressedRaw())
        {
            REQUESTLV();
            lv_obj_add_state(btns[btnSel], LV_STATE_PRESSED);
            RELEASELV();
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(20);
            REQUESTLV();
            lv_obj_clear_state(btns[btnSel], LV_STATE_PRESSED);
            lv_scr_load_anim(cur_scr, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, true);
            RELEASELV();
            hal.DoNotSleep = false;
            return btnSel;
        }
        vTaskDelay(50);
    }
}

uint16_t msgbox_time(const char *str, uint16_t value_pre)
{
    lv_obj_t *spinbox = NULL;
    uint16_t spinbox_val = value_pre;
    uint8_t pos = 0;
    while (hal.btnEnter.isPressedRaw())
        vTaskDelay(50);
    REQUESTLV();
    curr_scr = lv_scr_act();
    lv_obj_t *scr = lv_obj_create(lv_layer_top());
    lv_obj_set_size(scr, 130, 88);
    lv_obj_set_pos(scr, 0, 0);
    lv_obj_center(scr);
    spinbox = lv_spinbox_create(scr);
    lv_spinbox_set_range(spinbox, 0, 2359);
    lv_spinbox_set_digit_format(spinbox, 4, 2);
    lv_spinbox_set_value(spinbox, (uint16_t)(spinbox_val / 60) * 100 + spinbox_val % 60);
    lv_spinbox_set_rollover(spinbox, true);
    lv_spinbox_set_step(spinbox, 1000);
    lv_obj_set_width(spinbox, 100);
    lv_obj_set_style_text_align(spinbox, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(spinbox, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t *lbl = lv_label_create(scr);
    lv_obj_set_style_text_font(lbl, &lv_font_chinese_16, 0);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lbl, str);
    lv_obj_set_width(lbl, 100);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, -20);

    lv_obj_pop_up(scr);
    RELEASELV();
    while (1)
    {
        if (hal.btnUp.isPressedRaw())
        {
            uint16_t i = 1;
            switch (pos)
            {
            case 3:
                i = 1;
                break;
            case 2:
                i = 10;
                break;
            case 1:
                i = 60;
                break;
            case 0:
                i = 600;
                break;
            default:
                i = 1;
                break;
            }
            spinbox_val += i;
            if (spinbox_val > 23 * 60 + 59)
                spinbox_val = 0;
            REQUESTLV();
            lv_spinbox_set_value(spinbox, (uint16_t)(spinbox_val / 60) * 100 + spinbox_val % 60);
            RELEASELV();
            vTaskDelay(200);
        }
        if (hal.btnDown.isPressedRaw())
        {
            uint16_t i = 1;
            switch (pos)
            {
            case 3:
                i = 1;
                break;
            case 2:
                i = 10;
                break;
            case 1:
                i = 60;
                break;
            case 0:
                i = 600;
                break;
            default:
                i = 1;
                break;
            }
            spinbox_val -= i;
            if (spinbox_val > 23 * 60 + 59)
                spinbox_val = 23 * 60 + 59;
            REQUESTLV();
            lv_spinbox_set_value(spinbox, (uint16_t)(spinbox_val / 60) * 100 + spinbox_val % 60);
            RELEASELV();
            vTaskDelay(200);
        }
        if (hal.btnEnter.isPressedRaw())
        {
            REQUESTLV();
            lv_spinbox_step_next(spinbox);
            RELEASELV();
            ++pos;
            if (pos == 4)
                break;
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(20);
            vTaskDelay(50);
        }
        vTaskDelay(50);
    }
    REQUESTLV();
    lv_obj_fall_down(scr);
    RELEASELV();
    vTaskDelay(300);
    REQUESTLV();
    lv_obj_del(scr);
    RELEASELV();
    while (hal.btnEnter.isPressedRaw())
        vTaskDelay(50);
    return spinbox_val;
}

int msgbox_number(const char *str, uint16_t digits, uint16_t dotat,
                  int max, int min, int value_pre)
{
    lv_obj_t *spinbox = NULL;
    uint8_t pos = 0;
    while (hal.btnEnter.isPressedRaw())
        vTaskDelay(50);
    REQUESTLV();
    curr_scr = lv_scr_act();
    lv_obj_t *scr = lv_obj_create(lv_layer_top());
    lv_obj_set_size(scr, 130, 88);
    lv_obj_set_pos(scr, 0, 0);
    lv_obj_center(scr);
    spinbox = lv_spinbox_create(scr);
    lv_spinbox_set_range(spinbox, min, max);
    lv_spinbox_set_digit_format(spinbox, digits, dotat);
    lv_spinbox_set_value(spinbox, value_pre);
    lv_spinbox_set_rollover(spinbox, true);
    lv_obj_set_width(spinbox, 100);
    lv_obj_set_style_text_align(spinbox, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(spinbox, LV_ALIGN_CENTER, 0, 10);
    for (uint8_t i = 0; i < digits; ++i)
    {
        lv_spinbox_step_prev(spinbox);
    }
    lv_obj_t *lbl = lv_label_create(scr);
    lv_obj_set_style_text_font(lbl, &lv_font_chinese_16, 0);
    lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lbl, str);
    lv_obj_set_width(lbl, 100);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align(lbl, LV_ALIGN_CENTER, 0, -20);

    lv_obj_pop_up(scr);
    RELEASELV();
    while (1)
    {
        if (hal.btnUp.isPressedRaw())
        {
            REQUESTLV();
            lv_spinbox_increment(spinbox);
            RELEASELV();
            vTaskDelay(200);
        }
        if (hal.btnDown.isPressedRaw())
        {
            REQUESTLV();
            lv_spinbox_decrement(spinbox);
            RELEASELV();
            vTaskDelay(200);
        }
        if (hal.btnEnter.isPressedRaw())
        {
            REQUESTLV();
            lv_spinbox_step_next(spinbox);
            RELEASELV();
            ++pos;
            if (pos == digits)
                break;
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(20);
            vTaskDelay(50);
        }
        vTaskDelay(50);
    }
    value_pre = lv_spinbox_get_value(spinbox);
    REQUESTLV();
    lv_obj_fall_down(scr);
    RELEASELV();
    vTaskDelay(300);
    REQUESTLV();
    lv_obj_del(scr);
    RELEASELV();
    while (hal.btnEnter.isPressedRaw())
        vTaskDelay(50);
    return value_pre;
}

String msgbox_string(const char *msg, bool multiline)
{
    String res = "";
    char c;
    while (hal.btnEnter.isPressedRaw())
        vTaskDelay(50);
    REQUESTLV();
    full_screen_msgbox(BIG_SYMBOL_INFO, "输入提示", "请按上键用摩斯密码输入文字，确认键弹出菜单，下键退格");
    lv_obj_t *scr_msgbox = lv_obj_create(lv_layer_top());
    lv_obj_set_size(scr_msgbox, 200, 126);
    lv_obj_center(scr_msgbox);
    lv_obj_set_pos(scr_msgbox, 0, 0);
    lv_obj_pop_up(scr_msgbox, 20, 300, 0);
    lv_obj_set_style_pad_all(scr_msgbox, 0, 0);
    lv_obj_t *lblInfo = lv_label_create(scr_msgbox);
    lv_label_set_text(lblInfo, msg);
    lv_label_set_long_mode(lblInfo, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(lblInfo, &lv_font_chinese_16, 0);
    lv_obj_set_height(lblInfo, 22);
    lv_obj_set_width(lblInfo, 180);
    lv_obj_set_pos(lblInfo, 10, 0);
    lv_obj_set_style_text_align(lblInfo, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_pad_all(lblInfo, 0, 0);

    lv_obj_t *text = lv_textarea_create(scr_msgbox);
    lv_obj_set_size(text, 180, 100);
    lv_obj_set_pos(text, 10, 20);
    lv_obj_add_state(text, LV_STATE_FOCUSED);
    RELEASELV();
    morse.start();
    while (1)
    {
        if (morse.available())
        {
            c = morse.getChar();
            if (c == '\n' && multiline == false)
                break;
            res += c;
            lv_textarea_set_text(text, res.c_str());
        }
        if (hal.btnEnter.isPressedRaw())
        {
            morse.pause();
            uint8_t i = 0;
            REQUESTLV();
            lv_obj_fade_out(scr_msgbox, 300, 0);
            RELEASELV();
            menu_create();
            menu_add(LV_SYMBOL_EDIT " 结束输入");
            while (special_chars[i] != 0)
            {
                menu_add(String(special_chars[i]).c_str());
                ++i;
            }
            i = menu_show();
            if (i == 1)
            {
                break;
            }
            else if (i)
            {
                res += special_chars[i - 2];
                lv_textarea_set_text(text, res.c_str());
            }
            REQUESTLV();
            lv_obj_fade_in(scr_msgbox, 300, 0);
            RELEASELV();
            morse.resume();
        }
        if (hal.btnDown.isPressedRaw())
        {
            res = res.substring(0, res.length() - 1);
            lv_textarea_set_text(text, res.c_str());
            vTaskDelay(500);
        }
        vTaskDelay(50);
    }
    morse.pause();
    REQUESTLV();
    lv_obj_fall_down(scr_msgbox);
    RELEASELV();
    vTaskDelay(300);
    REQUESTLV();
    lv_obj_del(scr_msgbox);
    RELEASELV();
    while (hal.btnEnter.isPressedRaw())
        vTaskDelay(50);

    return res;
}