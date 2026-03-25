#include "motor.h"
#include "../def.h"
#include <Arduino.h>
// Áp dụng tốc độ
void applySpeed() {
  ledcWrite(ENA, speedL);
  ledcWrite(ENB, speedR);
}
// Tiến
void forward() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  speedL = speedBase;
  speedR = speedBase;

  applySpeed();
}
// Lùi
void backward() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  speedL = speedBase;
  speedR = speedBase;

  applySpeed();
}
// Rẽ trái
void turnLeft() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  speedL = speedBase;
  speedR = speedBase;

  applySpeed();
}
// Rẽ phải
void turnRight() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  speedL = speedBase;
  speedR = speedBase;

  applySpeed();
}
// Dừng
void stopMotor() {
  speedL = 0;
  speedR = 0;

  applySpeed();
}