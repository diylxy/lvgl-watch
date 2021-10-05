#ifndef __WATCH_IR_H__
#define __WATCH_IR_H__
#include <A_config.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>

class WatchIR
{
private:
    /* data */
public:
    void enableIROut(bool en)      //是否允许红外发射
    {
        if(en)
        {
            pinMode(WATCH_IR_LED, OUTPUT);
            digitalWrite(WATCH_IR_LED, 1);
        }
        else
        {
            pinMode(WATCH_IR_LED, INPUT);
            digitalWrite(WATCH_IR_LED, 1);
        }
    }
    /**
     * @brief 将空调设置为初始状态
     */
    void ACModeReset();
};

extern WatchIR ir;
extern IRsend irsend;
extern IRac irac;
#endif