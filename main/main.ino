#include <Wire.h>
#include <MPU6050_tockn.h>
#include <PID_v1.h>
#include "motorcontrol.h"
#include "IRController.h"
//#include "display.h"

#define MIN_ABS_SPEED 30

double deadzone = 0.2;
bool isFarFromSetpoint = false;

MPU6050 mpu6050(Wire);
IRController ir(4); // IR Receiver ที่ขา D4

// PID Variables
double originalSetpoint = 0.42;
double setpoint = originalSetpoint;
double input, output;
double Kp = 18;//45
double Ki = 75; //120
double Kd = 0.68;// 1

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// Motor controller setup

double motorSpeedFactorA = 1.22; //1
double motorSpeedFactorB = 2; //2
int ENA = 10;
int IN1 = 8;
int IN2 = 9;
int ENB = 7;
int IN3 = 12;
int IN4 = 13;
motorcontrol my_motor(ENA, IN1, IN2, ENB, IN3, IN4, motorSpeedFactorA, motorSpeedFactorB);

// เก็บเวลาล่าสุดที่ปุ่มถูกกด
unsigned long lastCommandTime = 0;
unsigned long commandTimeout = 200; // 200ms ถือว่าไม่ได้กดแล้ว

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.setGyroOffsets(-1.14, 1.02, -1.58); // จับ offset อัตโนมัติ

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

   if (!isFarFromSetpoint && abs(input - setpoint) > 20) {
    pid.SetTunings(Kp, 0, Kd);
    isFarFromSetpoint = true;
  }

  // คืนค่า Ki เมื่อกลับมาใกล้
  if (isFarFromSetpoint && abs(input - setpoint) <= 5) {
    pid.SetTunings(Kp, Ki, Kd);
    isFarFromSetpoint = false;
  }
  
  // Deadzone threshold (องศา)
  if (abs(input - setpoint) < deadzone) {
  my_motor.stop();  // หยุดมอเตอร์จริง
  } else {
  my_motor.move(-output, MIN_ABS_SPEED);  // ขยับตามปกติ
  } 

  // อ่านค่าจาก IR
  unsigned long code = 0;
  if (ir.available(code)) {
    if (ir.forward(code)) {
      if (setpoint < originalSetpoint+5)
            setpoint += 0.2;
    } else if (ir.backward(code)) {;
        if (setpoint > originalSetpoint-5)
            setpoint -= 0.2;
    } else if (ir.turnLeft(code)){
      my_motor.turnLeft(MIN_ABS_SPEED, false);
    } else if (ir.turnRight(code)){
      my_motor.turnRight(MIN_ABS_SPEED, false);
    }
    lastCommandTime = millis();
  }

  // ถ้าไม่มีคำสั่งนานเกิน timeout → reset
  if (millis() - lastCommandTime > commandTimeout) {
    setpoint = originalSetpoint;
  }


  // Debug
  Serial.print(" | Angle: ");
  Serial.print(input);
  Serial.print(" | Output: ");
  Serial.print(output);
  Serial.print(" | Setpoint: ");
  Serial.println(setpoint);

  delay(10); // PID sample time

 
}
