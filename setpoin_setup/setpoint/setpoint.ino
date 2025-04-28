#include <Wire.h>
#include <MPU6050_tockn.h>
#include <PID_v1.h>
#include "motorcontrol.h"
#include "IRController.h"
#include <EEPROM.h>  // <-- เพิ่มมา

#define MIN_ABS_SPEED 40

MPU6050 mpu6050(Wire);
IRController ir(4); // IR Receiver ที่ขา D4

// PID Variables
double originalSetpoint = -3.7;
double setpoint = originalSetpoint;
double input, output;
double Kp = 40;
double Ki = 400; //350
double Kd = 2.5;

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// Motor controller setup
double motorSpeedFactorA = 0.57; // 0.55
double motorSpeedFactorB = 0.82;
int ENA = 7;
int IN1 = 8;
int IN2 = 9;
int IN4 = 13;
int IN3 = 12;
int ENB = 10;
motorcontrol my_motor(ENA, IN1, IN2, ENB, IN3, IN4, motorSpeedFactorA, motorSpeedFactorB);

// เก็บเวลาล่าสุดที่ปุ่มถูกกด
unsigned long lastCommandTime = 0;
unsigned long commandTimeout = 200; // 200ms ถือว่าไม่ได้กดแล้ว

void saveSetpointToEEPROM(double value) {
  EEPROM.put(0, value);  // เขียนที่ address 0
}

double loadSetpointFromEEPROM() {
  double value;
  EEPROM.get(0, value);  // อ่านจาก address 0
  // เช็คว่าค่าอ่านได้สมเหตุสมผลไหม (กันค่าเน่าๆจาก EEPROM ใหม่)
  if (value < -90 || value > 90) { 
    value = originalSetpoint; // ถ้าเพี้ยนเกินไป ให้ใช้ค่า default
  }
  return value;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.setGyroOffsets(-1.40, 1.00, -1.20); // จับ offset อัตโนมัติ

  my_motor.begin();

  pid.SetMode(AUTOMATIC);
  pid.SetSampleTime(10);
  pid.SetOutputLimits(-255, 255);

  ir.begin(); // เริ่มต้น IR Receiver

  // โหลดค่า setpoint ล่าสุดจาก EEPROM
  originalSetpoint = loadSetpointFromEEPROM();
  setpoint = originalSetpoint;

  Serial.println("Ready!");
  Serial.print("Loaded Setpoint: ");
  Serial.println(originalSetpoint);
}

void loop() {
  // อัปเดต MPU6050 และ PID
  mpu6050.update();
  input = mpu6050.getAngleX();
  pid.Compute();

  // อ่านค่าจาก IR
  unsigned long code = 0;
  if (ir.available(code)) {
    if (ir.forward(code)) {
      originalSetpoint = originalSetpoint + 0.05;
      setpoint = originalSetpoint;
      saveSetpointToEEPROM(originalSetpoint);  // <-- เซฟ
      lastCommandTime = millis();
    } else if (ir.backward(code)) {
      originalSetpoint = originalSetpoint - 0.05;
      setpoint = originalSetpoint;
      saveSetpointToEEPROM(originalSetpoint);  // <-- เซฟ
      lastCommandTime = millis();
    }
  }

  // เช็คว่าผ่านไปนานแค่ไหน ถ้าไม่มีปุ่มถูกกดค้างไว้ ให้ setpoint = originalSetpoint
  if (millis() - lastCommandTime > commandTimeout) {
    setpoint = originalSetpoint;
  }

  // ควบคุมมอเตอร์ให้บาลานซ์
  my_motor.move(output, MIN_ABS_SPEED);

  // Debug
  Serial.print("Angle: ");
  Serial.print(input);
  Serial.print(" | Output: ");
  Serial.print(output);
  Serial.print(" | Setpoint: ");
  Serial.println(setpoint);

  delay(10); // PID sample time
}
