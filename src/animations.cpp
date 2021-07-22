#include "config.h"

void animation_start()
{
    lv_anim_t a;
    lv_anim_init(&a);

    /* MANDATORY SETTINGS
 *------------------*/

    /*Set the "animator" function*/
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);

    /*Set the "animator" function*/
    lv_anim_set_var(&a, obj);

    /*Length of the animation [ms]*/
    lv_anim_set_time(&a, duration);

    /*Set start and end values. E.g. 0, 150*/
    lv_anim_set_values(&a, start, end);

    /* OPTIONAL SETTINGS
 *------------------*/

    /*Time to wait before starting the animation [ms]*/
    lv_anim_set_delay(&a, delay);

    /*Set path (curve). Default is linear*/
    lv_anim_set_path(&a, lv_anim_path_ease_in);

    /*Set a callback to call when animation is ready.*/
    lv_anim_set_ready_cb(&a, ready_cb);

    /*Set a callback to call when animation is started (after delay).*/
    lv_anim_set_start_cb(&a, start_cb);

    /*Play the animation backward too with this duration. Default is 0 (disabled) [ms]*/
    lv_anim_set_playback_time(&a, wait_time);

    /*Delay before playback. Default is 0 (disabled) [ms]*/
    lv_anim_set_playback_delay(&a, wait_time);

    /*Number of repetitions. Default is 1.  LV_ANIM_REPEAT_INFINIT for infinite repetition*/
    lv_anim_set_repeat_count(&a, wait_time);

    /*Delay before repeat. Default is 0 (disabled) [ms]*/
    lv_anim_set_repeat_delay(&a, wait_time);

    /*true (default): apply the start vale immediately, false: apply start vale after delay when then anim. really starts. */
    lv_anim_set_early_apply(&a, true / false);

    /* START THE ANIMATION
 *------------------*/
    lv_anim_start(&a); /*Start the animation*/
}