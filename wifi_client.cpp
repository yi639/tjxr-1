#include "wifi_client.h"
#include <WiFi.h>

// 全局变量（wifi_client.cpp内部使用）
static WiFiStatus currentStatus = WIFI_DISCONNECTED;
static unsigned long lastReconnectAttempt = 0;
static unsigned long connectionStartTime = 0;
static bool connecting = false;
static const unsigned long CONNECTION_TIMEOUT = 10000; // 10秒超时
static const unsigned long RECONNECT_INTERVAL = 30000; // 30秒重连间隔

// 唯一的initWiFi实现（供main.ino调用）
void initWiFi(const char* ssid, const char* password) {
    Serial.println("开始初始化WiFi连接...");
    Serial.print("SSID: ");
    Serial.println(ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    connecting = true;
    connectionStartTime = millis();
    currentStatus = WIFI_CONNECTING;
    
    Serial.println("WiFi连接已启动（非阻塞模式）");
}



// 尝试连接WiFi（非阻塞）
bool tryConnectWiFi(const char* ssid, const char* password) {
    if (WiFi.status() == WL_CONNECTED) {
        currentStatus = WIFI_CONNECTED;
        return true;
    }
    
    // 如果不在连接状态，开始连接
    if (!connecting) {
        Serial.println("尝试连接WiFi...");
        WiFi.begin(ssid, password);
        connecting = true;
        connectionStartTime = millis();
        currentStatus = WIFI_CONNECTING;
    }
    
    // 检查连接状态
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi连接成功!");
        Serial.print("IP地址: ");
        Serial.println(WiFi.localIP());
        connecting = false;
        currentStatus = WIFI_CONNECTED;
        return true;
    }
    
    // 如果连接超时（10秒），重置连接状态
    if (millis() - connectionStartTime > CONNECTION_TIMEOUT) {
        Serial.println("WiFi连接超时");
        connecting = false;
        WiFi.disconnect();
        currentStatus = WIFI_DISCONNECTED;
    }
    
    return false;
}

// 获取WiFi状态
WiFiStatus getWiFiStatus() {
    if (WiFi.status() == WL_CONNECTED) {
        currentStatus = WIFI_CONNECTED;
    } else if (connecting) {
        currentStatus = WIFI_CONNECTING;
    } else {
        currentStatus = WIFI_DISCONNECTED;
    }
    return currentStatus;
}

// 检查并尝试重连WiFi
void checkAndReconnectWiFi(const char* ssid, const char* password) {
    unsigned long currentTime = millis();
    
    // 如果未连接且超过重连间隔，尝试重连
    if (WiFi.status() != WL_CONNECTED && 
        currentTime - lastReconnectAttempt >= RECONNECT_INTERVAL) {
        lastReconnectAttempt = currentTime;
        Serial.println("尝试重连WiFi...");
        tryConnectWiFi(ssid, password);
    }
}

// 获取WiFi状态字符串
String getWiFiStatusString() {
    WiFiStatus status = getWiFiStatus();
    switch (status) {
        case WIFI_DISCONNECTED:
            return "离线";
        case WIFI_CONNECTING:
            return "连接中";
        case WIFI_CONNECTED:
            return "已连接";
        default:
            return "未知";
    }
}

// 获取最后重连尝试时间
unsigned long getLastReconnectAttempt() {
    return lastReconnectAttempt;
}

// 获取重连间隔
unsigned long getReconnectInterval() {
    return RECONNECT_INTERVAL;
}