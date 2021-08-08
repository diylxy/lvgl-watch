#include "A_config.h"


typedef void (* wfSetupFunc)(void);

static wfSetupFunc funcStack[20];
static uint8_t top = 0;


void pushWatchFace(wfSetupFunc setup)
{
    funcStack[top] = setup;
    ++top;
    if(top >= 20)
    {
        Serial.println("Watchface stack overflowed!!!");
        abort();
    }
}

void popWatchFace()
{
    if(top == 0)
    {
        Serial.println("Watchface stack underflowed!!!");
        abort();
    }
    --top;
    funcStack[top]();
}