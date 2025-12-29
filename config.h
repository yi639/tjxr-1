#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

// 引脚定义（ESP32-WROOM-32 38Pin）
#define PIN_SOIL_AO       34    // 土壤湿度模拟引脚（ADC1_CH6）
#define PIN_LIGHT_AO      35    // 光敏电阻模拟引脚（ADC1_CH7）
#define PIN_LIGHT_DO      18    // 光敏电阻数字引脚（GPIO18）
#define PIN_BUZZER        27    // 蜂鸣器引脚（GPIO27）
#define PIN_PUMP_RELAY    26    // 水泵继电器引脚（GPIO26）
#define PIN_FAN_RELAY     25    // 风扇继电器引脚（GPIO25）
#define PIN_DHT11_DATA    4     // DHT11数据引脚（GPIO4）
#define PIN_OLED_SDA      22    // OLED SDA引脚（GPIO22）
#define PIN_OLED_SCL      21    // OLED SCL引脚（GPIO21）

// OLED配置（核心保留）
#define OLED_WIDTH        128   // OLED宽度
#define OLED_HEIGHT       64    // OLED高度
#define OLED_RESET        -1    // OLED重置引脚（无）
#define OLED_ADDR         0x3C  // OLED I2C地址（默认）

// DHT11配置（保留注释）
#define DHT_TYPE          DHT11 // DHT传感器类型

// 校准参数（保留注释）
#define TEMP_ALARM_THRESHOLD  40.0f // 温度报警阈值(℃)，超过则蜂鸣器报警
#define SOIL_DRY_VOLTAGE  3.0f  // 土壤干燥电压
#define SOIL_WET_VOLTAGE  0.8f  // 土壤湿润电压
#define SOIL_WATER_THRESHOLD 20.0f // 浇水阈值(%)
#define PUMP_WATER_TIME   10000 // 单次浇水时间(ms)
#define PUMP_INTERVAL     60000 // 水泵最小间隔(ms)
#define LIGHT_FAN_STATE   LOW   // 光线过强时DO输出状态（LOW=过强）
#define LIGHT_STRONG_THRESHOLD 80.0f  // 光照过强阈值（%），超过此值触发报警
#define TEMP_FAN_THRESHOLD 30.0f // 风扇启动温度阈值(℃)
#define LOOP_DELAY        1000  // 主循环延时(ms)

// ADC配置（保留注释）
#define ADC_WIDTH         12    // ADC分辨率
#define ADC_ATTEN         ADC_11db // ADC衰减（0~3.6V）
#define RECEIVER_API "/sensor"            // 接收端数据接口
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;
extern const char* SERVER_IP;
extern const uint16_t SERVER_PORT;


#endif // CONFIG_H