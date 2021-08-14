#ifndef __MORSE_H__
#define __MORSE_H__
#include "A_config.h"

class Morse
{
#define MAX_INPUT_CHR 7 //字符最大按键数
#define MORSE_TOTAL 36
private:
    /**
     * @brief 当前chr缓冲区morse匹配
     */
    void findMorse();
    /**
     * @brief 在inputstr数组尾部插入一个字符，并判断是否溢出
     */
    void insertChr(char chr);
    const char *MorseTable[MORSE_TOTAL] = {
        ".-",
        "-...",
        "-.-.",
        "-..",
        ".",
        "..-.",
        "--.",
        "....",
        "..",
        ".---",
        "-.-",
        ".-..",
        "--",
        "-.",
        "---",
        ".--.",
        "--.-",
        ".-.",
        "...",
        "-",
        "..-",
        "...-",
        ".--",
        "-..-",
        "-.--",
        "--..",
        "-----",
        ".----",
        "..---",
        "...--",
        "....-",
        ".....",
        "-....",
        "--...",
        "---..",
        "----."};
    const char *AsciiTable = "abcdefghijklmnopqrstuvwxyz0123456789";
    char inputStr[MAX_INPUT_STR] = {0};      //输入的字符串
    char inputChr[MAX_INPUT_CHR] = {0};      //输入的字符
    uint32_t pressTime = 0, releaseTime = 0; //按下的时间、松开的时间
    uint8_t strPointer = 0;                  //字符串指针
    uint8_t chrPointer = 0;                  //字符指针
    bool lastState = false;
    bool enabled = false;
    bool flagSpace = false;             //自动空格标志
public:
    /**
 * @brief 初始化morse
 */
    void init();

    /**
 * @brief 初始化morse输入
 */
    void start()
    {
        memset(inputStr, 0, MAX_INPUT_STR);
        memset(inputChr, 0, MAX_INPUT_CHR);
        lastState = hal.INPUTBUTTON.isPressedRaw();
        strPointer = 0;
        chrPointer = 0;
        pressTime = 0;
        releaseTime = 0;
        enabled = true;
        flagSpace = false;
    }
    
    /**
     * @brief 获取缓冲区中剩余字符数
     * @return 缓冲区字符数
     */
    uint8_t available()
    {
        return strPointer;
    }

    /**
     * @brief 获取缓冲区内容，并清空缓冲区
     * @return 如果有，则返回那个字符或字符串，否则返回""
     */
    String getInput()
    {
        inputStr[strPointer] = 0;
        strPointer = 0;
        return String(inputStr);
    }

    /**
     * @brief 获取缓冲区顶端一个字符，并返回
     * @return 最近输入的字符，注意是最近输入的，可以把缓冲区看做堆栈，如果没有，则返回0x00
     */
    char getChar()
    {
        if (strPointer == 0)
        {
            return 0;
        }
        --strPointer;
        return inputStr[strPointer];
    }

    void pause()
    {
        enabled = false;
    }

    void resume()
    {
        enabled = true;
        flagSpace = false;
    }
    /**
     * @brief 处理输入。保证每隔1ms调用一次
     */
    void update();
    bool enableAutoSpace = true; //是否允许自动空格
};

extern Morse morse;
#endif