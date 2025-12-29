#include "light_sensor.h"

// // 光敏电阻传感器初始化
void light_sensor_init() {
  pinMode(PIN_LIGHT_DO, INPUT);
  pinMode(PIN_LIGHT_AO, INPUT); 
  Serial.println("光敏电阻传感器初始化完成！");
}

// // 读取光线状态(返回是否过强)
bool light_sensor_read() {
  float light_intensity = light_sensor_read_intensity();
  bool light_strong = false;
  
  // 数值判断逻辑：超过阈值则为“过强”，否则为“正常”
  if (light_intensity >= LIGHT_STRONG_THRESHOLD) {
    light_strong = true;
  } else {
    light_strong = false;
  }
  
  Serial.print("光线状态：");
  Serial.println(light_strong ? "过强" : "正常");
  
  return light_strong;
}
float light_sensor_read_intensity() {
  // 读取ADC值（ESP32 12位ADC，范围0~4095）
  int adc_value = analogRead(PIN_LIGHT_AO);
  // 转换为0~100%（光线越强，ADC值越小，此处做反向映射）
  float light_intensity = 100.0 - (adc_value / 4095.0 * 100.0);
  return light_intensity;
}