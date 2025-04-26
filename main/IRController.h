#ifndef IRCONTROLLER_H
#define IRCONTROLLER_H

#include <IRremote.h>

class IRController {
public:
    IRController(int recvPin);
    void begin();
    bool available(unsigned long &codeOut);

    bool forward(unsigned long code);
    bool backward(unsigned long code);
    bool turnLeft(unsigned long code);
    bool turnRight(unsigned long code);

private:
    IRrecv irrecv;
    decode_results results;
    unsigned long lastCode;
    unsigned long lastIRTime;
};

#endif
