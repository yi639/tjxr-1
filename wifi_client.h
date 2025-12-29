#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <WiFi.h>
#include <Arduino.h>

// WiFi连接状态枚举
typedef enum {
    WIFI_DISCONNECTED,
    WIFI_CONNECTING,
    WIFI_CONNECTED
} WiFiStatus;

// 函数声明
void initWiFi(const char* ssid, const char* password);
bool tryConnectWiFi(const char* ssid, const char* password);
WiFiStatus getWiFiStatus();
void checkAndReconnectWiFi(const char* ssid, const char* password);
String getWiFiStatusString();
unsigned long getLastReconnectAttempt();
unsigned long getReconnectInterval();

#endif