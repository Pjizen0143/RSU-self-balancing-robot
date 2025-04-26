#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu6050(Wire);

void setup(){
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.setGyroOffsets(-1.51, 1.00, -1.50);
  Serial.println("MPU6050 Setuped");
}

void loop(){
  mpu6050.update();
  Serial.print("X angle: ");
  Serial.println(mpu6050.getAngleX());
  delay(100);
}
