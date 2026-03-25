#include "src/behavior/behavior.h"
#include "src/def.h"
#include "src/motor/motor.h"
#include "src/sensor/sensor.h"
#include "src/servo/servo.h"
#include "src/task/task.h"
#include "src/buzzer/buzzer.h"
void setup() {

  Serial.begin(115200);

  // Kết nối WiFi + WebSocket
  setupWiFi();
  setupWebSocket();

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // ===== Cài đặt SERVO trước =====
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);

  myServo.write(90);
  delay(500); // chờ servo ổn định

  // ===== Cài đặt động cơ =====
  ledcAttach(ENA, 1000, 8);
  ledcAttach(ENB, 1000, 8);

  // LED
  pinMode(LED, OUTPUT);
  
  // Còi báo (Buzzer)
  pinMode(BUZZER_PIN, OUTPUT);


  xTaskCreatePinnedToCore(sensorTask, "Sensor Task", 4096, NULL, 2, NULL, 1);
}

void loop() {
  handleBuzzer(); // Kiểm tra khoảng cách và hú còi nếu cần

  // Vòng lặp WebSocket - nhận lệnh từ server
  webSocket.loop();

  // Cập nhật trạng thái thời gian thực qua WebSocket
  // Chỉ gửi tín hiệu qua ngõ Socket nếu có bất kì thông số nào thay đổi (Quay đầu, Đổi khoảng cách >0.5cm, Thay đổi chế độ)
  static int lastSentHead = -1;
  static float lastSentDist = -1.0;
  static bool lastSentAuto = !autoMode;

  if (lastSentHead != headDegree || abs(lastSentDist - distance) > 0.5 || lastSentAuto != autoMode) {
    lastSentHead = headDegree;
    lastSentDist = distance;
    lastSentAuto = autoMode;
    
    char statusMsg[64];
    snprintf(statusMsg, sizeof(statusMsg), "{\"dist\":%.2f, \"auto\":%s, \"head\":%d}", 
             distance, autoMode ? "true" : "false", headDegree);
    webSocket.sendTXT(statusMsg);
  }

  // Xử lý lệnh đang chờ
  if (pendingCmd != 0) {
    char cmd = pendingCmd;
    pendingCmd = 0; // Xóa lệnh sau khi lấy ra
    executeCommand(cmd);
    
    // Đảm bảo lệnh di chuyển chạy ít nhất 30ms (vượt qua điểm chết động cơ) 
    // trước khi xử lý các lệnh quá nhanh kế tiếp
    if (cmd == 'F' || cmd == 'B' || cmd == 'L' || cmd == 'R') {
      delay(30);
    }
  }

  // Dừng an toàn (Timeout): Nếu quá lâu không nhận được lệnh, tự động dừng xe & cổ servo
  // (Chỉ áp dụng khi chế độ tự động TẮT)
  if (!autoMode && (millis() - lastCmdTime > timeout) && (speedL != 0 || speedR != 0 || headTurnDir != 0)) {
    stopMotor();
    headTurnDir = 0;
  }

  // Xoay cổ servo mượt mà, liên tục
  if (headTurnDir != 0) {
    static unsigned long lastHeadTurn = 0;
    if (millis() - lastHeadTurn > 15) { // Tốc độ xoay (15ms mỗi bước)
      lastHeadTurn = millis();
      headDegree += headTurnDir * 2; // Khoảng cách mỗi bước xoay
      if (headDegree < 0) headDegree = 0;
      if (headDegree > 180) headDegree = 180;
      myServo.write(headDegree);
    }
  }

  // Chế độ tự hành (Auto mode)
  if (autoMode) {
    if (distance < StopDistance) {
      stopMotor();
      autoTurn();
    } else {
      forward();
      delay(20);
      stopMotor();
    }
  }
}