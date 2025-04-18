#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

class MotorControl {
public:
  MotorControl(int IN1, int IN2, int IN3, int IN4, int ENA, int ENB);
  void driveMotor(float output);
  void begin();
private:
  int in1Pin, in2Pin, in3Pin, in4Pin, enaPin, enbPin;

  static const int MAX_MOTOR_SPEED = 255;
  static const int MIN_MOTOR_SPEED = -255;
};

#endif // MOTOR_CONTROL_H
