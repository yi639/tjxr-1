#ifndef DATA_SENSOR_H
#define DATA_SENSOR_H

struct SensorData {
  float temp;    // 空气温度（DHT11）
  float hum;       // 空气湿度（DHT11）
  float soil_hum;      // 土壤湿度（土壤传感器）
  float light_intensity;    // 光照强度（%，光敏电阻）
  bool light_strong;        // 光照状态（是否过强）
  unsigned long timestamp;  // 时间戳
};

// 传感器初始化（统一初始化所有启用的传感器）
void sensor_init();

SensorData readSensorData();  // 读取传感器数据

#endif