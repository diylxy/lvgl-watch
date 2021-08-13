#include "A_config.h"

static lv_obj_t *scr_hidden_func;
static void wf_hiddenfunc_loop()
{
    if (hal.btnEnter.isPressedRaw())
    {
        menu_create();
        menu_add("FlappyBird");
        menu_add("退出隐藏功能");
        switch (menu_show(1))
        {
        case 1:
            //FlappyBird
            pushWatchFace(wf_hiddenfunc_load);
            wf_flappy_bird_load();
            break;
        case 2:
            //退出隐藏功能
            hal.fLoop = NULL;
            wf_clock_load();
            return;
            break;
        default:
            break;
        }
    }
    vTaskDelay(100);
}

void wf_hiddenfunc_load()
{
    EASY_LOAD_WATCHFACE(scr_hidden_func);
    lv_obj_center(label("隐藏菜单", 60, 112, true, 0));
    RELEASELV();
    hal.fLoop = wf_hiddenfunc_loop;
}