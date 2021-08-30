#include "A_config.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "FreeRTOS.h"
#include "configParams.h"
static Arduino_DataBus *bus = new Arduino_ESP32SPI_DMA(12, 5, 18, 23, -1, VSPI);
static Arduino_TFT *gfx = new Arduino_GC9A01(bus, 33, 0 /* rotation */, true /* IPS */);

static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf[DISPLAY_MAX_X * 40];
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static lv_disp_t *disp;
volatile bool lv_processing = false;
volatile bool lv_halt = false;
static void encoder_with_keys_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    data->state = LV_INDEV_STATE_RELEASED;
}

static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    while (hal.screenMutex)
        ;
    hal.screenMutex = true;
    gfx->startWrite();
    gfx->writeAddrWindow(area->x1, area->y1, w, h);
    gfx->writePixels(&color_p->full, w * h);
    gfx->endWrite();
    hal.screenMutex = false;
    lv_disp_flush_ready(disp);
}

void loop(void)
{
    while (lv_halt)
        ;
    lv_processing = true;
    lv_timer_handler();
    lv_processing = false;
    vTaskDelay(2);
}
/**
 * @brief 表盘更新
 */
static void update_loop(void *param)
{
    while (1)
    {
        if (hal.fLoop)
        {
            hal.fLoop();
        }
        vTaskDelay(1);
    }
}

static void lvgl_begin()
{
    lv_init();

    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISPLAY_MAX_X * 40);

    /*Initialize the display*/
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &disp_buf;
    disp = lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_with_keys_read;
    lv_indev_drv_register(&indev_drv);
}

void Watch_HAL::begin()
{
    rtc_gpio_deinit(GPIO_NUM_0);
    rtc_gpio_deinit(GPIO_NUM_32);
    pinMode(WATCH_TFT_LED, OUTPUT);
    digitalWrite(WATCH_TFT_LED, 0);
    pinMode(WATCH_RTC_INT, INPUT_PULLUP);
    conf.setDescription(configParams);
    conf.readConfig();

    btnUp.begin(WATCH_BUTTON_UP, INPUT, false, false);
    btnDown.begin(WATCH_BUTTON_DOWN, INPUT, false, false);
    btnEnter.begin(WATCH_BUTTON_ENTER, INPUT_PULLUP, false, true);
    Wire.setPins(WATCH_SDA, WATCH_SCL);
    acc.init();
    pinMode(VIBRATE_MOTOR, OUTPUT);
    gfx->begin(80000000); /* TFT init */
    gfx->fillScreen(BLACK);
    displayOn();
    setBrightness(Brightness);
    lvgl_begin();

    xTaskCreatePinnedToCore(update_loop, "WatchFace-Loop", 8192, NULL, configMAX_PRIORITIES - 2, NULL, CONFIG_ARDUINO_RUNNING_CORE);
}

void Watch_HAL::update()
{
    btnUp.loop();
    btnDown.loop();
    btnEnter.loop();
    motor_update();
    if (btnEnter.isPressedRaw() || btnUp.isPressedRaw() || btnDown.isPressedRaw())
    {
        release_time = millis();
    }
}

void Watch_HAL::displayOff(void)
{
    ledcDetachPin(WATCH_TFT_LED);
    pinMode(WATCH_TFT_LED, OUTPUT);
    digitalWrite(WATCH_TFT_LED, LOW);
    while (hal.screenMutex)
        ;
    hal.screenMutex = true;
    gfx->displayOff();
    hal.screenMutex = false;
}

void Watch_HAL::displayOn(void)
{
    ledcAttachPin(WATCH_TFT_LED, 1);
    ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
    while (hal.screenMutex)
        ;
    hal.screenMutex = true;
    gfx->displayOn();
    hal.screenMutex = false;
}

void Watch_HAL::setBrightness(uint8_t b)
{
    ledcWrite(1, b);
    if (b != 0)
        Brightness = b;
}

void Watch_HAL::lightSleep()
{
    Serial.print("Sleeping...");
    while (hal.screenMutex == true)
        vTaskDelay(10);
    hal.screenMutex = true;
    setBrightness(0);
    ledcDetachPin(WATCH_TFT_LED);
    gfx->displayOff();
    rtc_gpio_init(GPIO_NUM_13);
    rtc_gpio_pullup_dis(GPIO_NUM_13);
    esp_sleep_enable_ext1_wakeup(1 << 13 /* BTN_2 */, ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_32 /* RTC_INT */, LOW); // RTC中断
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_35 /* BMA_INT_2 / TAP */, HIGH); // step interrupts (currently)
    //esp_sleep_enable_ext0_wakeup(GPIO_NUM_34 /* BMA_INT_1 */, HIGH);  // tilt to wake and tap interrupts
    delay(100);
    Serial.println("Done!");
    esp_light_sleep_start();
    Serial.print("Waking up...");
    rtc_gpio_deinit(GPIO_NUM_13);
    rtc_gpio_deinit(GPIO_NUM_32);
    digitalWrite(33, HIGH);
    delay(25);
    digitalWrite(33, LOW);
    delay(25);
    digitalWrite(33, HIGH);
    delay(25);
    gfx->tftInit();
    gfx->displayOn();
    gfx->setRotation(0); // apply the setting rotation to the display
    gfx->setAddrWindow(0, 0, 240, 240);

    ledcAttachPin(WATCH_TFT_LED, 1);
    setBrightness(Brightness);

    hal.release_time = millis();
    hal.screenMutex = false;
    Serial.println("Done!");
    while (hal.btnEnter.isPressedRaw())
        ;
    delay(20);
}
void Watch_HAL::deepSleep()
{
    while (hal.screenMutex == true)
        vTaskDelay(10);
    hal.screenMutex = true;
    setBrightness(0);
    gfx->displayOff();
    rtc_gpio_init(GPIO_NUM_13);
    rtc_gpio_pullup_dis(GPIO_NUM_13);
    esp_sleep_enable_ext1_wakeup(1 << 13 /* BTN_2 */, ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_32 /* RTC_INT */, LOW); // RTC中断
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_35 /* BMA_INT_2 / TAP */, HIGH); // step interrupts (currently)
    //esp_sleep_enable_ext0_wakeup(GPIO_NUM_34 /* BMA_INT_1 */, HIGH);  // tilt to wake and tap interrupts
    delay(100);
    ledcDetachPin(WATCH_TFT_LED);
    esp_deep_sleep_start();
}

void Watch_HAL::motor_update()
{
    if(strcmp(hal.conf.getValue("enmotor"), "0") == 0)
    {
        motor_seq_tail = motor_seq_head;
        return;
    }
    if (motor_seq_tail != motor_seq_head)
    {
        if (motor_playing == false)
        {
            motor_playing = true;
            if (mot_seq[motor_seq_tail].type == MOTOR_RUN)
            {
                digitalWrite(VIBRATE_MOTOR, 1);
            }
            else
            {
                digitalWrite(VIBRATE_MOTOR, 0);
            }
            last_motor_millis = millis();
        }
        else
        {
            if (millis() - last_motor_millis >= mot_seq[motor_seq_tail].time)
            {
                motor_playing = false;
                motor_seq_tail++;
                if (motor_seq_tail == 30)
                {
                    motor_seq_tail = 0;
                }
            }
        }
    }
    else
    {
        digitalWrite(VIBRATE_MOTOR, 0);
    }
}

void Watch_HAL::motorAdd(enum enum_motor_type type, uint16_t time)
{
    mot_seq[motor_seq_head].type = type;
    mot_seq[motor_seq_head].time = time;
    ++motor_seq_head;
    if (motor_seq_head == 30)
        motor_seq_head = 0;
}

bool Watch_HAL::beginSmartconfig()
{
    uint8_t tries = 0;
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();

    while (!WiFi.smartConfigDone())
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ++tries;
        if (tries >= 120)
        {
            return false;
        }
        if (btnEnter.isPressedRaw())
        {
            return false;
        }
    }
    tries = 0;
    WiFi.mode(WIFI_STA);
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ++tries;
        if (tries >= 60)
        {
            return false;
        }
        if (btnEnter.isPressedRaw())
        {
            return false;
        }
    }
    conf.setValue("ssid", WiFi.SSID());
    conf.setValue("pwd", WiFi.psk());
    return true;
}

bool Watch_HAL::connectWiFi()
{
    uint8_t tries = 0;
    bool donotsleepstatus = DoNotSleep;
    DoNotSleep = true;
    WiFi.mode(WIFI_STA);
    WiFi.begin(conf.getValue("ssid"), conf.getValue("pwd"));
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(500);
        ++tries;
        if (tries > 60 || WiFi.status() == WL_CONNECT_FAILED)
        {
            goto error;
        }
        if (hal.btnEnter.isPressedRaw())
        {
            goto error;
        }
    }
    DoNotSleep = donotsleepstatus;
    return true;
error:
    WiFi.mode(WIFI_OFF);
    DoNotSleep = donotsleepstatus;
    return false;
}

void Watch_HAL::disconnectWiFi()
{
    WiFi.mode(WIFI_OFF);
}

/*-------- NTP ----------*/

static const int NTP_PACKET_SIZE = 48;        // NTP time is in the first 48 bytes of message
static byte NTPpacketBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
    // set all bytes in the buffer to 0
    memset(NTPpacketBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    NTPpacketBuffer[0] = 0b11100011; // LI, Version, Mode
    NTPpacketBuffer[1] = 0;          // Stratum, or type of clock
    NTPpacketBuffer[2] = 6;          // Polling Interval
    NTPpacketBuffer[3] = 0xEC;       // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    NTPpacketBuffer[12] = 49;
    NTPpacketBuffer[13] = 0x4E;
    NTPpacketBuffer[14] = 49;
    NTPpacketBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    hal.Udp.beginPacket(address, 123); //NTP requests are to port 123
    hal.Udp.write(NTPpacketBuffer, NTP_PACKET_SIZE);
    hal.Udp.endPacket();
}
time_t getNtpTime()
{
    IPAddress ntpServerIP;
    while (hal.Udp.parsePacket() > 0)
        ;
    WiFi.hostByName(CONFIG_NTP_ADDR, ntpServerIP);
    sendNTPpacket(ntpServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500)
    {
        int size = hal.Udp.parsePacket();
        if (size >= NTP_PACKET_SIZE)
        {
            hal.Udp.read(NTPpacketBuffer, NTP_PACKET_SIZE);
            unsigned long secsSince1900;
            secsSince1900 = (unsigned long)NTPpacketBuffer[40] << 24;
            secsSince1900 |= (unsigned long)NTPpacketBuffer[41] << 16;
            secsSince1900 |= (unsigned long)NTPpacketBuffer[42] << 8;
            secsSince1900 |= (unsigned long)NTPpacketBuffer[43];
            return secsSince1900 - 2208988800UL;
        }
    }
    return 0; // return 0 if unable to get the time
}

bool Watch_HAL::NTPSync()
{
    if (hal.connectWiFi() == false)
    {
        return false;
    }
    vTaskDelay(100);
    hal.Udp.begin(8888);
    time_t tm_now = getNtpTime();
    if (tm_now != 0)
    {
        DateTime dt(tm_now + CONFIG_NTP_OFFSET);
        hal.rtc.setYear(dt.year() - 2000);
        hal.rtc.setMonth(dt.month());
        hal.rtc.setDate(dt.day());
        tm_now /= 86400;
        uint8_t week = (tm_now + 4) % 7;
        hal.rtc.setDoW(week ? week : 7);
        hal.rtc.setHour(dt.hour());
        hal.rtc.setMinute(dt.minute());
        hal.rtc.setSecond(dt.second());
    }
    hal.Udp.stop();
    WiFi.mode(WIFI_OFF);
    if (tm_now == 0)
        return false;
    return true;
}