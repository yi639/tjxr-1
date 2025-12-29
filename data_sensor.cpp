#include "data_sensor.h"
#include "dht11_sensor.h"
#include "light_sensor.h"
#include "soil_sensor.h"  
#include "config.h"

// 初始化所有传感器（DHT11 + 光敏电阻 + 土壤湿度传感器）
void sensor_init() {
  dht11_init();          // 初始化DHT11温湿度传感器
  light_sensor_init();   // 初始化光敏电阻传感器
  soil_sensor_init();    // 初始化土壤湿度传感器（解除注释启用）
  Serial.println("所有传感器初始化完成！");
}

// 读取所有传感器数据，整合为SensorData结构体
SensorData readSensorData() {
  SensorData data;
  
  // 1. 读取DHT11温湿度
  data.temp = dht11_read_temp();
  data.hum = dht11_read_hum();
  
  // 2. 读取土壤湿度（解除注释启用真实数据，否则保持模拟值）
  data.soil_hum = soil_sensor_read();  // 真实土壤湿度读取

  
  // 3. 读取光照数据（强度+状态）
  data.light_intensity = light_sensor_read_intensity();
  data.light_strong = (data.light_intensity >= LIGHT_STRONG_THRESHOLD) ? true : false;
  
  // 4. 记录时间戳
  data.timestamp = millis();
  
  // 串口打印日志（便于调试）
  Serial.printf("采集数据：空气温=%.1f℃, 空气湿=%.1f%%, 土壤湿=%.1f%%, 光照强度=%.1f%%, 光照状态=%s\n",
                data.temp, data.hum, data.soil_hum,
                data.light_intensity, data.light_strong ? "过强" : "正常");
  
  return data;
}