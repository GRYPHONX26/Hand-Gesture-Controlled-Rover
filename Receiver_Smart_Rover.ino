#include <esp_now.h>
#include <WiFi.h>

typedef struct {
  char command[30];
} Message;

Message received;

// REPLACE WITH YOUR TRANSMITTER ESP32 MAC ADDRESS
uint8_t allowedMAC[] = {0xd4, 0xe9, 0xf4, 0x66, 0xae, 0xd0};

// ============ L298N PINS ============
const int IN1 = 27;
const int IN2 = 26;
const int IN3 = 33;
const int IN4 = 32;

const int ENA = 14;
const int ENB = 25;

// =============== SENSORS ===============
const int irLeft = 34;
const int irRight = 35;

const int trigPin = 4;
const int echoPin = 36;

// ================= LEDs =================
const int redPin = 18;
const int greenPin = 19;
const int bluePin = 21;

// =============== MODE ===============
int currentMode = 1;

// ================= LED =================
void setLED(int m){
  digitalWrite(redPin, m==1);
  digitalWrite(greenPin, m==2);
  digitalWrite(bluePin, m==3);
}

// =========== MOTOR CONTROL ============
void stopMotors(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void forward(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// ============= ULTRASONIC =============
long readDistanceCM(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if(duration == 0) return 0;

  return duration * 0.034 / 2;
}

// ============= LINE FOLLOW =============
void lineFollower(){
  int leftIR = digitalRead(irLeft);
  int rightIR = digitalRead(irRight);

  if(leftIR == LOW && rightIR == LOW){
    forward();
  }
  else if(leftIR == LOW && rightIR == HIGH){
    turnLeft();
  }
  else if(leftIR == HIGH && rightIR == LOW){
    turnRight();
  }
  else{
    stopMotors();
  }
}

// =========== OBSTACLE AVOID ===========
void obstacleAvoid(){
  long distance = readDistanceCM();

  if(distance == 0 || distance > 20){
    forward();
  } else {
    stopMotors();
    delay(150);
    backward();
    delay(250);
    stopMotors();
    delay(100);
    turnRight();
    delay(350);
    stopMotors();
    delay(100);
  }
}

// =========== MANUAL CONTROL ==========
void manualControl(String cmd){
  if(cmd == "FORWARD") forward();
  else if(cmd == "BACKWARD") backward();
  else if(cmd == "LEFT") turnLeft();
  else if(cmd == "RIGHT") turnRight();
  else stopMotors();
}

// ============== RECEIVE ==============
void onReceive(const esp_now_recv_info *info, const uint8_t *data, int len){
  if(memcmp(info->src_addr, allowedMAC, 6) != 0){
    return; 
  }

  memcpy(&received, data, sizeof(received));
  String cmd = String(received.command);

  if(cmd == "MODE1"){
    currentMode = 1;
    setLED(1);
    stopMotors();
  }
  else if(cmd == "MODE2"){
    currentMode = 2;
    setLED(2);
    stopMotors();
  }
  else if(cmd == "MODE3"){
    currentMode = 3;
    setLED(3);
    stopMotors();
  }

  if(currentMode == 1){
    manualControl(cmd);
  }
}

// =============== SETUP ===============
void setup(){
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onReceive);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(irLeft, INPUT);
  pinMode(irRight, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  stopMotors();
  setLED(1);
}

// =============== LOOP ===============
void loop(){
  if(currentMode == 2){
    lineFollower();
  }
  else if(currentMode == 3){
    obstacleAvoid();
  }
}
