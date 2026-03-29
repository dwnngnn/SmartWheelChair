#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <WebServer.h>
#include <WiFi.h>

// Audio headers removed (Zero-Internet Mode)

extern Servo myServo;
extern WebSocketsServer localWebSocket;
extern WebSocketsClient remoteWebSocket;
extern WebServer server;

struct WifiCredential {
  const char *ssid;
  const char *password;
};

extern const WifiCredential wifiList[];
extern const int wifiCount;

#include <time.h>

// AP + WebServer + WebSocket Server + Client
void setupWiFi();
void setupTime();
void setupWebServer();
void setupWebSocket();
long long getCurrentTimeMs();
void handleCommand(char cmd);
void executeCommand(char cmd);
extern volatile char pendingCmd;
extern unsigned long lastConnectTime;
// Servo
#define SERVO_PIN 12
// HC-SR04
#define TRIG 4
#define ECHO 18
// Motor pins
#define ENA 25
#define IN1 26
#define IN2 27
#define ENB 14
#define IN3 32
#define IN4 33
// Led
#define LED 15
// Buzzer
#define BUZZER_PIN 23
// PWM channels
#define PWM_CH_L 0
#define PWM_CH_R 1
// Servo
extern int scanAngle;
extern int scanDir;
extern bool autoMode;
extern bool buzzerEnabled;
extern int degreePerTurn;
// Current head degree
extern int headDegree;
extern int headTurnDir;

// Speed for motor
extern int speedBase;
extern int speedL;
extern int speedR;

// Run time each command
extern int moveTime;
extern bool moving;

// distance from HC-SRO4 sensor, duration is time for sound wave from sensor to
// touch wall and come back (to caculator distance)
extern volatile float distance;
extern long duration;

// Time of last recieved command
extern unsigned long lastCmdTime;

// Time for waiting another command
extern int timeout;

extern unsigned long lastSensor;

// If distance Below this, Stop running
extern int StopDistance;

// Audio externs removed (Zero-Internet Mode)
void logToBrowser(String msg);