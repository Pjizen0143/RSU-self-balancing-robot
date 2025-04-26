#include <MPU6050_tockn.h>
#include <Wire.h>

#define IN1 13
#define IN2 12
#define ENA 7
#define ENB 10
#define IN3 9
#define IN4 8

MPU6050 mpu6050(Wire); // สร้างออบเจกต์ MPU6050

float kp = 20, ki = 0, kd = 1; // ค่าพารามิเตอร์ PID
float setpoint = 0; // มุมที่ต้องการ (สมดุล)
float error, lastError, integral, derivative, output; // ตัวแปรสำหรับ PID
String cmdstring;
String cmdcode;
String parmstring;
bool newcmd = 0;
int sepIndex;
bool noparm = 0;

void pidControl(float pitch) {
  error = setpoint - pitch;
  integral += error;
  derivative = error - lastError;
  output = kp * error + ki * integral + kd * derivative;
  lastError = error;
  motorcontrol(output); // เรียก function ควบคุมมอเตอร์
}

void motorcontrol(float output) {
  int motorSpeed = constrain(output, -255, 255);

  // แสดงค่าความเร็วที่ส่งไปที่ ENA และ ENB
  Serial.print("ENA: ");
  Serial.print(abs(motorSpeed));  // ค่าความเร็วที่ส่งไปยัง ENA
  Serial.print("\tENB: ");
  Serial.println(abs(motorSpeed));  // ค่าความเร็วที่ส่งไปยัง ENB

  analogWrite(ENA, abs(motorSpeed));
  digitalWrite(IN1, motorSpeed > 0 ? HIGH : LOW);
  digitalWrite(IN2, motorSpeed > 0 ? LOW : HIGH);
  analogWrite(ENB, abs(motorSpeed));
  digitalWrite(IN3, motorSpeed > 0 ? LOW : HIGH);
  digitalWrite(IN4, motorSpeed > 0 ? HIGH : LOW);
}

void setup() {
  Serial.begin(9600);
  Wire.begin(); // เริ่มการสื่อสาร I2C
  mpu6050.begin(); // เริ่มการทำงานของเซ็นเซอร์
  mpu6050.calcGyroOffsets(true); // ปรับเทียบค่า Gyroscope อัตโนมัติ

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  mpu6050.update(); // อัปเดตค่าจากเซ็นเซอร์

  Serial.print("angleX : ");
  Serial.print(mpu6050.getAngleX());
  Serial.print("\t angleY : ");
  Serial.print(mpu6050.getAngleY());
  Serial.print("\t angleZ : ");
  Serial.println(mpu6050.getAngleZ());

  pidControl(mpu6050.getAngleX()); // เรียกฟังก์ชัน PID Control เพื่อคำนวณค่าความเร็ว

  delay(10); // ให้เวลาสำหรับ PID sample time
}
