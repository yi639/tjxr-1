#include "soil_sensor.h"

// 土壤湿度传感器初始化
void soil_sensor_init() {
  // 配置ADC
  analogReadResolution(ADC_WIDTH);
  analogSetAttenuation(ADC_ATTEN);
  analogSetPinAttenuation(PIN_SOIL_AO, ADC_ATTEN);
  Serial.println("土壤湿度传感器初始化完成！");
}

// // 读取土壤湿度(%)
float soil_sensor_read() {
  int ao_value = analogRead(PIN_SOIL_AO);
  float voltage = ao_value * 3.3f / 4095.0f;
  float humidity = 0.0f;
  
  if (voltage >= SOIL_DRY_VOLTAGE) {
    humidity = 0.0f;
  } else if (voltage <= SOIL_WET_VOLTAGE) {
    humidity = 100.0f;
  } else {
    humidity = (SOIL_DRY_VOLTAGE - voltage) / (SOIL_DRY_VOLTAGE - SOIL_WET_VOLTAGE) * 100.0f;
  }
  
  Serial.print("土壤湿度：");
  Serial.print(humidity, 1);
  Serial.println("%");
  
  return humidity;
}