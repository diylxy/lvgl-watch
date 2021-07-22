#include "A_config.h"

static lv_obj_t *scr_menu = NULL;
static lv_obj_t * obj_menu = NULL;
static lv_obj_t *current_scr = NULL;
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

/**
 * Translate the object as they scroll
 */
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
    lv_label_set_text_fmt(label, "[Back]");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);

    /*Update the buttons position manually for first*/
    lv_event_send(obj_menu, LV_EVENT_SCROLL, NULL);

    /*Be sure the fist button is in the middle*/
    lv_obj_scroll_to_view(lv_obj_get_child(obj_menu, 0), LV_ANIM_OFF);
}

void menu_add(char *str)
{
    lv_obj_t *btn = lv_btn_create(obj_menu);
    lv_obj_set_width(btn, lv_pct(100));

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text_fmt(label, str);
    lv_obj_set_style_text_font(label, &lv_font_chinese_16, 0);
}