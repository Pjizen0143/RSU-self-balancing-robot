#include "IRController.h"

IRController::IRController(int recvPin) : irrecv(recvPin) {
    lastCode = 0;
    lastIRTime = 0;
}

void IRController::begin() {
    irrecv.enableIRIn();
}

bool IRController::available(unsigned long &codeOut) {
    if (irrecv.decode(&results)) {
        lastIRTime = millis();

        if (results.value == 0xFFFFFFFF) {
            codeOut = lastCode;
        } else {
            lastCode = results.value;
            codeOut = results.value;
        }

        irrecv.resume();
        return true;
    }

    if (millis() - lastIRTime > 200) {
        codeOut = 0;
    }

    return false;
}

bool IRController::forward(unsigned long code) {
    return code == 0xFF18E7;
}

bool IRController::backward(unsigned long code) {
    return code == 0xFF4AB5;
}

bool IRController::turnLeft(unsigned long code) {
    return code == 0xFF10EF;
}

bool IRController::turnRight(unsigned long code) {
    return code == 0xFF5AA5;
}
