#ifndef DHT11_SENSOR_H
#define DHT11_SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"

// // // 全局DHT对象声明
extern DHT dht11;

// // 函数声明
void dht11_init();
float dht11_read_temp();
float dht11_read_hum();

#endif // DHT11_SENSOR_H