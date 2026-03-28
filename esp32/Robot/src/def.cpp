#include "def.h"

Servo myServo;

AudioGeneratorMP3 *mp3 = NULL;
AudioFileSourceICYStream *file = NULL;
AudioOutputI2S *out = NULL;
String currentSoundUrl = "";
bool shouldPlaySound = false;

SemaphoreHandle_t motorMutex = NULL;
SemaphoreHandle_t distanceMutex = NULL;

int scanAngle = 90;
int scanDir = 1;
bool autoMode = false;
int degreePerTurn = 10;

int headDegree = 90;
int headTurnDir = 0;

int speedBase = 200;
int speedL = speedBase * 0.7;
int speedR = speedBase * 0.7;

int moveTime = 50;
bool moving = false;

volatile float distance = 100;
long duration = 0;

unsigned long lastCmdTime = 0;

int timeout = 100;

unsigned long lastSensor = 0;

int StopDistance = 30;

volatile char pendingCmd = 0;
unsigned long lastConnectTime = 0;

#include "behavior/behavior.h"
#include "motor/motor.h"
#include "servo/servo.h"

// ============ Cấu hình WiFi ============
const char *ssid = "BA4-1017 5G";
const char *password = "1234567?";

WebSocketsClient webSocket;

// ============ Xử lý lệnh ============
// Lưu lệnh mới nhất vào buffer
void handleCommand(char cmd) { pendingCmd = cmd; }

// Thực thi lệnh (gọi trong vòng lặp)
void executeCommand(char cmd) {
  if (cmd != 'P') {
    lastCmdTime = millis();
  }
  switch (cmd) {
  case 'F':
    if (distance > StopDistance) {
      forward();
    } else {
      stopMotor(); // Nếu khoảng cách quá gần, không cho chạy và tự ép dừng
    }
    break;
  case 'B':
    backward();
    break;
  case 'L':
    turnLeft();
    break;
  case 'R':
    turnRight();
    break;
  case 'S':
    stopMotor();
    headTurnDir = 0;
    break;
  case 'C':
    headTurnDir = 5;
    break;
  case 'D':
    headTurnDir = -5;
    break;
  case '1':
    autoTurn();
    break;
  case 'V':
    autoMode = true;
    break;
  case 'v':
    autoMode = false;
    stopMotor();
    break;
  case 'P':
    webSocket.sendTXT("PONG");
    break;
  case 'I':
    digitalWrite(LED, !digitalRead(LED));
    break;
  case 'Z':
    ESP.restart();
    break;
  }
}

// ============ Sự kiện WebSocket ============
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
  case WStype_DISCONNECTED:
    Serial.println("[WS] Disconnected!");
    pendingCmd = 0; // Xóa lệnh chờ khi mất kết nối
    break;
  case WStype_CONNECTED:
    Serial.printf("[WS] Connected to: %s\n", payload);
    lastConnectTime = millis();
    pendingCmd = 0; // Xóa lệnh chờ khi kết nối lại
    webSocket.sendTXT("ESP32 connected");
    break;
  case WStype_TEXT:
    if (length > 0) {
      // Bỏ qua các lệnh nhận được trong 500ms đầu tiên (tránh lệnh cũ)
      if (millis() - lastConnectTime < 500) {
        Serial.println(
            "[WS] Data received too soon after connect, ignoring...");
        return;
      }

      String payloadStr = (char *)payload;
      if (payloadStr.startsWith("play?sound=")) {
        String filename = payloadStr.substring(11);
        filename.trim();
        currentSoundUrl = "http://222.253.80.30:1111/sounds/" + filename;
        shouldPlaySound = true;
        return;
      }

      char cmd = (char)payload[0];
      handleCommand(cmd);
    }
    break;
  }
}

// ============ Cài đặt WiFi ============
void setupWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ============ Cài đặt WebSocket ============
void setupWebSocket() {
  webSocket.begin("222.253.80.30", 1111, "/ws/car");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}
