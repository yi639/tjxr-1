#include "oled_display.h"

// 全局oled对象定义（使用u8g2库，支持中文）
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, PIN_OLED_SCL, PIN_OLED_SDA);

// oled初始化
void oled_init() {
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312); // 设置支持中文的字体
  u8g2.setCursor(0, 15);
  u8g2.print("OLED初始化中...");
  u8g2.sendBuffer();
}

// 显示简单消息（支持中文）
void oled_show_msg(const char* msg) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  u8g2.setCursor(0, 15);
  u8g2.print(msg);
  u8g2.sendBuffer();
}

// 更新oled显示数据（支持中文显示）
void oled_update_display(float soil_hum, float temp, float hum, bool light_strong, 
                          bool fan_state, bool pump_state) 
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312); // 中文显示字体
  
  // 第一行：土壤湿度
  u8g2.setCursor(0, 16);
  u8g2.print("土壤湿度:");
  u8g2.print(soil_hum, 1);
  u8g2.print("%");
  
  // 第二行：空气温湿度
  u8g2.setCursor(0, 32);
  u8g2.print("空气:");
  u8g2.print(temp, 1);
  u8g2.print("℃ ");
  u8g2.print(hum, 1);
  u8g2.print("%RH");
  
  // 第三行：光线状态+强度
  u8g2.setCursor(0, 48);
  u8g2.print("光线: ");
  u8g2.print(light_strong ? "过强" : "正常");

  
  // 第四行：设备状态
  u8g2.setCursor(0, 64);
  u8g2.print("风扇:");
  u8g2.print(fan_state ? "ON" : "OFF");
  u8g2.print(" 水泵:");
  u8g2.print(pump_state ? "ON" : "OFF");
  
  u8g2.sendBuffer();
}
