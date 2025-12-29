#include "dht11_sensor.h"

// // 全局DHT对象定义
DHT dht11(PIN_DHT11_DATA, DHT_TYPE);

// // DHT11初始化
void dht11_init() {
  dht11.begin();
  Serial.println("DHT11初始化完成!");
}

// // 读取温度
float dht11_read_temp() {
  float temp = dht11.readTemperature();
  if (isnan(temp)) {
    Serial.println("DHT11温度读取失败!");
    return 0.0f;
  }
  return temp;
}

// // 读取湿度
float dht11_read_hum() {
  float hum = dht11.readHumidity();
  if (isnan(hum)) {
    Serial.println("DHT11湿度读取失败!");
    return 0.0f;
  }
  return hum;
}