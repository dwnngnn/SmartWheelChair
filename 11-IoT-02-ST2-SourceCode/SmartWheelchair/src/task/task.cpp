#include "task.h"
#include "../def.h"
#include "../sensor/sensor.h"
#include <Arduino.h>
static unsigned long lastSend = 0;
// Task đọc cảm biến
void sensorTask(void *parameter) {
  while (true) {
    float d = readDistance();
    // Nếu khoảng cách > 0 và < 400cm thì lưu vào biến distance
    if (d > 0 && d < 400) {
      distance = d;
    }
    // Nếu sau 1s không gửi dữ liệu thì gửi lại
    if (millis() - lastSend > 1000) {
      lastSend = millis();
    }
    // Dừng 200ms
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}
