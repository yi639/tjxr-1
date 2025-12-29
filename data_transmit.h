#ifndef DATA_TRANSMIT_H
#define DATA_TRANSMIT_H

#include <WiFiClient.h>
#include "data_sensor.h"

bool sendDataToServer(WiFiClient& client, const char* serverIP, uint16_t serverPort, SensorData data);

#endif