/*
 *                        _oo0oo_
 *                       o8888888o
 *                       88" . "88
 *                       (| -_- |)
 *                       0\  =  /0
 *                     ___/`---'\___
 *                   .' \\|     |// '.
 *                  / \\|||  :  |||// \
 *                 / _||||| -:- |||||- \
 *                |   | \\\  - /// |   |
 *                | \_|  ''\---/''  |_/ |
 *                \  .-\__  '-'  ___/-. /
 *              ___'. .'  /--.--\  `. .'___
 *           ."" '<  `.___\_<|>_/___.' >' "".
 *          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *          \  \ `_.   \_ __\ /__ _/   .-` /  /
 *      =====`-.____`.___ \_____/___.-`___.-'=====
 *                        `=---='
 * 
 * 
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *            佛祖保佑      永不宕机      永无BUG
 */

#include "A_config.h"
#include <rom/rtc.h>
#include "EEPROM.h"
#define EEPROM_SIZE 64

Watch_HAL hal;
static uint32_t last_millis = 0;
static lv_obj_t *lblBattery;
static lv_obj_t *lblTime;
bool first = false;

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

static void light_sleep_loop(void *param)
{
    while (1)
    {
        if (hal.DoNotSleep)
        {
            hal.release_time = millis();
        }
        if (millis() - hal.release_time > hal.autoSleepTime)
        {
            if (hal.canDeepSleep && hal.canDeepSleepFromAlarm)
            {
                hal.deepSleep();
            }
            else
            {
                hal.lightSleep();
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

static void task_dock_update(void *param)
{
    uint16_t batVoltage;
    while (1)
    {
        batVoltage = analogReadMilliVolts(WATCH_BAT_MON) * 2;
        if (batVoltage == 256)
        {
            lv_label_set_text_fmt(lblBattery, LV_SYMBOL_WIFI "RSSI:%d", WiFi.RSSI());
        }
        else if (digitalRead(WATCH_STAT_PWR))
        {
            lv_label_set_text_fmt(lblBattery, LV_SYMBOL_USB "%d.%03d V", batVoltage / 1000, batVoltage % 1000);
        }
        else
        {
            lv_label_set_text_fmt(lblBattery, "%d.%03d V", batVoltage / 1000, batVoltage % 1000);
        }
        lv_label_set_text_fmt(lblTime, "%02d:%02d", hal.rtc.getHour(), hal.rtc.getMinute());
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void setup()
{
    bool hiddenFunctions = false;
    Serial.begin(115200);
    if (!SPIFFS.begin(true))
    {
        Serial.println("无法挂载SPIFFS，对于这个项目，绝对不可能执行到这里，但是写一下以防万一谁拿去二次开发了呢");
        while (1)
            delay(10);
    }
    if (!EEPROM.begin(EEPROM_SIZE))
    {
        Serial.println("初始化EEPROM失败，请检查NVS存储区是否已满");
        while (1)
            delay(10);
    }
    hal.begin();
    if (hal.btnDown.isPressedRaw() && hal.btnEnter.isPressedRaw() && !hal.btnUp.isPressedRaw())
    {
        hiddenFunctions = true;
    }
    weather.begin();
    morse.init();
    //电池图标
    lblBattery = lv_label_create(lv_layer_top());
    lv_label_set_text(lblBattery, LV_SYMBOL_BATTERY_EMPTY);
    lv_obj_align(lblBattery, LV_ALIGN_CENTER, 0, -104);
    lblTime = lv_label_create(lv_layer_top());
    lv_label_set_text(lblTime, "00:00");
    lv_obj_align(lblTime, LV_ALIGN_CENTER, 0, 104);
    pinMode(WATCH_BAT_MON, ANALOG);
    xTaskCreatePinnedToCore(task_dock_update, "Indicator", 2048, NULL, configMAX_PRIORITIES - 3, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(light_sleep_loop, "LightSleeper", 2048, NULL, configMAX_PRIORITIES - 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(taskHALUpdate, "HALUpdate", 4096, NULL, configMAX_PRIORITIES - 2, NULL, !CONFIG_ARDUINO_RUNNING_CORE);

    if (!alarm_load())
    {
        alarm_erase();
    }
    alarm_sort();
    if (EEPROM.read(0) != hal.rtc.getDate())
    {
        //这里是需要每日更新的内容，比如天气，会在当天首次DEEPSLEEP唤醒时或复位时更新
        if (strcmp(hal.conf.getValue("updated"), "1") == 0)
        {
            uint8_t b = hal.Brightness;
            hal.motorAdd(MOTOR_RUN, 50);
            hal.setBrightness(20);
            Serial.println("Connecting...");
            if (hal.connectWiFi())
            {
                weather.refresh(hal.conf.getString("city"));
                hal.NTPSync();
                hal.disconnectWiFi();
            }
            hal.setBrightness(b);
            hal.motorAdd(MOTOR_RUN, 50);
        }
        EEPROM.write(0, hal.rtc.getDate());
        EEPROM.commit();
    }

    if (rtc_get_reset_reason(0) != DEEPSLEEP_RESET && rtc_get_reset_reason(1) != DEEPSLEEP_RESET)
    {
        hal.rtc.turnOffAlarm(2);
        hal.rtc.checkIfAlarm(1);
        alarm_update();
    }
    Serial.println("Done");
    if (hiddenFunctions)
    {
        wf_hiddenfunc_load();
    }
    else
    {
        wf_clock_load();
    }
    last_millis = millis();
}
