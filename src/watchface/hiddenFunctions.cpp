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
        menu_add("这就是普通手表");
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
        case 4:
            //这就是普通手表
            if(msgbox_yn("是否开放完整功能?"))
            {
                hal.conf.setValue("watchonly", "0");
                hal.conf.writeConfig();
            }
            else
            {
                hal.conf.setValue("watchonly", "1");
                hal.conf.writeConfig();
            }
            msgbox("提示", "执行成功，请按确认键重启");
            ESP.restart();
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