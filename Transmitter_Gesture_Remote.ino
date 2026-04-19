#include <Wire.h>
#include <MPU6050.h>
#include <esp_now.h>
#include <WiFi.h>

MPU6050 mpu;

// REPLACE WITH YOUR RECEIVER ESP32 MAC ADDRESS
uint8_t receiverMAC[] = {0x6c, 0xc8, 0x40, 0x4f, 0x78, 0x2C};

typedef struct {
  char command[30];
} Message;

Message msg;

// =============== PINS ===============
const int buttonPin = 32;

const int redPin = 25;   // Mode 1
const int greenPin = 26; // Mode 2
const int bluePin = 27;  // Mode 3

// =============== MODE ===============
int mode = 1;
int lastButtonState = HIGH;

// =========== MPU SENSITIVITY ===========
int threshold = 7000;
int deadZone = 3000;

// ========== SEND CONTROL FIX ===========
unsigned long lastSendTime = 0;

// ================= LED CONTROL =================
void setLED(int m) {
  digitalWrite(redPin,   m == 1);
  digitalWrite(greenPin, m == 2);
  digitalWrite(bluePin,  m == 3);
}

// ======== SEND COMMAND (FIXED) ========
void sendCommand(String command) {
  if (millis() - lastSendTime < 100) return; 
  lastSendTime = millis();

  command.toCharArray(msg.command, sizeof(msg.command));
  esp_now_send(receiverMAC, (uint8_t *)&msg, sizeof(msg));
}

// =============== SETUP ===============
void setup() {
  Wire.begin(21, 22);
  mpu.initialize();

  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  setLED(mode);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    return;
  }

  sendCommand("MODE1");
}

// ============== LOOP ==============
void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && lastButtonState == HIGH) {
    mode++;
    if (mode > 3) mode = 1;

    setLED(mode);

    if (mode == 1)      sendCommand("MODE1");
    else if (mode == 2) sendCommand("MODE2");
    else if (mode == 3) sendCommand("MODE3");

    delay(300); // debounce
  }
  lastButtonState = buttonState;

  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  if (mode == 1) {
    if (abs(ax) < deadZone && abs(ay) < deadZone) {
      sendCommand("STOP");
    }
    else if (ax > threshold) {
      sendCommand("FORWARD");
    }
    else if (ax < -threshold) {
      sendCommand("BACKWARD");
    }
    else if (ay > threshold) {
      sendCommand("RIGHT");
    }
    else if (ay < -threshold) {
      sendCommand("LEFT");
    }
  }
  delay(10);
}
