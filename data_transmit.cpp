#include "data_transmit.h"
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>

// 引用main.ino中的全局变量
extern unsigned long lastSendTime;

bool sendDataToServer(WiFiClient& client, const char* serverIP, uint16_t serverPort, SensorData data) {
  // 1. 检查WiFi连接
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[发送失败] WiFi未连接");
    return false;
  }

  // 2. 解析接收端IP
  IPAddress destIP;
  if (!destIP.fromString(serverIP)) {
    Serial.println("[发送失败] 接收端IP格式错误");
    return false;
  }

  // 3. 连接接收端（80端口）
  if (!client.connected()) {
    Serial.print("[连接] 尝试连接接收端：");
    Serial.print(serverIP);
    Serial.print(":");
    Serial.println(serverPort);
    
    if (!client.connect(destIP, serverPort, 5000)) {
      Serial.println("[连接失败] 接收端未响应");
      client.stop();
      return false;
    }
  }

  // 4. 构建符合要求的JSON（字段与测试数据完全一致）
  StaticJsonDocument<300> doc;
  doc["air_temp"] = data.temp;         // 字段名：air_temp（必须）
  doc["air_hum"] = data.hum;           // 字段名：air_hum（必须）
  doc["soil_hum"] = data.soil_hum;     // 字段名：soil_hum（必须）
  doc["light_int"] = data.light_intensity; // 字段名：light_int（必须，不是light_intensity）
  doc["light_strong"] = data.light_strong; // 字段名：light_strong（必须）
  doc["timestamp"] = data.timestamp;   // 字段名：timestamp（必须）

  String jsonData;
  serializeJson(doc, jsonData);
  Serial.print("[JSON] 待发送数据：");
  Serial.println(jsonData);

  // 5. 构建HTTP POST请求（核心：符合HTTP协议规范）
  String postRequest = 
    "POST /sensor HTTP/1.1\r\n"          // POST方法 + /sensor接口（必须）
    "Host: " + String(serverIP) + "\r\n" // Host头（必须）
    "Content-Type: application/json\r\n" // 声明JSON格式（必须）
    "Content-Length: " + String(jsonData.length()) + "\r\n" // 数据长度（必须）
    "Connection: close\r\n\r\n"          // 关闭连接（避免复用问题）
    + jsonData;                          // 拼接JSON数据

  // 6. 发送HTTP请求
  client.setTimeout(2000);
  size_t bytesSent = client.print(postRequest);
  client.clear(); // 替换废弃的flush()

  if (bytesSent != postRequest.length()) {
    Serial.print("[发送失败] 数据不完整，发送：");
    Serial.print(bytesSent);
    Serial.print("/");
    Serial.println(postRequest.length());
    client.stop();
    return false;
  }

  // 7. 等待接收端响应（验证是否成功）
  Serial.println("[发送成功] 等待接收端响应...");
  unsigned long waitStart = millis();
  while (millis() - waitStart < 1000) {
    if (client.available()) {
      String response = client.readStringUntil('\n');
      Serial.print("[接收端响应] ");
      Serial.println(response);
      break;
    }
  }

  // 8. 空闲时关闭连接
  if (millis() - lastSendTime > 10000) {
    client.stop();
  }

  return true;
}