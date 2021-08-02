#include "A_config.h"
#include <rom/rtc.h>

Watch_HAL hal;
static uint32_t last_millis = 0;
static lv_obj_t *lblBattery;
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
            hal.lightSleep();
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

static void task_battery_update(void *param)
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
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void setup()
{
    Serial.begin(115200);
    hal.begin();
    //电池图标
    lblBattery = lv_label_create(lv_layer_top());
    lv_label_set_text(lblBattery, LV_SYMBOL_BATTERY_EMPTY);
    lv_obj_align(lblBattery, LV_ALIGN_CENTER, 0, -104);
    pinMode(WATCH_BAT_MON, ANALOG);
    xTaskCreatePinnedToCore(task_battery_update, "BatteryIndicator", 2048, NULL, configMAX_PRIORITIES - 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(light_sleep_loop, "LightSleeper", 2048, NULL, configMAX_PRIORITIES - 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(taskHALUpdate, "HALUpdate", 3824, NULL, configMAX_PRIORITIES - 2, NULL, !CONFIG_ARDUINO_RUNNING_CORE);
    if (rtc_get_reset_reason(1) == DEEPSLEEP_RESET)
    {
        alarm_check();
    }
    hal.rtc.turnOffAlarm(2);
    hal.rtc.checkIfAlarm(1);
    alarm_update();
    Serial.println("Done");
    wf_clock_load();
    last_millis = millis();
}
