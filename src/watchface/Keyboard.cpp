#include "A_config.h"
#include <BleKeyboard.h>
static lv_obj_t *scr_keyboard;
static lv_obj_t *lbl_keyboard_status;
static lv_obj_t *btnm_keyboard_main;
static BleKeyboard bleKeyboard;

static const char *btnm_map[] = {"菜单", "CMD", "锁定", "桌面", "\n",
                                 "Alt+F4", "Alt+Tab", "截图" /*Win+Shift+S*/, "\n",
                                 "保存", "C+A+Del",
                                 ""};
static uint16_t btncount = 9;
static uint16_t selected = 0;
static String cmdtmp = ""; //临时存储cmd
static void wf_terminal_loop()
{
    if (bleKeyboard.isConnected())
    {
        REQUESTLV();
        lv_label_set_text(lbl_keyboard_status, "已连接");
        RELEASELV();
    }
    else
    {
        REQUESTLV();
        lv_label_set_text(lbl_keyboard_status, "未连接");
        RELEASELV();
    }
    if (hal.btnEnter.isPressedRaw())
    {
        REQUESTLV();
        lv_btnmatrix_set_btn_ctrl(btnm_keyboard_main, selected, LV_BTNMATRIX_CTRL_DISABLED);
        RELEASELV();
        hal.motorAdd(MOTOR_RUN, 5);
        while (hal.btnEnter.isPressedRaw())
        {
            vTaskDelay(20);
        }
        vTaskDelay(20);
        if (bleKeyboard.isConnected() || selected == 0)
        {
            switch (selected)
            {
            case 0:
                //menu
                menu_create();
                menu_add("退出");
                switch (menu_show())
                {
                case 1:
                    hal.DoNotSleep = false;
                    btStop();
                    popWatchFace();
                    return;
                default:
                    break;
                }
                break;
            case 1:
                //CMD
                cmdtmp = msgbox_string("请输入命令");
                do
                {
                    bleKeyboard.press(KEY_LEFT_GUI);
                    bleKeyboard.press('r');
                    vTaskDelay(100);
                    bleKeyboard.releaseAll();
                    vTaskDelay(300);
                    bleKeyboard.println(cmdtmp);
                } while (msgbox_yn("是否重复执行？"));
                break;
            case 2:
            //锁定
                    bleKeyboard.press(KEY_LEFT_GUI);
                    bleKeyboard.press('l');
                    vTaskDelay(100);
                    bleKeyboard.releaseAll();
                    break;
            case 3:
            //桌面
                    bleKeyboard.press(KEY_LEFT_GUI);
                    bleKeyboard.press('d');
                    vTaskDelay(100);
                    bleKeyboard.releaseAll();
                    break;
            case 4:
            //Alt+F4
                    bleKeyboard.press(KEY_LEFT_ALT);
                    bleKeyboard.press(KEY_F4);
                    vTaskDelay(100);
                    bleKeyboard.releaseAll();
                    break;
            case 5:
            //Alt+Tab
                    bleKeyboard.press(KEY_LEFT_ALT);
                    bleKeyboard.press(KEY_TAB);
                    vTaskDelay(100);
                    bleKeyboard.releaseAll();
                    break;
            case 6:
            //截图
                    bleKeyboard.press(KEY_LEFT_GUI);
                    bleKeyboard.press(KEY_LEFT_SHIFT);
                    bleKeyboard.press('s');
                    vTaskDelay(100);
                    bleKeyboard.releaseAll();
                    break;
            case 7:
            //保存
                    bleKeyboard.press(KEY_LEFT_CTRL);
                    bleKeyboard.press('s');
                    vTaskDelay(100);
                    bleKeyboard.releaseAll();
                    break;
            case 8:
            //Ctrl+Alt+Del
                    bleKeyboard.press(KEY_LEFT_CTRL);
                    bleKeyboard.press(KEY_LEFT_ALT);
                    bleKeyboard.press(KEY_DELETE);
                    vTaskDelay(100);
                    bleKeyboard.releaseAll();
                    break;
            default:
                break;
            }
        }
        else
        {
            msgbox("提示", "未连接");
        }
        vTaskDelay(300);
        REQUESTLV();
        lv_btnmatrix_clear_btn_ctrl(btnm_keyboard_main, selected, LV_BTNMATRIX_CTRL_DISABLED);
        RELEASELV();
        vTaskDelay(10);
    }
    if (hal.btnUp.isPressedRaw())
    {
        REQUESTLV();
        lv_btnmatrix_clear_btn_ctrl(btnm_keyboard_main, selected, LV_BTNMATRIX_CTRL_CHECKED);
        RELEASELV();
        if (selected == 0)
            selected = btncount;
        --selected;
        REQUESTLV();
        lv_btnmatrix_set_btn_ctrl(btnm_keyboard_main, selected, LV_BTNMATRIX_CTRL_CHECKED);
        RELEASELV();
        vTaskDelay(300);
    }
    if (hal.btnDown.isPressedRaw())
    {
        REQUESTLV();
        lv_btnmatrix_clear_btn_ctrl(btnm_keyboard_main, selected, LV_BTNMATRIX_CTRL_CHECKED);
        RELEASELV();
        ++selected;
        if (selected == btncount)
            selected = 0;
        REQUESTLV();
        lv_btnmatrix_set_btn_ctrl(btnm_keyboard_main, selected, LV_BTNMATRIX_CTRL_CHECKED);
        RELEASELV();
        vTaskDelay(300);
    }
    vTaskDelay(50);
}

void wf_terminal_load()
{
    uint16_t i = 0;
    btncount = 0;
    selected = 0;
    while (*btnm_map[i])
    {
        if (*btnm_map[i] != '\n')
            ++btncount;
        ++i;
    }
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
    lv_obj_set_style_pad_all(btnm_keyboard_main, 4, 0);
    lv_btnmatrix_set_btn_ctrl(btnm_keyboard_main, selected, LV_BTNMATRIX_CTRL_CHECKED);
    lv_obj_pop_up(btnm_keyboard_main);

    hal.fLoop = wf_terminal_loop;
    RELEASELV();
    hal.DoNotSleep = true;
    bleKeyboard.begin();
}