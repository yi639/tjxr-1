#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>  // 需安装 PubSubClient 2.8.0 库
#include <WiFiClientSecure.h>
#include "config_receiver.h"
#include "data_storage.h"
#include "actuator_control.h"
#include "buzzer_alarm.h"

// -------------------------- 全局变量（修复核心冲突问题）--------------------------
WebServer server(SERVER_PORT);          // TCP WebServer（端口80）
SensorData currentData;                 // 当前传感器数据
bool newDataAvailable = false;          // 新数据可用标记
unsigned long lastDataTime = 0;         // 上次接收数据时间
WiFiClientSecure wifiClient;            // TLS客户端（用于MQTT 8883端口）
PubSubClient mqttClient(wifiClient);    // MQTT客户端（绑定TLS）
bool mqttConnected = false;             // MQTT连接状态标记

// -------------------------- WiFi初始化（STA模式，连接路由器）--------------------------
void initWiFiSTA() {
  Serial.println("=== 初始化WiFi（STA模式）===");
  WiFi.mode(WIFI_STA);                  // 纯STA模式（仅连接路由器，不创建热点）
  WiFi.begin(ROUTER_SSID, ROUTER_PASSWORD);
  
  int retry = 0;
  // 等待WiFi连接（最多重试20次，每次500ms）
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi连接成功！");
    Serial.print("接收端IP地址: ");
    Serial.println(WiFi.localIP().toString()); // 采集端需配置此IP为SERVER_IP
  } else {
    Serial.println("\nWiFi连接失败！请检查SSID和密码");
  }
}

// -------------------------- MQTT连接与重连（修复核心调用错误）--------------------------
void connectMQTT() {
  // 未连接时循环重试
  while (!mqttClient.connected()) {
    Serial.printf("=== 连接MQTT服务器：%s:%d ===\n", MQTT_SERVER, MQTT_PORT);
    
    // 生成唯一客户端ID（避免多设备冲突，EMQX服务器要求唯一）
    String clientId = MQTT_CLIENT_ID;
    clientId += "-";
    clientId += String(random(0xffff), HEX); // 追加随机数
    
    // PubSubClient 2.8.0 正确连接参数（支持用户名密码+Clean Session）
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD, NULL, 0, false, NULL, true)) {
      Serial.println("MQTT连接成功！");
      mqttClient.subscribe(MQTT_TOPIC_CONTROL); // 订阅执行器控制主题
      mqttConnected = true;
    } else {
      Serial.print("MQTT连接失败，错误码: ");
      Serial.print(mqttClient.state()); // 打印错误码（便于排查）
      Serial.println("，5秒后重试");
      delay(5000);
    }
  }
}

// -------------------------- MQTT消息回调（处理远程控制命令）--------------------------
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("收到MQTT控制命令，主题: ");
  Serial.print(topic);
  Serial.print("，内容: ");
  
  // 解析MQTT payload为字符串
  String cmd = String((char*)payload, length);
  Serial.println(cmd);
  
  // 解析JSON格式命令
  StaticJsonDocument<100> doc;
  DeserializationError error = deserializeJson(doc, cmd);
  if (!error) {
    // 控制水泵（{"pump":true} 开启，{"pump":false} 关闭）
    if (doc.containsKey("pump")) {
      bool pumpOn = doc["pump"];
      digitalWrite(PIN_PUMP_RELAY, pumpOn ? HIGH : LOW);
      Serial.print("水泵远程控制: ");
      Serial.println(pumpOn ? "开启" : "关闭");
    }
    // 控制风扇（{"fan":true} 开启，{"fan":false} 关闭）
    if (doc.containsKey("fan")) {
      bool fanOn = doc["fan"];
      digitalWrite(PIN_FAN_RELAY, fanOn ? HIGH : LOW);
      Serial.print("风扇远程控制: ");
      Serial.println(fanOn ? "开启" : "关闭");
    }
  } else {
    Serial.println("MQTT命令JSON解析失败！");
  }
}

// -------------------------- WebServer回调：处理采集端数据上传 --------------------------
void handleSensorData() {
    // 1. 强制打印原始请求数据（关键调试）
    Serial.println("===== 原始请求数据 =====");
    Serial.print("Has plain arg: ");
    Serial.println(server.hasArg("plain") ? "YES" : "NO");
    int reqLength = server.hasArg("plain") ? server.arg("plain").length() : 0;
    Serial.print("Request body length: ");
    Serial.println(reqLength);
    if (server.hasArg("plain")) {
        String jsonData = server.arg("plain");
        Serial.print("Raw JSON: ");
        Serial.println(jsonData);
    } else {
        Serial.println("NO PLAIN ARG RECEIVED!");
        server.send(400, "application/json; charset=utf-8", "{\"status\":\"error\",\"message\":\"Empty body\"}");
        return;
    }

    // 2. 解析JSON并打印解析结果
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    if (!error) {
        // 打印解析后的数据
        Serial.println("===== 解析后数据 =====");
        Serial.print("air_temp: ");
        Serial.println(doc["air_temp"].as<float>());
        Serial.print("soil_hum: ");
        Serial.println(doc["soil_hum"].as<float>());
        Serial.print("light_strong: ");
        Serial.println(doc["light_strong"].as<bool>());

        // 强制赋值（即使字段缺失，也给默认值）
        currentData.temp = doc.containsKey("air_temp") ? doc["air_temp"].as<float>() : 25.0f;
        currentData.hum = doc.containsKey("air_hum") ? doc["air_hum"].as<float>() : 60.0f;
        currentData.soil_hum = doc.containsKey("soil_hum") ? doc["soil_hum"].as<float>() : 30.0f;
        currentData.light_intensity = doc.containsKey("light_int") ? doc["light_int"].as<float>() : 50.0f;
        currentData.light_strong = doc.containsKey("light_strong") ? doc["light_strong"].as<bool>() : false;
        currentData.timestamp = doc.containsKey("timestamp") ? doc["timestamp"].as<unsigned long>() : millis();
        currentData.receive_time = millis();

        // 强制存储（不管数据是否合理）
        storeSensorData(currentData);
        newDataAvailable = true;
        lastDataTime = millis();

        // 打印存储结果
        Serial.print("存储后历史数据条数: ");
        Serial.println(getHistoryCount());

        server.sendHeader("Connection", "close");
        server.send(200, "application/json; charset=utf-8", "{\"status\":\"success\",\"message\":\"Data received\",\"receive_time\":" + String(millis()) + "}");
    } else {
        Serial.printf("JSON解析失败：%s\n", error.c_str());
        server.send(400, "application/json; charset=utf-8", "{\"status\":\"error\",\"message\":\"Invalid JSON\",\"error\":\"" + String(error.c_str()) + "\"}");
    }
}

// -------------------------- WebServer路由配置（供采集端/小程序调用）--------------------------
void setupRoutes() {
  // 根路径（测试用）
  server.on("/", HTTP_GET, []() {
    String html = 
                  "<!DOCTYPE html>"
                  "<html lang=\"zh-CN\">" // 声明中文语言
                  "<head>"
                  "<meta charset=\"UTF-8\">" // 强制浏览器用UTF-8解析
                  "<title>ESP32接收端</title>"
                  "</head>"
                  "<body>""<h1>ESP32接收端</h1>"
                  "<p>1. 采集端数据上传：POST /sensor (JSON格式)</p>"
                  "<p>2. 小程序实时数据：GET /data</p>"
                  "<p>3. 小程序历史数据：GET /history</p>";
    server.send(200, "text/html", html);
  });
  
  // 采集端数据上传接口（POST）
  server.on("/sensor", HTTP_POST, handleSensorData);
  
  // 小程序实时数据接口（GET）
  server.on("/data", HTTP_GET, []() {
    StaticJsonDocument<300> doc;
    doc["air_temp"] = currentData.temp;
    doc["air_hum"] = currentData.hum;
    doc["soil_hum"] = currentData.soil_hum;
    doc["light_int"] = currentData.light_intensity;
    doc["light_strong"] = currentData.light_strong;
    doc["mqtt_connected"] = mqttConnected;
    doc["last_update"] = lastDataTime;
    
    String jsonStr;
    serializeJson(doc, jsonStr);
    server.send(200, "application/json", jsonStr);
  });
  
  // 小程序历史数据接口（GET）
  server.on("/history", HTTP_GET, []() {
    String historyJson = getHistoryDataJSON();
    server.send(200, "application/json", historyJson);
  });
  
  // 404页面（未定义路由）
  server.onNotFound([]() {
    server.send(404, "application/json", "{\"status\":\"error\",\"message\":\"Route not found\"}");
  });
}

// -------------------------- 发布传感器数据到MQTT服务器 --------------------------
void publishSensorData() {
  if (newDataAvailable && mqttConnected) {
    StaticJsonDocument<300> doc;
    doc["air_temp"] = currentData.temp;
    doc["air_hum"] = currentData.hum;
    doc["soil_hum"] = currentData.soil_hum;
    doc["light_int"] = currentData.light_intensity;
    doc["light_strong"] = currentData.light_strong;
    doc["timestamp"] = currentData.timestamp;
    doc["receive_time"] = currentData.receive_time;
    
    String jsonStr;
    serializeJson(doc, jsonStr);
    
    // 发布数据到MQTT主题
    if (mqttClient.publish(MQTT_TOPIC_SENSOR, jsonStr.c_str())) {
      Serial.println("MQTT发布成功：" + jsonStr);
      newDataAvailable = false; // 发布后重置标记
    } else {
      Serial.println("MQTT发布失败！检查MQTT连接");
    }
  }
}

// -------------------------- 自动控制执行器（水泵+风扇）--------------------------
void autoControlActuators() {
  if (newDataAvailable) {
    bool pumpState = actuator_control_pump(currentData.soil_hum); // 水泵自动控制
    bool fanState = actuator_control_fan(currentData.light_strong, currentData.temp); // 风扇自动控制
    
    // 发布执行器状态到MQTT
    StaticJsonDocument<100> actDoc;
    actDoc["pump"] = pumpState;
    actDoc["fan"] = fanState;
    String jsonStr;
    serializeJson(actDoc, jsonStr);
    mqttClient.publish(MQTT_TOPIC_ACT_STATUS, jsonStr.c_str());
  }
}

// -------------------------- 初始化配置 --------------------------
void setup() {
  // 初始化串口（调试用，波特率115200）
  Serial.begin(115200);
  while (!Serial) delay(10); // 等待串口就绪
  
  // 初始化模块
  initDataStorage();         // 数据存储初始化
  actuator_init();           // 执行器（水泵、风扇）初始化
  buzzer_init();             // 蜂鸣器初始化

   

  // 初始化WiFi（连接路由器）
  initWiFiSTA();
  
  // 初始化MQTT客户端
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT); // 设置MQTT服务器地址和端口
  mqttClient.setCallback(mqttCallback);         // 绑定MQTT消息回调函数
  wifiClient.setInsecure();                     // TLS关闭证书验证（避免证书错误）
  mqttClient.setKeepAlive(60);                 // MQTT心跳间隔60秒
  mqttClient.setBufferSize(512);                // 增大缓冲区（适配JSON数据）
  
  // 初始化WebServer（接收采集端数据）
  setupRoutes();
  server.begin();
  Serial.println("WebServer启动成功！访问地址: http://" + WiFi.localIP().toString());
}

// -------------------------- 主循环 --------------------------
void loop() {
  server.handleClient(); // 处理WebServer请求（采集端数据上传）
  
  // MQTT连接保持与消息处理
  if (!mqttClient.connected()) {
    connectMQTT(); // 断开后自动重连
  }
  mqttClient.loop(); // 处理MQTT消息（远程控制命令）
  
  // 核心功能执行
  publishSensorData();      // 发布数据到MQTT（供小程序读取）
  autoControlActuators();   // 自动控制水泵和风扇
  
  // 定期清理过期历史数据（每60秒一次）
  static unsigned long lastCleanup = 0;
  if (millis() - lastCleanup > 60000) {
    lastCleanup = millis();
    cleanupOldData();
  }
  
  // 定期打印系统状态（每10秒一次）
  static unsigned long lastStatusPrint = 0;
  if (millis() - lastStatusPrint > 10000) {
    lastStatusPrint = millis();
    Serial.println("\n=== 系统状态 ===");
    Serial.print("WiFi状态: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "已连接" : "未连接");
    Serial.print("MQTT状态: ");
    Serial.println(mqttConnected ? "已连接" : "未连接");
    Serial.print("历史数据条数: ");
    Serial.println(getHistoryCount());
    Serial.print("上次接收数据时间: ");
    Serial.println(millis() - lastDataTime < 30000 ? "最近30秒内" : "超过30秒");
  }
  
  delay(10); // 降低CPU占用率
}