#ifndef CONFIG_RECEIVER_H
#define CONFIG_RECEIVER_H

#include <Arduino.h>
#include <ArduinoJson.h>  // 补充JSON类定义

// WiFi热点配置
#define ROUTER_SSID "wo shi nen die"   // 路由器WiFi名称
#define ROUTER_PASSWORD "woshinendie" // 路由器WiFi密码

// 服务器配置
#define SERVER_PORT 80

// ========== MQTT配置 ==========
#define MQTT_SERVER "gb54bd1e.ala.cn-hangzhou.emqxsl.cn"    // 如 "test.mosquitto.org" 或私有服务器
#define MQTT_PORT 8883                 // MQTT端口
#define MQTT_CLIENT_ID "mqttx_9142"// MQTT客户端ID
#define MQTT_USER "tjxr"                   // MQTT用户名（无则留空）
#define MQTT_PASSWORD "123456789"               // MQTT密码（无则留空）
#define MQTT_TOPIC_SENSOR "/sensor"// 发布传感器数据的主题
#define MQTT_TOPIC_CONTROL "actuator/control" // 新增：订阅控制命令的主题
#define MQTT_TOPIC_ACT_STATUS "actuator/status" // 发布执行器状态的主题


// 数据存储配置
#define MAX_HISTORY_DATA 100  // 最大存储数据条数
#define DATA_RETENTION_MS 3600000  // 数据保留时间（1小时）

// 传感器数据结构
struct SensorData {
  float temp;           // 空气温度
  float hum;            // 空气湿度
  float soil_hum;       // 土壤湿度
  float light_intensity; // 光照强度
  bool light_strong;    // 光照状态
  unsigned long timestamp; // 原始时间戳
  unsigned long receive_time; // 接收时间
};

// ========== 执行器引脚与参数（与采集端一致） ==========
#define PIN_PUMP_RELAY 26    // 水泵继电器引脚
#define PIN_FAN_RELAY 25     // 风扇继电器引脚
#define PIN_BUZZER 27        // 蜂鸣器引脚

#define SOIL_WATER_THRESHOLD 20.0f // 浇水阈值(%)
#define PUMP_WATER_TIME 10000      // 单次浇水时间(ms)
#define PUMP_INTERVAL 60000        // 水泵最小间隔(ms)
#define TEMP_FAN_THRESHOLD 30.0f   // 风扇启动温度阈值(℃)
#define TEMP_ALARM_THRESHOLD 40.0f // 温度报警阈值(℃)

// 全局变量声明（供actuator_control.cpp访问）
extern bool newDataAvailable;
extern SensorData currentData;

#define MQTT_TOPIC_ACT_STATUS "actuator/status"


#endif // CONFIG_RECEIVER_H
