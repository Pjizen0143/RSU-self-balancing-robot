#include <Wire.h>
#include <MPU6050_tockn.h>
#include <PID_v1.h>
#include "motorcontrol.h"
#include "IRController.h"

#define MIN_ABS_SPEED 80

double deadzone = 0;

MPU6050 mpu6050(Wire);
IRController ir(4); // IR Receiver ที่ขา D4

// PID Variables
double originalSetpoint = 0.8;
double setpoint = originalSetpoint;
double targetSetpoint = originalSetpoint;

double input, output;
double Kp = 20;
double Ki = 160;
double Kd = 1;

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// Motor controller setup
double motorSpeedFactorA = 0.9;
double motorSpeedFactorB = 0.9;
int ENA = 5;
int IN1 = 13;
int IN2 = 12;
int ENB = 6;
int IN3 = 9;
int IN4 = 8;
motorcontrol my_motor(ENA, IN1, IN2, ENB, IN3, IN4, motorSpeedFactorA, motorSpeedFactorB);

// เวลา
unsigned long lastCommandTime = 0;
unsigned long commandTimeout = 200;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
//  mpu6050.setGyroOffsets(-1.36, 1.01, -1.13);
  mpu6050.setGyroOffsets(0.00, 1.34, 0.00);

  my_motor.begin();

  pid.SetMode(AUTOMATIC);
  pid.SetSampleTime(10);
  pid.SetOutputLimits(-255, 255);

  ir.begin();
  Serial.println("Ready!");
}

void loop() {
  // อ่านเซนเซอร์
  mpu6050.update();
  input = mpu6050.getAngleX();

  // PID คำนวณจาก setpoint
  pid.Compute();

  // มอเตอร์ทำงาน
  if (abs(input - setpoint) < deadzone) {
    my_motor.stop();
  } else {
    my_motor.move(output, MIN_ABS_SPEED);
  }

  // อ่าน IR
  unsigned long code = 0;
  if (ir.available(code)) {
    if (ir.forward(code)) {
      if (targetSetpoint < originalSetpoint + 5)
        targetSetpoint += 0.2;
    } else if (ir.backward(code)) {
      if (targetSetpoint > originalSetpoint - 5)
        targetSetpoint -= 0.2;
    } else if (ir.turnLeft(code)) {
      my_motor.turnLeft(MIN_ABS_SPEED, false);
    } else if (ir.turnRight(code)) {
      my_motor.turnRight(MIN_ABS_SPEED, false);
    }
    lastCommandTime = millis();
  }

  // ถ้าไม่มีคำสั่งนาน → กลับไป originalSetpoint
  if (millis() - lastCommandTime > commandTimeout) {
    targetSetpoint = originalSetpoint;
  }

  // Soft transition: ปรับ setpoint ทีละน้อย
  setpoint = 0.9 * setpoint + 0.1 * targetSetpoint;

  // Debug
  Serial.print(" | Angle: ");
  Serial.print(input);
  Serial.print(" | Output: ");
  Serial.print(output);
  Serial.print(" | Setpoint: ");
  Serial.println(setpoint);

  delay(10);
}
