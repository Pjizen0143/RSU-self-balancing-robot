#ifndef motorcontrol_h
#define motorcontrol_h


#include "Arduino.h"


class motorcontrol
{
protected:
    int _ena, _in1, _in2, _enb, _in3, _in4;
    int _currentSpeed;
    double _motorAConst, _motorBConst;
public:
    motorcontrol(int ena, int in1, int in2, int enb, int in3, int in4, double motorAConst, double motorBConst);
    void begin();
    void move(int speed, int minAbsSpeed);
    void turnLeft(int speed, bool kick);
    void turnRight(int speed, bool kick);
    void stopMoving();
};


#endif // motorcontrol_h
