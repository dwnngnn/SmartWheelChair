#include "sensor.h"
#include "../def.h"
#include <Arduino.h>
// Đọc khoảng cách
float readDistance() {
  digitalWrite(TRIG,LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG,LOW);

  duration = pulseIn(ECHO,HIGH,30000); 
  // Tính khoảng cách = ( thời gian * tốc độ âm thanh ) / 2
  return  duration * 0.034 / 2;
}