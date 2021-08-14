#include "A_config.h"

lv_obj_t *scr_keyboard;
lv_obj_t *lbl_keyboard_status;
lv_obj_t *btnm_keyboard_main;
static const char *btnm_map[] = {"菜单", "CMD", "锁定", "桌面", "Alt+F4", "\n", 
                                "Alt+Tab","截图"/*Win+Shift+S*/, "保存", "C+A+Del", 
                                 ""};
static void wf_terminal_loop()
{
    if(hal.btnEnter.isPressedRaw())
    {

    }
    if(hal.btnUp.isPressedRaw())
    {

    }
    if(hal.btnDown.isPressedRaw())
    {

    }
    vTaskDelay(5);
}

void wf_terminal_load()
{
    EASY_LOAD_WATCHFACE(scr_keyboard);

    lbl_keyboard_status = lv_label_create(scr_keyboard);
    lv_label_set_text(lbl_keyboard_status, "未连接");
    lv_obj_set_style_text_font(lbl_keyboard_status, &lv_font_chinese_16, 0);
    lv_obj_align(lbl_keyboard_status, LV_ALIGN_CENTER, 0, -70);

    btnm_keyboard_main = lv_btnmatrix_create(scr_keyboard);
    lv_obj_set_style_text_font(btnm_keyboard_main, &lv_font_chinese_16, 0);
    lv_obj_set_size(btnm_keyboard_main, 200, 120);
    lv_obj_align(btnm_keyboard_main, LV_ALIGN_CENTER, 0, 0);
    lv_btnmatrix_set_map(btnm_keyboard_main, btnm_map);
    lv_obj_fade_in(btnm_keyboard_main, 300, 0);

    hal.fLoop = wf_terminal_loop;
    RELEASELV();
}