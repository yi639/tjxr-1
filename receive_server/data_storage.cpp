#include "data_storage.h"

// 全局变量
static SensorData historyData[MAX_HISTORY_DATA];
static int historyIndex = 0;
static int historyCount = 0;

// 初始化数据存储
void initDataStorage() {
  for (int i = 0; i < MAX_HISTORY_DATA; i++) {
    historyData[i].timestamp = 0;
  }
  Serial.println("数据存储初始化完成");
}

// 存储传感器数据
void storeSensorData(const SensorData& data) {
    // 打印待存储数据
    Serial.println("===== 待存储数据 =====");
    Serial.print("temp: ");
    Serial.println(data.temp);
    Serial.print("soil_hum: ");
    Serial.println(data.soil_hum);
    Serial.print("timestamp: ");
    Serial.println(data.timestamp);

    historyData[historyIndex] = data;
    Serial.print("存储到索引: ");
    Serial.println(historyIndex);

    historyIndex = (historyIndex + 1) % MAX_HISTORY_DATA;
    if (historyCount < MAX_HISTORY_DATA) {
        historyCount++;
    }

    // 打印存储后状态
    Serial.print("historyIndex: ");
    Serial.println(historyIndex);
    Serial.print("historyCount: ");
    Serial.println(historyCount);
}

// 获取历史数据JSON
String getHistoryDataJSON() {
  StaticJsonDocument<4096> doc; // 根据数据量调整大小
  JsonArray dataArray = doc.createNestedArray("history");
  
  int startIndex = (historyIndex - historyCount + MAX_HISTORY_DATA) % MAX_HISTORY_DATA;
  
  for (int i = 0; i < historyCount; i++) {
    int idx = (startIndex + i) % MAX_HISTORY_DATA;
    if (historyData[idx].timestamp > 0) {
      JsonObject dataObj = dataArray.createNestedObject();
      dataObj["air_temp"] = historyData[idx].temp;
      dataObj["air_hum"] = historyData[idx].hum;
      dataObj["soil_hum"] = historyData[idx].soil_hum;
      dataObj["light_int"] = historyData[idx].light_intensity;
      dataObj["light_strong"] = historyData[idx].light_strong;
      dataObj["timestamp"] = historyData[idx].timestamp;
      dataObj["receive_time"] = historyData[idx].receive_time;
    }
  }
  
  doc["count"] = historyCount;
  
  String jsonResponse;
  serializeJson(doc, jsonResponse);
  return jsonResponse;
}

// 获取历史数据数量
int getHistoryCount() {
  return historyCount;
}

// 清理旧数据
void cleanupOldData() {
  unsigned long currentTime = millis();
  int newCount = 0;
  
  for (int i = 0; i < historyCount; i++) {
    int idx = (historyIndex - historyCount + i + MAX_HISTORY_DATA) % MAX_HISTORY_DATA;
    if (currentTime - historyData[idx].receive_time < DATA_RETENTION_MS) {
      if (i != newCount) {
        historyData[newCount] = historyData[idx];
      }
      newCount++;
    }
  }
  
  historyCount = newCount;
  Serial.print("清理后数据条数: ");
  Serial.println(historyCount);
}
