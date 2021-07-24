#ifndef __MENU_H__
#define __MENU_H__
#include <Arduino.h>

void menu_create(void);
void menu_add(const char *str);
int16_t menu_show(int16_t startAt = 0);
void menu_slide(int8_t direction);
void menu_add(char *str);
void menu_create(void);
void msgbox(const char *prompt, const char *msg);


#endif