#include "A_config.h"

static lv_obj_t *scr_hidden_func;
static void wf_hiddenfunc_loop()
{
    if (hal.btnEnter.isPressedRaw())
    {
        menu_create();
        menu_add("退出隐藏功能");
        menu_add("FlappyBird");
        menu_add("Keyboard");
        switch (menu_show(1))
        {
        case 1:
            //退出隐藏功能
            hal.fLoop = NULL;
            wf_clock_load();
            return;
            break;
        case 2:
            //FlappyBird
            pushWatchFace(wf_hiddenfunc_load);
            wf_flappy_bird_load();
            break;
        case 3:
            //Keyboard
            pushWatchFace(wf_hiddenfunc_load);
            wf_terminal_load();
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