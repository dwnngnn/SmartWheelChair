#include "behavior.h"
#include "../def.h"
#include "../motor/motor.h"
#include "../servo/servo.h"
#include <Arduino.h>
// Hàm quét trong khi di chuyển
void scanWhileMove() {
  // Thay đổi góc quét
  scanAngle += scanDir * 3;
  if (scanAngle > 160) {  // Góc quét tối đa
    scanAngle = 160;
    scanDir = -1;
  }
  if (scanAngle < 20) {  // Góc quét tối thiểu
    scanAngle = 20;
    scanDir = 1;
  }
  // Điều khiển động cơ servo
  myServo.write(scanAngle);
  float d = distance;
  if (d < 30) {  // Nếu khoảng cách nhỏ hơn 30cm
    if (scanAngle < 90) {
      // Vật bên phải → lệch trái
      speedL = speedBase;
      speedR = speedBase * 0.5;
    } else {
      // Vật bên trái → lệch phải
      speedL = speedBase * 0.5;
      speedR = speedBase;
    }
  } else {  // Không có vật cản → đi thẳng
    speedL = speedBase;
    speedR = speedBase;
  }
  applySpeed();
}
// Hàm tự động quay đầu khi gặp vật cản
void autoTurn() {
  float leftDist;
  float rightDist;
  // Quét sang trái
  moveServoTo(170);
  delay(300);
  leftDist = distance;
  // Quét sang phải
  moveServoTo(20);
  delay(300);
  rightDist = distance;
  // Trả về chính giữa
  moveServoTo(90);
  delay(200);
  // Lùi lại
  backward();
  delay(200);
  // Chọn hướng tốt nhất
  if (leftDist > rightDist) {
    // Rẽ trái
    turnLeft();
    int okCount = 0;
    // Đợi đến khi khoảng cách lớn hơn 50cm
    while (okCount < 3) {
      float d = distance;
      if (d > StopDistance) {
        okCount++;
      } else {
        okCount = 0;
      }
      delay(20);
    }
  } else {
    // Rẽ phải
    turnRight();
    int okCount = 0;
    // Đợi đến khi khoảng cách lớn hơn 50cm
    while (okCount < 3) {

      float d = distance;

      if (d > StopDistance) {
        okCount++;
      } else {
        okCount = 0;
      }
      delay(20);
    }
  }
  // Dừng lại
  stopMotor();
}
