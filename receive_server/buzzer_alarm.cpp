#include <Arduino.h>   // 补充Arduino核心函数定义
#include "config_receiver.h"  // 补充引脚定义
#include "buzzer_alarm.h"

// 蜂鸣器初始化
void buzzer_init() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, HIGH); // 初始关闭（高电平截止）
  Serial.println("蜂鸣器初始化完成！");
}

// 蜂鸣器短鸣（光照过强报警）
void buzzer_beep_short() {
  digitalWrite(PIN_BUZZER, LOW);
  delay(500);
  digitalWrite(PIN_BUZZER, HIGH);
  delay(20);
}

// 蜂鸣器长鸣（温度超限报警）
void buzzer_beep_temp() {
  static unsigned long beep_start_time = 0;
  unsigned long current_time = millis();

  if (beep_start_time == 0) { // 首次调用，启动计时
    digitalWrite(PIN_BUZZER, LOW);
    beep_start_time = current_time;
  } else if (current_time - beep_start_time > 60000) { 
    digitalWrite(PIN_BUZZER, HIGH);
    beep_start_time = 0; // 重置计时
  }
}
