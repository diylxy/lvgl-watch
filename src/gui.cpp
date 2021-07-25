#include "A_config.h"

static lv_obj_t *obj_menu = NULL;
static int16_t selected = 0;
static int16_t total = 0;

static bool anim_ready_req = false;
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
    obj_menu = lv_obj_create(lv_layer_top());
    lv_obj_set_size(obj_menu, 180, 180);
    lv_obj_set_x(obj_menu, 30);
    lv_obj_set_y(obj_menu, 30);
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
}

/**
 * 
 * @brief 添加菜单项
 * @param str 项目名称
 * @return 无
 */
void menu_add(const char *str)
{

    lv_obj_t *btn = lv_btn_create(obj_menu);
    lv_obj_set_width(btn, lv_pct(100));

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(label, &lv_font_chinese_16, 0);
    lv_label_set_text_fmt(label, str);
    lv_obj_scroll_to_view(lv_obj_get_child(obj_menu, 0), LV_ANIM_OFF);
    ++total;
}

/**
 * @brief 显示菜单函数
 * @param autoback 自动重新加载之前的screen
 * @return 选择的项目序号，如果为0则为[-返回-]
 */
int16_t menu_show(int16_t startAt)
{
    uint32_t last_millis_up = millis();
    uint32_t last_millis_down = millis();
    selected = startAt;
    lv_obj_scroll_to_view(lv_obj_get_child(obj_menu, 0), LV_ANIM_OFF);
    lv_obj_add_state(lv_obj_get_child(obj_menu, selected), LV_STATE_FOCUS_KEY);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj_menu);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)opa_set);
    lv_anim_set_time(&a, 200);
    lv_anim_start(&a);
    lv_obj_scroll_to_view(lv_obj_get_child(obj_menu, selected), LV_ANIM_ON);

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
            lv_obj_add_state(lv_obj_get_child(obj_menu, selected), LV_STATE_PRESSED);
            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(100 / portTICK_PERIOD_MS);
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, obj_menu);
            lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)opa_set);
            lv_anim_set_time(&a, 200);
            lv_anim_set_ready_cb(&a, anim_ready_cb);
            lv_anim_start(&a);
            anim_ready_req = true;
            while (anim_ready_req)
                vTaskDelay(40 / portTICK_PERIOD_MS);
            DELAYLV();
            lv_obj_del(obj_menu);
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
    lv_obj_t *mbox1 = lv_msgbox_create(lv_scr_act(), prompt, msg, NULL, false);
    lv_obj_set_style_text_font(mbox1, &lv_font_chinese_16, 0);
    lv_obj_center(mbox1);
    lv_obj_fade_in(mbox1, 200, 0);
    last_millis = millis();
    while (1)
    {
        if (hal.btnEnter.isPressedRaw() || (auto_back && millis() - last_millis > auto_back))
        {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, mbox1);
            lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)opa_set);
            lv_anim_set_time(&a, 200);
            lv_anim_set_ready_cb(&a, anim_ready_cb);
            lv_anim_start(&a);
            anim_ready_req = true;
            while (anim_ready_req)
                vTaskDelay(40 / portTICK_PERIOD_MS);
            DELAYLV();
            lv_obj_del(mbox1);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            return;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

lv_obj_t *label(const char *str, uint16_t x, uint16_t y, bool animation, uint16_t anim_delay)
{
    lv_obj_t *label1;
    label1 = lv_label_create(lv_scr_act());
    lv_obj_set_x(label1, x);
    lv_obj_set_y(label1, y);
    lv_label_set_text(label1, str);
    lv_obj_set_style_text_font(label1, &lv_font_chinese_16, 0);
    if (animation)
    {
        DELAYLV();
        lv_obj_fade_in(label1, 300, anim_delay);
    }
    return label1;
}

lv_obj_t *full_screen_msgbox_create(const char *icon, const char *title, const char *str, lv_color_t bg_color)
{
    lv_obj_t *msgbox_container = lv_obj_create(lv_scr_act());
    lv_obj_t *lblicon, *lbltitle, *lblstr;
    DELAYLV();
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
    lv_obj_set_y(lblicon, 74-16-16);
    lv_obj_set_width(lblicon, 64);
    lv_obj_set_height(lblicon, 64);

    lbltitle = lv_label_create(msgbox_container);
    lv_label_set_long_mode(lbltitle, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(lbltitle, &lv_font_chinese_16, 0);
    lv_obj_set_style_text_color(lbltitle, lv_color_white(), 0);
    lv_obj_set_style_text_align(lbltitle, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lbltitle, title);
    lv_obj_set_x(lbltitle, 88-8);
    lv_obj_set_y(lbltitle, 20-8);
    lv_obj_set_width(lbltitle, 64);

    lblstr = lv_label_create(msgbox_container);
    lv_obj_set_style_text_font(lblstr, &lv_font_chinese_16, 0);
    lv_obj_set_style_text_color(lblstr, lv_color_white(), 0);
    lv_label_set_long_mode(lblstr, LV_LABEL_LONG_WRAP);
    lv_label_set_text(lblstr, str);
    lv_obj_set_x(lblstr, 39-16);
    lv_obj_set_y(lblstr, 144-16);
    lv_obj_set_width(lblstr, 163+16);
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
    lv_anim_set_values(&a, 74-12-24, 74-12-12);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_time(&a, 800);
    lv_anim_set_delay(&a, 200);
    lv_anim_start(&a);

    lv_obj_fade_in(msgbox_container, 300, 0);

    return msgbox_container;
}

void full_screen_msgbox_del(lv_obj_t *mbox)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, mbox);
    lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)opa_set);
    lv_anim_set_time(&a, 200);
    lv_anim_set_ready_cb(&a, anim_ready_cb);
    lv_anim_start(&a);
    anim_ready_req = true;
    while (anim_ready_req)
        vTaskDelay(40 / portTICK_PERIOD_MS);
    DELAYLV();
    lv_obj_del(mbox);
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
void full_screen_msgbox(const char *icon, const char *title, const char *str, lv_color_t bg_color, uint32_t auto_back)
{
    full_screen_msgbox_wait_del(full_screen_msgbox_create(icon, title, str, bg_color), auto_back);
}