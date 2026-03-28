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

int speedBase = 100;
int speedL = speedBase * 0.7;
int speedR = speedBase * 0.7;

int moveTime = 50;
bool moving = false;

volatile float distance = 100;
long duration = 0;

unsigned long lastCmdTime = 0;

int timeout = 500;

unsigned long lastSensor = 0;

int StopDistance = 30;

volatile char pendingCmd = 0;
unsigned long lastConnectTime = 0;

#include "behavior/behavior.h"
#include "motor/motor.h"
#include "servo/servo.h"

// ============ Cấu hình WiFi ============
const WifiCredential wifiList[] = {{"ngu", "12345678"},
                                   {"BA4-1017 5G", "1234567?"}};
const int wifiCount = sizeof(wifiList) / sizeof(WifiCredential);

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
      if (payloadStr == "P" || payloadStr == "PING" || payloadStr == "PONG") {
        return;
      }
      Serial.printf("[WS] Received: %s\n", payloadStr.c_str());

      if (payloadStr.startsWith("play?sound=")) {
        String filename = payloadStr.substring(11);
        filename.trim();
        currentSoundUrl = "http://222.253.80.30:1111/sounds/" + filename;
        shouldPlaySound = true;
        Serial.printf("[WS] Sound command: %s\n", filename.c_str());
        return;
      }

      // Format mới: CMD|TIMESTAMP (vd: F|1711612800000)
      int sepIdx = payloadStr.indexOf('|');
      if (sepIdx != -1) {
        char cmd = payloadStr.charAt(0);
        long long serverTs = atoll(payloadStr.substring(sepIdx + 1).c_str());
        long long localTs = getCurrentTimeMs();
        long long latency = localTs - serverTs;

        Serial.printf("[WS] Cmd: %c, Latency: %lld ms\n", cmd, latency);

        if (abs(latency) > 2000) {
          Serial.printf("[WS] Latency too high! Resetting socket...\n");
          webSocket.disconnect();
          return;
        }
        handleCommand(cmd);
      } else {
        // Fallback for old format if needed, or ignore
        char cmd = (char)payload[0];
        Serial.printf("[WS] Legacy Cmd: %c\n", cmd);
        handleCommand(cmd);
      }
    }
    break;
  }
}

// ============ Cài đặt WiFi ============
void setupWiFi() {
  Serial.println("\nScanning for WiFi...");
  int n = WiFi.scanNetworks();
  Serial.printf("%d networks found\n", n);

  for (int i = 0; i < wifiCount; i++) {
    for (int j = 0; j < n; j++) {
      if (WiFi.SSID(j) == wifiList[i].ssid) {
        Serial.printf("Connecting to priority %d: %s\n", i + 1,
                      wifiList[i].ssid);
        WiFi.begin(wifiList[i].ssid, wifiList[i].password);
        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }
        Serial.println("\nWiFi connected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        return;
      }
    }
  }

  Serial.println("No priority networks found. Retrying in 5s...");
  delay(5000);
  ESP.restart();
}

// ============ Cài đặt Thời gian ============
void setupTime() {
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Syncing time (NTP)...");
  struct tm timeinfo;
  int retry = 0;
  while (!getLocalTime(&timeinfo) && retry < 10) {
    Serial.print(".");
    delay(500);
    retry++;
  }
  if (retry < 10) {
    Serial.println("\nTime synced!");
  } else {
    Serial.println("\nTime sync failed! Check network.");
  }
}

long long getCurrentTimeMs() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)tv.tv_sec * 1000 + (tv.tv_usec / 1000);
}

// ============ Cài đặt WebSocket ============
void setupWebSocket() {
  webSocket.begin("222.253.80.30", 1111, "/ws/car");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}
