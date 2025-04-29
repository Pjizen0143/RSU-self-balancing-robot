#include "motorcontrol.h"
#include "Arduino.h"


motorcontrol::motorcontrol(int ena, int in1, int in2, int enb, int in3, int in4, double motorAConst, double motorBConst){
    _motorAConst = motorAConst;
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


void motorcontrol::move(int speed, int minAbsSpeed)
{
    int direction = 1;
    
    if (speed < 0)
    {
        direction = -1;
        
        speed = min(speed, -1*minAbsSpeed);
        speed = max(speed, -255);
    }
    else
    {
        speed = max(speed, minAbsSpeed);
        speed = min(speed, 255);
    }
    
    if (abs(speed - _currentSpeed) < 3) return;
    
    int realSpeed = max(minAbsSpeed, abs(speed));
    
    digitalWrite(_in1, speed > 0 ? HIGH : LOW);
    digitalWrite(_in2, speed > 0 ? LOW : HIGH);
    digitalWrite(_in3, speed > 0 ? HIGH : LOW);
    digitalWrite(_in4, speed > 0 ? LOW : HIGH);
    analogWrite(_ena, constrain(realSpeed * _motorAConst, 0, 255));
    analogWrite(_enb, constrain(realSpeed * _motorBConst, 0, 255));
    
    _currentSpeed = direction * realSpeed;
}


void motorcontrol::turnLeft(int speed, bool kick)
{
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, LOW);
    digitalWrite(_in3, LOW);
    digitalWrite(_in4, HIGH);
    
    if (kick)
    {
        analogWrite(_ena, 255);
        analogWrite(_enb, 255);
    
        delay(100);
    }
    
    analogWrite(_ena, speed * _motorAConst);
    analogWrite(_enb, speed * _motorBConst);
}


void motorcontrol::turnRight(int speed, bool kick)
{
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, HIGH);
    digitalWrite(_in3, HIGH);
    digitalWrite(_in4, LOW);
 
    if (kick)
    {
        analogWrite(_ena, 255);
        analogWrite(_enb, 255);
    
        delay(100);
    }
    
    analogWrite(_ena, speed * _motorAConst);
    analogWrite(_enb, speed * _motorBConst);
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
