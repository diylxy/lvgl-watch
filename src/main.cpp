#include "A_config.h"

Watch_HAL hal;
uint32_t last_millis = 0;
bool first = false;

void lv_example_spinner_1(void)
{

    label("菜单演示程序3.0", 60, 90, true, 0);
    label("更方便的Label", 60, 110, true, 300);
    label("和动画", 90, 130, true, 600);
    
}

void deepsleepCB(Button2 &btn)
{
    hal.deepSleep();
}

void taskHALUpdate(void *param)
{
    while (1)
    {
        hal.update();
        vTaskDelay(30);
    }
}
void loop1(void *param)
{
    while (1)
    {
        if (millis() - last_millis > 1000)
        {
            menu_create();
            menu_add(LV_SYMBOL_LIST "  电源菜单");
            menu_add("支持长文本~~~~~~");
            if (menu_show() == 1)
            {
            st:
                menu_create();
                menu_add(LV_SYMBOL_IMAGE "  一个信息提示");
                menu_add(LV_SYMBOL_LOOP "  重启");
                menu_add(LV_SYMBOL_SAVE "  睡眠");
                switch (menu_show(1))
                {
                case 1:
                    full_screen_msgbox(BIG_SYMBOL_INFO, "变大了？", "新的信息提示框，自动换行。。。。。。。。。。。");
                    full_screen_msgbox(BIG_SYMBOL_CROSS, "还有", "更多的图标和颜色",  FULL_SCREEN_BG_CROSS);
                    full_screen_msgbox(BIG_SYMBOL_CHECK, "还有", "更多的图标和颜色",  FULL_SCREEN_BG_CHECK);
                    full_screen_msgbox(BIG_SYMBOL_QUESTION, "还有", "更多的图标和颜色",  FULL_SCREEN_BG_QUESTION);
                    full_screen_msgbox(BIG_SYMBOL_BELL, "还有", "更多的图标和颜色",  FULL_SCREEN_BG_BELL);
                    full_screen_msgbox(BIG_SYMBOL_LOCK, "还有", "更多的图标和颜色",  FULL_SCREEN_BG_LOCK);
                    full_screen_msgbox(BIG_SYMBOL_SYNC, "还有", "同样支持设置自动返回时间（这里是1秒）",  FULL_SCREEN_BG_SYNC, 1000);
                    goto st;
                case 2:
                    msgbox(LV_SYMBOL_CLOSE " 错误", "重启不了 :(");
                    goto st;
                case 3:
                    msgbox(LV_SYMBOL_WARNING " 提示", "按确定或等待三秒<新内容>进入睡眠模式", 3000);
                    hal.deepSleep();
                }
            }
            else
            {
                hal.deepSleep();
            }
            last_millis = millis();
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void setup()
{
    Serial.begin(115200);
    hal.begin();
    lv_example_spinner_1();
    xTaskCreatePinnedToCore(loop1, "Loop1", 8192, NULL, configMAX_PRIORITIES - 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(taskHALUpdate, "HALUpdate", 2048, NULL, configMAX_PRIORITIES - 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    Serial.println("Done");
    last_millis = millis();
}
