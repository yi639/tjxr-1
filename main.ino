#include <Arduino.h>
// 引入模块头文件（包含wifi_client.h，使用其initWiFi函数）
#include "config.h"
#include "oled_display.h"
#include "dht11_sensor.h"
#include "soil_sensor.h"
#include "light_sensor.h"
// #include "actuator_control.h"  // 未启用执行器，保留注释
#include "buzzer_alarm.h"
#include "wifi_client.h"  // 关键：引入wifi_client.h，使用其initWiFi函数
#include "data_sensor.h"
#include "data_transmit.h"
#include <WiFiClient.h>
#include <U8g2lib.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

/******************************************* WIFI端配置信息 ************************************************/
const char* WIFI_SSID = "wo shi nen die";  
const char* WIFI_PASS = "woshinendie";       
const char* SERVER_IP = "192.168.160.67";    // 接收端IP
const uint16_t SERVER_PORT = 80;            // 接收端端口

/******************************************* 全局变量 ************************************************/
WiFiClient client;
unsigned long lastSendTime = 0;
unsigned long lastStatusDisplayTime = 0;

/******************************************* setup函数（调用wifi_client.h的initWiFi） ************************************************/
void setup() {
  // 初始化串口
  Serial.begin(115200);
  while (!Serial) delay(10);
  
  // 初始化硬件模块
  oled_init();
  sensor_init();
  buzzer_init();
  // actuator_init();

  // 系统启动提示
  Serial.println("系统初始化完成!");
  oled_show_msg("OLED启动成功!");
  delay(1000);
  
  // 关键：调用wifi_client.h中定义的initWiFi函数（无重复）
  initWiFi(WIFI_SSID, WIFI_PASS);
}

/******************************************* loop函数（逻辑不变） ************************************************/
void loop() {
  // 1. 读取传感器数据
  SensorData data = readSensorData();
  
  // 2. 蜂鸣器报警
  if (data.light_strong) {
    buzzer_beep_short();
    Serial.println("警告：光照强度过强！");
  } else {
    digitalWrite(PIN_BUZZER, HIGH);
  }
  if (data.temp > TEMP_ALARM_THRESHOLD) {
    buzzer_beep_temp();
    Serial.printf("警告：温度超限！当前%.1f℃，阈值%.1f℃\n", data.temp, TEMP_ALARM_THRESHOLD);
  }
  
  // 3. OLED显示
  oled_update_display(
    data.soil_hum, data.temp, data.hum,
    data.light_strong, false, false
  );
  
  // 4. WiFi重连（调用wifi_client.h的函数）
  checkAndReconnectWiFi(WIFI_SSID, WIFI_PASS);
  
  // 5. 定期发送数据
  unsigned long currentTime = millis();
  if (WiFi.status() == WL_CONNECTED) {
    if (currentTime - lastSendTime >= 5000) {
      lastSendTime = currentTime;
      if (sendDataToServer(client, SERVER_IP, SERVER_PORT, data)) {
        Serial.println("数据发送成功!");
      } else {
        Serial.println("数据发送失败!");
      }
    }
  } else {
    // WiFi未连接时显示状态
    if (currentTime - lastStatusDisplayTime >= 5000) {
      lastStatusDisplayTime = currentTime;
      String status = getWiFiStatusString();
      Serial.println("WiFi状态: " + status);
      oled_show_msg(("WiFi: " + status).c_str());
      delay(500);
      oled_update_display(data.soil_hum, data.temp, data.hum, data.light_strong, false, false);
    }
  }
  
  delay(LOOP_DELAY);
}