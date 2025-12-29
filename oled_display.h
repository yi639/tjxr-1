#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Wire.h>
#include <U8g2lib.h>
#include "config.h"

// 全局OLED对象声明
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

// 函数声明
void oled_init();
void oled_show_msg(const char* msg);
void oled_update_display(float soil_hum, float temp, float hum, bool light_strong, 
                          bool fan_state, bool pump_state);

#endif // OLED_DISPLAY_H
