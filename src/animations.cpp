#include "config.h"

void animation_start(void * obj, lv_anim_exec_xcb_t func, uint16_t start, uint16_t end,uint16_t duration,lv_anim_path_cb_t path_cb = lv_anim_path_ease_out , uint16_t before = 0)
{
    lv_anim_t a;
    lv_anim_init(&a);

    /* MANDATORY SETTINGS
 *------------------*/

    /*Set the "animator" function*/
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)func);

    /*Set the "animator" function*/
    lv_anim_set_var(&a, obj);

    /*Length of the animation [ms]*/
    lv_anim_set_time(&a, duration);

    /*Set start and end values. E.g. 0, 150*/
    lv_anim_set_values(&a, start, end);

    /*Time to wait before starting the animation [ms]*/
    lv_anim_set_delay(&a, before);

    /*Set path (curve). Default is linear*/
    lv_anim_set_path_cb(&a, path_cb);

    /*true (default): apply the start vale immediately, false: apply start vale after delay when then anim. really starts. */
    lv_anim_set_early_apply(&a, false);

    /* START THE ANIMATION
 *------------------*/
    lv_anim_start(&a); /*Start the animation*/
}