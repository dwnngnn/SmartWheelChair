#include "src/behavior/behavior.h"
#include "src/def.h"
#include "src/motor/motor.h"
#include "src/sensor/sensor.h"
#include "src/servo/servo.h"
#include "src/task/task.h"
#include "src/buzzer/buzzer.h"

void setup() {

  Serial.begin(115200);

  // Cấu hình WiFi (Local AP)
  setupWiFi();
  
  // Khởi tạo Web Server (Local)
  setupWebServer();

  // Khởi tạo WebSocket (Local Server)
  setupWebSocket();

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // ===== Cài đặt SERVO =====
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);

  myServo.write(90);
  delay(500); 

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
  handleBuzzer(); 

  // Chạy các tác vụ server/client
  server.handleClient();
  localWebSocket.loop();

  // Heartbeat mỗi 5 giây
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 5000) {
    lastHeartbeat = millis();
    localWebSocket.broadcastTXT("PING");
  }

  // Cập nhật trạng thái định kỳ mỗi 1s
  static unsigned long lastStatusSent = 0;
  if (millis() - lastStatusSent > 1000) {
    lastStatusSent = millis();
    
    char statusMsg[64];
    snprintf(statusMsg, sizeof(statusMsg), "{\"dist\":%.2f, \"auto\":%s, \"head\":%d}", 
             distance, autoMode ? "true" : "false", headDegree);
             
    localWebSocket.broadcastTXT(statusMsg);
  }

  // Xử lý lệnh đang chờ
  if (pendingCmd != 0) {
    char cmd = pendingCmd;
    pendingCmd = 0; 
    executeCommand(cmd);
    
    if (cmd == 'F' || cmd == 'B' || cmd == 'L' || cmd == 'R') {
      delay(30);
    }
  }

  // Dừng an toàn (Timeout)
  if (!autoMode && (millis() - lastCmdTime > timeout) && (speedL != 0 || speedR != 0 || headTurnDir != 0)) {
    stopMotor();
    headTurnDir = 0;
  }

  // Xoay cổ servo
  if (headTurnDir != 0) {
    static unsigned long lastHeadTurn = 0;
    if (millis() - lastHeadTurn > 15) {
      lastHeadTurn = millis();
      headDegree += headTurnDir * 2;
      if (headDegree < 0) headDegree = 0;
      if (headDegree > 180) headDegree = 180;
      myServo.write(headDegree);
    }
  }

  // Chế độ tự hành
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