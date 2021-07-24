#ifndef __WATCH_HAL_H__
#define __WATCH_HAL_H__
#include "A_config.h"
#include "Button2.h"
#include "BlueDot_BMA400.h"
#include "RTClib.h"
#include <SPI.h>
#include <Arduino_GFX.h>
#include <Arduino_ESP32SPI_DMA.h>
#include <Arduino_GC9A01.h>

enum enum_motor_type
{
    MOTOR_SLEEP,
    MOTOR_RUN
};

class Watch_HAL
{
public:
    void begin();
    void update();
    void deepSleep();
    void setBrightness(uint8_t b);
    void displayOn(void);
    void displayOff(void);
    void motorAdd(enum enum_motor_type type, uint16_t time);
    Button2 btnUp, btnDown, btnEnter;
    BlueDot_BMA400 acc;
    RTC_DS3231 rtc;
    bool screenMutex = false;
    TaskHandle_t handleScreenUpdate;
private:
    struct motor_seq
    {
       enum enum_motor_type type;
       uint16_t time; 
    };
    struct motor_seq mot_seq[30];
    uint8_t motor_seq_head = 0;
    uint8_t motor_seq_tail = 0;
    uint32_t last_motor_millis = 0;
    bool motor_playing = false;
    void motor_update();
};
extern Watch_HAL hal;
#endif