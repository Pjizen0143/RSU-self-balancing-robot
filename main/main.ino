#include <Wire.h>
#include <MPU6050_tockn.h>
#include <PID_v1.h>
#include "motorcontrol.h"
#include "IRController.h"

#define MIN_ABS_SPEED_A 0
#define MIN_ABS_SPEED_B 0

#define IN1 8
#define IN2 9
#define ENA 6
#define ENB 5
#define IN3 13
#define IN4 12

MPU6050 mpu6050(Wire);
IRController ir(4);

double originalSetpoint = 1;
double input, output, setpoint = originalSetpoint;
double Kp = 30, Ki = 80, Kd = 0.05;
double speedOffset = 0, trunOffset = 0, realOutput;

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

motorcontrol my_motor(ENA, IN1, IN2, ENB, IN3, IN4, 1.0, 1.0);  // SpeedFactorA, SpeedFactorB

unsigned long commandTimeout = 200;  // สำหรับเข็คว่ามี IRcommand ไหมใน 0.2 วิ

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.setGyroOffsets(-1.99, 0.00, 0.00);

  my_motor.begin();

  pid.SetMode(AUTOMATIC);
  pid.SetSampleTime(10);
  pid.SetOutputLimits(-255, 255);

  ir.begin();
  Serial.println("Ready!");
  delay(2000);
}

void loop() {
  // อ่านข้อมูลจากเซนเซอร์
  mpu6050.update();
  input = mpu6050.getAngleX();

  // คำนวณค่า PID
  pid.Compute();
  
  unsigned long now = millis();
  
  if (ir.available()) {
      IRCommand cmd = ir.getCommand();
      ir.resume();
      ir.updateLastCommandTime();

      switch (cmd) {
          case IRCommand::Forward:
              // เดินหน้า
              if (setpoint < originalSetpoint + 3)
                setpoint += 0.5;
              break;

          case IRCommand::Backward:
              // ถอยหลัง 
              if (setpoint > originalSetpoint - 3)
                setpoint -= 0.5;
              break;

          case IRCommand::Left:
              // เลี้ยวซ้าย
              my_motor.turnLeft(255);
              break;

          case IRCommand::Right:
              // เลี้ยวขวา
              my_motor.turnRight(255);
              break;

          default:
              break;
      }
  }

  // กลับสู่สภาวะ Default ถ้าไม่มีคำสั่ง IR มานานเกินไป
  if (now - ir.getLastCommandTime() > commandTimeout) {
     setpoint = originalSetpoint;
  }
  my_motor.move(output, speedOffset, trunOffset, MIN_ABS_SPEED_A, MIN_ABS_SPEED_B);

  // แสดงข้อมูลใน Serial Monitor
  Serial.print(" | Angle: ");
  Serial.print(input);
  Serial.print(" | Output: ");
  Serial.print(output);
  Serial.print(" | Setpoint: ");
  Serial.print(setpoint);
  delay(10);
}
