#include <Wire.h>
#include <MPU6050_tockn.h>
#include <PID_v1.h>
#include "motorcontrol.h"
#include "IRController.h"

#define MIN_ABS_SPEED 0

#define IN1 8
#define IN2 9
#define ENA 6
#define ENB 5
#define IN3 13
#define IN4 12

MPU6050 mpu6050(Wire);
IRController ir(4);

double input, output, setpoint = 0;
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
}

void loop() {
  // อ่านข้อมูลจากเซนเซอร์
  mpu6050.update();
  input = mpu6050.getAngleX();

  // คำนวณค่า PID
  pid.Compute();
  realOutput = output;
  
  unsigned long now = millis();
  
  if (ir.available()) {
      IRCommand cmd = ir.getCommand();
      ir.resume();
      ir.updateLastCommandTime();

      switch (cmd) {
          case IRCommand::Forward:
              // เดินหน้า
              if (speedOffset < 50)
                speedOffset += 1;
              break;

          case IRCommand::Backward:
               if (speedOffset > -50)
                speedOffset -= 1;
              break;

          case IRCommand::Left:
              // เลี้ยวซ้าย
              if (speedOffset < 50)
                speedOffset += 1;
              trunOffset = 50;
              break;

          case IRCommand::Right:
              // เลี้ยวขวา
              if (speedOffset < 50)
                speedOffset += 1;
              trunOffset = -50;
              break;

          default:
              break;
      }
  }

  // กลับสู่สภาวะ Default ถ้าไม่มีคำสั่ง IR มานานเกินไป
  if (now - ir.getLastCommandTime() > commandTimeout) {
     speedOffset = 0;
     trunOffset = 0;
  }
  my_motor.move(output, speedOffset, trunOffset, MIN_ABS_SPEED);

  // แสดงข้อมูลใน Serial Monitor
  Serial.print(" | Angle: ");
  Serial.print(input);
  Serial.print(" | Output: ");
  Serial.print(output);
  Serial.print(" | speedOffset: ");
  Serial.print(speedOffset);
  Serial.print(" | trunOffset: ");
  Serial.print(trunOffset);


  delay(10);
}
