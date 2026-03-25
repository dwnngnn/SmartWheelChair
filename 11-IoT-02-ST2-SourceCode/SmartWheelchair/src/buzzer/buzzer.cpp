#include "buzzer.h"
#include "../def.h"
#include <Arduino.h>

// Xử lý còi
void handleBuzzer() {
  if (distance < 15.0) {  // Nếu khoảng cách nhỏ hơn 15cm
    digitalWrite(BUZZER_PIN, HIGH);  // Bật còi
  } else {
    digitalWrite(BUZZER_PIN, LOW);  // Tắt còi
  }
}
