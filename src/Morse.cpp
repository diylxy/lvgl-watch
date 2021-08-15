#include "A_config.h"

Morse morse;

/**
 * @brief morse输入更新
 */
static void update_morse(void *param)
{
    while (1)
    {
        morse.update();
        vTaskDelay(1);
    }
}

void Morse::init()
{
    xTaskCreatePinnedToCore(update_morse, "Morse-Loop", 2048, NULL, configMAX_PRIORITIES - 2, NULL, !CONFIG_ARDUINO_RUNNING_CORE);
}

void Morse::insertChr(char chr)
{
    inputStr[strPointer++] = chr;
    if (strPointer == MAX_INPUT_STR - 1)
    {
        inputStr[strPointer] = 0;
        --strPointer;
    }
}

void Morse::findMorse()
{
    unsigned char i;
    inputChr[chrPointer] = 0;
    for (i = 0; i < MORSE_TOTAL; ++i)
    {
        if (strcmp(inputChr, MorseTable[i]) == 0)
        {
            insertChr(AsciiTable[i]);
            chrPointer = 0;
            return;
        }
    }
    chrPointer = 0;
}

void Morse::update()
{
    if(enabled == false)return;
    if (hal.INPUTBUTTON.isPressedRaw())
    {
        if (lastState == false)
        {
            lastState = true;
            pressTime = 0;
            releaseTime = 0;
            return;
        }
        pressTime++;
        if (pressTime == LONG_TIME)
        {
            insertChr('\n');
            chrPointer = 0;
            return;
        }
    }
    else //没有按下
    {
        if (lastState == true)
        {
            lastState = false;
            releaseTime = 0;
            flagSpace = true;
            return;
        }
        releaseTime++;
        if (releaseTime == 1)
        {
            if (pressTime == 0)
            {
            }
            else if (pressTime < DA_TIME) //.
            {
                inputChr[chrPointer++] = '.';
            }
            else if (pressTime < LONG_TIME) //-
            {
                inputChr[chrPointer++] = '-';
            }
            if (chrPointer == MAX_INPUT_CHR)
            {
                chrPointer = 0;
            }
        }
        else if (releaseTime == CHR_TIME && chrPointer != 0)
        {
            inputChr[chrPointer] = 0;
            findMorse();
            chrPointer = 0;
            return;
        }
        else if (releaseTime == LONG_TIME * 3 && enableAutoSpace && flagSpace)
        {
            flagSpace = false;
            insertChr(' ');
        }
    }
}