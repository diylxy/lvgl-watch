#include "A_config.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "FreeRTOS.h"

static Arduino_DataBus *bus = new Arduino_ESP32SPI_DMA(12, 5, 18, 23, -1, VSPI);
static Arduino_TFT *gfx = new Arduino_GC9A01(bus, 33, 0 /* rotation */, true /* IPS */);

static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf[DISPLAY_MAX_X * 80];
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

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

void loop()
{
    lv_timer_handler();
    vTaskDelay(5/portTICK_PERIOD_MS);
}

static void lvgl_begin()
{
    lv_init();

    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISPLAY_MAX_X * 80);

    /*Initialize the display*/
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

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

    btnUp.begin(WATCH_BUTTON_UP, INPUT, false, false);
    btnDown.begin(WATCH_BUTTON_DOWN, INPUT, false, false);
    btnEnter.begin(WATCH_BUTTON_ENTER, INPUT_PULLUP, false, true);
    Wire.setPins(WATCH_SDA, WATCH_SCL);
    acc.init();
    rtc.begin();
    pinMode(VIBRATE_MOTOR, OUTPUT);

    gfx->begin(80000000); /* TFT init */
    displayOn();
    gfx->fillScreen(BLACK);
    setBrightness(60);
    lvgl_begin();
}

void Watch_HAL::update()
{
    btnUp.loop();
    btnDown.loop();
    btnEnter.loop();
    motor_update();
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
}

void Watch_HAL::deepSleep()
{
    setBrightness(0);
    gfx->displayOff();
    rtc_gpio_init(GPIO_NUM_0);
    rtc_gpio_pullup_en(GPIO_NUM_0);
    esp_sleep_enable_ext1_wakeup(1 /* BTN_0 */, ESP_EXT1_WAKEUP_ALL_LOW);
    //esp_sleep_enable_ext0_wakeup(GPIO_NUM_32 /* BMA_INT_1 */, LOW); // RTC中断
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_35 /* BMA_INT_2 / TAP */, HIGH); // step interrupts (currently)
    //esp_sleep_enable_ext0_wakeup(GPIO_NUM_34 /* BMA_INT_1 */, HIGH);  // tilt to wake and tap interrupts
    while(hal.screenMutex == true);
    esp_deep_sleep_start();
}

void Watch_HAL::motor_update()
{
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