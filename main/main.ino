#include <Wire.h>
#include <MPU6050_tockn.h>
#include <PID_v1.h>
#include "motorcontrol.h"
#include "IRController.h"

#define MIN_ABS_SPEED 40

MPU6050 mpu6050(Wire);
IRController ir(4); // IR Receiver ที่ขา D4

// PID Variables
double originalSetpoint = -4;
double setpoint = originalSetpoint;
double input, output;
double Kp = 60;
double Ki = 400; //350
double Kd = 2.2;

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// Motor controller setup
double motorSpeedFactorLeft = 0.79;
double motorSpeedFactorRight = 0.9;
int ENA = 7;
int IN1 = 8;
int IN2 = 9;
int IN4 = 13;
int IN3 = 12;
int ENB = 10;
motorcontrol my_motor(ENA, IN1, IN2, ENB, IN3, IN4, motorSpeedFactorLeft, motorSpeedFactorRight);

// เก็บเวลาล่าสุดที่ปุ่มถูกกด
unsigned long lastCommandTime = 0;
unsigned long commandTimeout = 200; // 200ms ถือว่าไม่ได้กดแล้ว

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.setGyroOffsets(-1.51, 1.00, -1.50); // จับ offset อัตโนมัติ

  my_motor.begin();

  pid.SetMode(AUTOMATIC);
  pid.SetSampleTime(10);
  pid.SetOutputLimits(-255, 255);

  ir.begin(); // เริ่มต้น IR Receiver
  Serial.println("Ready!");
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
      setpoint = originalSetpoint+1.15;
      lastCommandTime = millis();
    } else if (ir.backward(code)) {
      setpoint = originalSetpoint-3;
      lastCommandTime = millis();
    }
  }

  // เช็คว่าผ่านไปนานแค่ไหน ถ้าไม่มีปุ่มถูกกดค้างไว้ ให้ setpoint = 0
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
