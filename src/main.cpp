#include "A_config.h"

Watch_HAL hal;
uint32_t last_millis = 0;
bool first = false;

lv_obj_t *scrmain;
lv_obj_t *label2;
void lv_example_spinner_1(void)
{
  scrmain = lv_obj_create(NULL);
  label2 = lv_label_create(scrmain);
  lv_obj_set_width(label2, 240);
  lv_label_set_text(label2, "菜单演示程序2.0");
  lv_obj_set_style_text_font(label2, &lv_font_chinese_16, 0);
  lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_fade_out(label2, 500, 1200);
  lv_scr_load_anim(scrmain, LV_SCR_LOAD_ANIM_FADE_ON, 600, 10, true);
}

void deepsleepCB(Button2 &btn)
{
  hal.deepSleep();
}

void taskHALUpdate(void *param)
{
  while(1)
  {
    hal.update();
    vTaskDelay(30);
  }
}
void loop1(void *param)
{
  while (1)
  {
    if (millis() - last_millis > 2000)
    {
      menu_create();
      menu_add("还是这行中文");
      menu_add("多级菜单测试");
      menu_add("HAL基本完成");
      menu_add("不支持长文本-----");
      menu_add("Hello World!");
      menu_add(LV_SYMBOL_OK " 第二级菜单");
      menu_add("这回有输入了");
      menu_add("后面就没了");
      if(menu_show() == 6)
      {
        st:
        menu_create();
        menu_add("这是第二级菜单");
        if(menu_show(1) == 1)
        {
          msgbox(LV_SYMBOL_SETTINGS" 提示","点这儿没用");
          goto st;
        }
      }
      else
      {
        hal.deepSleep();
      }
      lv_example_spinner_1();
      last_millis = millis();
    }
    vTaskDelay(5/portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  hal.begin();
  lv_example_spinner_1();
  xTaskCreatePinnedToCore(loop1, "Loop1", 4096, NULL, configMAX_PRIORITIES - 1, NULL, CONFIG_ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(taskHALUpdate, "HALUpdate", 1024, NULL, configMAX_PRIORITIES - 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);
  Serial.println("Done");
  last_millis = millis();
}
