#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>
#include "config.h"

// // 函数声明
void light_sensor_init();
bool light_sensor_read();
float light_sensor_read_intensity();  //读取光照强度百分比

#endif // LIGHT_SENSOR_H