# PROJECT: HAND GESTURE SMART ROVER
# SYSTEM ARCHITECTURE & DOCUMENTATION

## 1. FILE NAMES
* **Transmitter Code:** `Hand_Remote_ESP32.ino`
* **Receiver Code:** `Smart_Rover_ESP32.ino`

---

## 2. SYSTEM OVERVIEW
This project utilizes two ESP32 microcontrollers to create a gesture-controlled vehicle. Communication is handled via **ESP-NOW**, a wireless protocol that allows the devices to talk directly to each other without needing a Wi-Fi router, ensuring ultra-low latency.

---

## 3. TRANSMITTER (HAND REMOTE)
**Components:** ESP32, MPU6050, Push Button, RGB LED.  
**Function:** Translates hand orientation into movement commands.

* **TILT FORWARD:** Sends "FORWARD"
* **TILT BACK:** Sends "BACKWARD"
* **TILT LEFT/RIGHT:** Sends "LEFT" or "RIGHT"
* **LEVEL HAND:** Sends "STOP" (via Deadzone logic)
* **BUTTON CLICK:** Cycles through the three operational modes.

---

## 4. RECEIVER (SMART ROVER)
**Components:** ESP32, L298N Motor Driver, Ultrasonic Sensor, IR Sensors (x2), RGB LED.  
**Function:** Executes movement based on the active mode received from the remote.

* **MODE 1 (RED LED):** Manual control via Remote gestures.
* **MODE 2 (GREEN LED):** Autonomous Line Following using IR sensors.
* **MODE 3 (BLUE LED):** Autonomous Obstacle Avoidance using Ultrasonic pulses.

---

## 5. HARDWARE PINOUTS

### REMOTE (Transmitter)
| Component    | ESP32 Pin |
|--------------|-----------|
| MPU6050 SDA  | GPIO 21   |
| MPU6050 SCL  | GPIO 22   |
| Mode Button  | GPIO 32   |
| RGB Red      | GPIO 25   |
| RGB Green    | GPIO 26   |
| RGB Blue     | GPIO 27   |

### ROVER (Receiver)
| Component         | ESP32 Pin |
|-------------------|-----------|
| Motor IN1 / IN2   | GPIO 27 / 26 |
| Motor IN3 / IN4   | GPIO 33 / 32 |
| Speed ENA / ENB   | GPIO 14 / 25 |
| IR Left / Right   | GPIO 34 / 35 |
| Trig / Echo       | GPIO 4 / 36 |
| RGB R / G / B     | GPIO 18 / 19 / 21 |

---

## 6. PAIRING CONFIGURATION
To establish the wireless link, you must exchange MAC addresses in the source code:

1.  Find the **Receiver's** MAC address and paste it into the Transmitter's `receiverMAC[]` array.
2.  Find the **Transmitter's** MAC address and paste it into the Receiver's `allowedMAC[]` array.

---

## 7. LIBRARIES REQUIRED
* **Wire.h** (Built-in)
* **WiFi.h** (Built-in)
* **esp_now.h** (Built-in)
* **MPU6050.h** (By Electronic Cats / Jeff Rowberg)

---

## 8. TROUBLESHOOTING
* **No Movement:** Ensure the L298N and ESP32 share a **Common Ground (GND)**.
* **Inverted Steering:** Swap the motor wires on the L298N terminal blocks.
* **Connection Issues:** Verify that the MAC addresses in the code match the physical hardware addresses of your specific boards.
