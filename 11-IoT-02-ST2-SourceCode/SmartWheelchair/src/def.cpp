#include "def.h"
#include "webpage.h"

Servo myServo;

// Audio variables removed (Zero-Internet Mode)

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

#include "behavior/behavior.h"
#include "motor/motor.h"
#include "servo/servo.h"

// ============ Cấu hình WiFi ============
const WifiCredential wifiList[] = {{"BA41017 5G", "1234567?"}};
const int wifiCount = sizeof(wifiList) / sizeof(WifiCredential);

WebSocketsServer localWebSocket = WebSocketsServer(81);
WebSocketsClient remoteWebSocket;
WebServer server(80);

// ============ Xử lý lệnh ============
void handleCommand(char cmd) { pendingCmd = cmd; }

void executeCommand(char cmd) {
  if (cmd != 'P') {
    lastCmdTime = millis();
  }
  switch (cmd) {
  case 'F':
    if (distance > StopDistance) {
      forward();
    } else {
      stopMotor();
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
    localWebSocket.broadcastTXT("PONG");
    break;
  case 'I':
    digitalWrite(LED, !digitalRead(LED));
    break;
  case 'Z':
    ESP.restart();
    break;
  }
}

// Logic xử lý lệnh chung cho cả local và remote
void processRawCommand(String payloadStr, bool isRemote) {
  if (payloadStr == "P" || payloadStr == "PING" || payloadStr == "PONG") {
    return;
  }

  if (payloadStr.startsWith("{") && payloadStr.endsWith("}")) {
    return;
  }

  if (payloadStr.startsWith("play?sound=")) {
    Serial.println("[WS] Sound command received (Disabled in Local Mode)");
    return;
  }

  int sepIdx = payloadStr.indexOf('|');
  if (sepIdx != -1) {
    char cmd = payloadStr.charAt(0);
    handleCommand(cmd);
  } else {
    char cmd = payloadStr.charAt(0);
    handleCommand(cmd);
  }
}

// ============ Sự kiện localWebSocket (Server) ============
void localWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                         size_t length) {
  switch (type) {
  case WStype_DISCONNECTED:
    Serial.printf("[Local:%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED: {
    IPAddress ip = localWebSocket.remoteIP(num);
    Serial.printf("[Local:%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1],
                  ip[2], ip[3]);
    localWebSocket.sendTXT(num, "ESP32 connected");
    break;
  }
  case WStype_TEXT:
    if (length > 0) {
      processRawCommand((char *)payload, false);
    }
    break;
  }
}

// ============ Sự kiện remoteWebSocket (Client) ============
void remoteWebSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
  case WStype_DISCONNECTED:
    Serial.println("[Remote] Disconnected!");
    break;
  case WStype_CONNECTED:
    Serial.printf("[Remote] Connected to server: %s\n", payload);
    remoteWebSocket.sendTXT("ESP32 connected");
    break;
  case WStype_TEXT:
    if (length > 0) {
      processRawCommand((char *)payload, true);
    }
    break;
  }
}

// ============ Cài đặt WiFi ============
void setupWiFi() {
  Serial.println("\nSetting up Local AP...");

  WiFi.disconnect(true); 
  delay(100);
  WiFi.mode(WIFI_AP);

  // Cấu hình IP tĩnh cho AP
  IPAddress local_IP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_IP, gateway, subnet);

  WiFi.softAP("SmartWheelChair", "12345678");
  Serial.println("Access Point Started: SmartWheelChair");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("-------------------------");
}

// ============ Cài đặt Thời gian ============
void setupTime() {
  // NTP time sync disabled in local AP mode
}

long long getCurrentTimeMs() {
  return millis();
}

// ============ Cài đặt Web Server ============
void setupWebServer() {
  server.on("/", []() { server.send(200, "text/html", index_html); });
  server.on("/style.css", []() { server.send(200, "text/css", style_css); });
  server.on("/script.js",
            []() { server.send(200, "application/javascript", script_js); });
  server.on("/api/status", []() {
    char statusMsg[64];
    snprintf(statusMsg, sizeof(statusMsg),
             "{\"dist\":%.2f, \"auto\":%s, \"head\":%d}", distance,
             autoMode ? "true" : "false", headDegree);
    server.send(200, "application/json", statusMsg);
  });
  server.begin();
  Serial.println("HTTP Web Server Started on Port 80");
}

// ============ Cài đặt WebSocket ============
void setupWebSocket() {
  // Local Server
  localWebSocket.begin();
  localWebSocket.onEvent(localWebSocketEvent);

  Serial.println("WebSocket Local Server Started on Port 81");
}
