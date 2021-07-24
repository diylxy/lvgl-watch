#include "A_config.h"

static lv_obj_t *scr_menu = NULL;
static lv_obj_t *obj_menu = NULL;
static int16_t selected = 0;
static int16_t total = 0;

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

        /*Use some opacity with larger translations*/
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

void menu_create(void)
{

    scr_menu = lv_obj_create(NULL);
    obj_menu = lv_obj_create(scr_menu);
    lv_obj_set_size(obj_menu, 180, 180);
    lv_obj_center(obj_menu);
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
    lv_label_set_text_fmt(label, LV_SYMBOL_LEFT "  [-返回-]");
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

    lv_obj_scroll_to_view(lv_obj_get_child(obj_menu, selected), LV_ANIM_OFF);
    lv_obj_add_state(lv_obj_get_child(obj_menu, selected), LV_STATE_FOCUS_KEY);
    lv_scr_load_anim(scr_menu, LV_SCR_LOAD_ANIM_FADE_ON, 350, 0, true);

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

            //lv_obj_clear_state(lv_obj_get_child(obj_menu, selected), LV_STATE_FOCUS_KEY);
            lv_obj_add_state(lv_obj_get_child(obj_menu, selected), LV_STATE_PRESSED);

            while (hal.btnEnter.isPressedRaw())
                vTaskDelay(100 / portTICK_PERIOD_MS);
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

/**
 * 显示一个信息提示
 * @param prompt 标题
 * @param msg 内容
 */
void msgbox(const char *prompt, const char *msg)
{
    lv_obj_t *mbox1 = lv_msgbox_create(lv_scr_act(), prompt, msg, NULL, false);
    lv_obj_set_style_text_font(mbox1, &lv_font_chinese_16, 0);
    lv_obj_center(mbox1);
    lv_obj_fade_in(mbox1, 300, 0);
    while (1)
    {
        if (hal.btnEnter.isPressedRaw())
        {
            TODO：动画完成后删除
            lv_obj_fade_out(mbox1, 400,0);
            while(hal.btnEnter.isPressedRaw())vTaskDelay(50 / portTICK_PERIOD_MS);
            return;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}