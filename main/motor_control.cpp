#include "motor_control.h"

MotorControl::MotorControl(int IN1, int IN2, int IN3, int IN4, int ENA, int ENB)
    : in1Pin(IN1), in2Pin(IN2), in3Pin(IN3), in4Pin(IN4), enaPin(ENA), enbPin(ENB) {}

void MotorControl::begin(){
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enaPin, OUTPUT);
  pinMode(enbPin, OUTPUT);
}

void MotorControl::driveMotor(float output) {
  int motorSpeed = constrain(output, MIN_MOTOR_SPEED, MAX_MOTOR_SPEED);
  analogWrite(enaPin, abs(motorSpeed));
  digitalWrite(in1Pin, motorSpeed > 0 ? HIGH : LOW);
  digitalWrite(in2Pin, motorSpeed > 0 ? LOW : HIGH);
  analogWrite(enbPin, abs(motorSpeed));
  digitalWrite(in3Pin, motorSpeed > 0 ? HIGH : LOW);
  digitalWrite(in4Pin, motorSpeed > 0 ? LOW : HIGH);
}
