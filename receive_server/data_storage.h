#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config_receiver.h"

// 函数声明
void initDataStorage();
void storeSensorData(const SensorData& data);
String getHistoryDataJSON();
int getHistoryCount();
void cleanupOldData();

#endif // DATA_STORAGE_H
