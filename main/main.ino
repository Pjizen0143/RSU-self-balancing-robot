#include "motor_control.h"
#include <MPU6050_tockn.h>
#include <Wire.h>

const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;
const int ENA = 6;
const int ENB = 7;

MotorControl mymotor(IN1, IN2, IN3, IN4, ENA, ENB);
MPU6050 mpu6050(Wire);

float kp = 20; //การตอบสนองการเอียง
float ki = 0; //ความเร็วในการกลับมาตรง
float kd = 1; //เพิ่มความสเถียร
float setpoint = 0;
float error, lastError, integral, derivative, output;

float currentPitchAngle;

void setup() {
  Serial.begin(9600);
  mymotor.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  Serial.println("Motor Control System Initialized");
}

void loop() {
  mpu6050.update();
  currentPitchAngle = mpu6050.getAngleX();

  pidControl(currentPitchAngle);

  mymotor.driveMotor(output);

  delay(10);
}

void pidControl(float pitch) {
  error = setpoint - pitch;
  integral += error;
  derivative = error - lastError;
  output = (kp * error) + (ki * integral) + (kd * derivative);
  lastError = error;
}
