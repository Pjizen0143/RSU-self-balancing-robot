#include "motorcontrol.h"
#include "Arduino.h"


motorcontrol::motorcontrol(int ena, int in1, int in2, int enb, int in3, int in4, double motorAConst, double motorBConst){
    _motorAConst = motorAConst; // ตัวคูณความเร็วของมอเตอร์ A
    _motorBConst = motorBConst; 
    
  _ena = ena;
  _in1 = in1;
  _in2 = in2;
  _enb = enb;
  _in3 = in3;
  _in4 = in4;
}

void motorcontrol::begin(){
  pinMode(_ena, OUTPUT);
  pinMode(_in1, OUTPUT);
  pinMode(_in2, OUTPUT);
    
  pinMode(_enb, OUTPUT);
  pinMode(_in3, OUTPUT);
  pinMode(_in4, OUTPUT);
}

void motorcontrol::_move(int speed, int minAbsSpeedA, int minAbsSpeedB, int bias)
{
    int direction = 1;
    direction = (speed < 0) ? -1 : 1;
    speed = constrain(speed, -255, 255);

    // ถ้าความเร็วใหม่ต่างจากเดิมไม่ถึง 3 กับไม่มี bias ไม่ต้องทำอะไร
    if (abs(speed - _currentSpeed) < 3 && bias == 0) return;

    int baseSpeed = abs(speed);
    int speedA = baseSpeed + bias;
    int speedB = baseSpeed - bias;

    // บังคับให้ speedA, speedB มีค่าขั้นต่ำตาม minAbs
    if (speedA != 0)
        speedA = max(abs(speedA), minAbsSpeedA);
    if (speedB != 0)
        speedB = max(abs(speedB), minAbsSpeedB);

    speedA = constrain(speedA, 0, 255);
    speedB = constrain(speedB, 0, 255);

    digitalWrite(_in1, speed > 0 ? HIGH : LOW);
    digitalWrite(_in2, speed > 0 ? LOW : HIGH);
    digitalWrite(_in3, speed > 0 ? HIGH : LOW);
    digitalWrite(_in4, speed > 0 ? LOW : HIGH);

    analogWrite(_ena, speedA * _motorAConst);
    analogWrite(_enb, speedB * _motorBConst);

    _currentSpeed = direction * baseSpeed;
}


void motorcontrol::turnLeft(int speed)
{
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, LOW);
    digitalWrite(_in3, LOW);
    digitalWrite(_in4, HIGH);
    
    analogWrite(_ena, speed * _motorAConst);
    analogWrite(_enb, speed * _motorBConst);
}


void motorcontrol::turnRight(int speed)
{
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, HIGH);
    digitalWrite(_in3, HIGH);
    digitalWrite(_in4, LOW);
    
    analogWrite(_ena, speed * _motorAConst);
    analogWrite(_enb, speed * _motorBConst);
}


void motorcontrol::move(int speed, int speedoffset, int turnOffset, int minAbsSpeedA, int minAbsSpeedB)
{
    int controlSpeed = speed + speedoffset;
    _move(controlSpeed, minAbsSpeedA, minAbsSpeedB, turnOffset);
}



void motorcontrol::stop() {
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, HIGH);
    digitalWrite(_in3, HIGH);
    digitalWrite(_in4, HIGH);
    analogWrite(_ena, 0);
    analogWrite(_enb, 0);
    _currentSpeed = 0;
}
