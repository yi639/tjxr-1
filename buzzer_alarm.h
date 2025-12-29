#ifndef BUZZER_ALARM_H
#define BUZZER_ALARM_H

#include <Arduino.h>
#include "config.h"

// // 函数声明
void buzzer_init();
void buzzer_beep_short();
void buzzer_beep_temp();

#endif // BUZZER_ALARM_H