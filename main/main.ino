#include <Wire.h>
#include <MPU6050_tockn.h>
#include <PID_v1.h>
#include "motorcontrol.h"
#include "IRController.h"


#define IN1 8
#define IN2 9
#define ENA 6
#define ENB 5
#define IN4 11
#define IN3 10

#define LED 7

int MIN_ABS_SPEED_A = 120;
int MIN_ABS_SPEED_B = 180;

MPU6050 mpu6050(Wire);
IRController ir(4);

double originalSetpoint = 0;
double input, output, setpoint = originalSetpoint;
double Kp = 30, Ki = 120, Kd = 0.4;
double speedOffset = 0, trunOffset = 0;

double boot = 150;
double bootLR = 110;

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

motorcontrol my_motor(ENA, IN1, IN2, ENB, IN3, IN4, 0.95, 1.2);  // SpeedFactorA, SpeedFactorB

unsigned long commandTimeout = 200;  // สำหรับเข็คว่ามี IRcommand ไหมใน 0.2 วิ

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.setGyroOffsets(-1.99, 0.00, 0.00);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  my_motor.begin();

  pid.SetMode(AUTOMATIC);
  pid.SetSampleTime(10);
  pid.SetOutputLimits(-255, 255);

  ir.begin();
  Serial.println("Ready!");
  digitalWrite(LED, HIGH);
  delay(800);
  digitalWrite(LED, LOW);
  delay(400);
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW);
  delay(200);
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
              if (setpoint < originalSetpoint + 4)
                setpoint += 1.5;
              digitalWrite(LED, HIGH);
              break;

          case IRCommand::Backward:
              // ถอยหลัง
              if (setpoint > originalSetpoint - 4)
                setpoint -= 1.5;
              digitalWrite(LED, HIGH);
              break;

          case IRCommand::Left:
              // เลี้ยวซ้าย
              speedOffset = bootLR;
              trunOffset = -bootLR;
              my_motor.turnLeft(255);
              digitalWrite(LED, HIGH);
              break;

          case IRCommand::Right:
              // เลี้ยวขวา
              speedOffset = bootLR;
              trunOffset = bootLR;
              digitalWrite(LED, HIGH);
              break;

          case IRCommand::ForwardBoot:
              MIN_ABS_SPEED_A = 120;
              MIN_ABS_SPEED_B = 120;
              if (setpoint < originalSetpoint + 2)
                setpoint += 0.5;
              speedOffset = boot;
              digitalWrite(LED, HIGH);
              break;

          case IRCommand::BackwardBoot:
              MIN_ABS_SPEED_A = 120;
              MIN_ABS_SPEED_B = 120;
              if (setpoint > originalSetpoint - 2)
                setpoint -= 0.5;
              speedOffset = -boot;
              digitalWrite(LED, HIGH);
          default:
              break;
      }
  }

  // กลับสู่สภาวะ Default ถ้าไม่มีคำสั่ง IR มานานเกินไป
  if (now - ir.getLastCommandTime() > commandTimeout) {
     MIN_ABS_SPEED_A = 40;
     MIN_ABS_SPEED_B = 40;
     trunOffset = 0;
     speedOffset = 0;
     setpoint = originalSetpoint;
     digitalWrite(LED, LOW);
  }

    my_motor.move(output, speedOffset, trunOffset, MIN_ABS_SPEED_A, MIN_ABS_SPEED_B);

// debug
  Serial.print(" | Angle: ");
  Serial.print(input);
  Serial.print(" | Output: ");
  Serial.print(output);
  Serial.print(" | Setpoint: ");
  Serial.println(setpoint);

  delay(10);
}
