#include "actuator_control.h"

// // // 全局变量定义
// unsigned long pump_last_run_time = 0;

// // // 执行器初始化
// void actuator_init() {
//   pinMode(PIN_PUMP_RELAY, OUTPUT);
//   pinMode(PIN_FAN_RELAY, OUTPUT);
//   digitalWrite(PIN_PUMP_RELAY, LOW);
//   digitalWrite(PIN_FAN_RELAY, LOW);
//   Serial.println("执行器初始化完成！");
// }

// // // 水泵控制(返回水泵状态)
// bool actuator_control_pump(float soil_hum) {
//   unsigned long current_time = millis();
//   bool pump_state = false;
  
//   if (soil_hum < SOIL_WATER_THRESHOLD && (current_time - pump_last_run_time) > PUMP_INTERVAL) {
//     Serial.println("土壤过干，启动水泵浇水...");
//     digitalWrite(PIN_PUMP_RELAY, HIGH);
//     pump_state = true;
//     //buzzer_beep_short(); // 蜂鸣器报警
//     delay(PUMP_WATER_TIME);
//     digitalWrite(PIN_PUMP_RELAY, LOW);
//     pump_state = false;
//     pump_last_run_time = current_time;
//     Serial.println("浇水完成，关闭水泵");
//   }
  
//   return pump_state;
// }

// // // 风扇控制(返回风扇状态)
// bool actuator_control_fan(bool light_strong, float temp) {
//   bool fan_state = false;
  
//   if ( temp > TEMP_FAN_THRESHOLD) {
//     digitalWrite(PIN_FAN_RELAY, HIGH);
//     fan_state = true;
//     Serial.println("温度过高，启动风扇降温...");
//   } else {
//     digitalWrite(PIN_FAN_RELAY, LOW);
//     fan_state = false;
//     Serial.println("温度正常，关闭风扇");
//   }
  
//   return fan_state;
// }