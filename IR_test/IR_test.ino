#include <IRremote.h>
#define RECV_PIN 4

IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long lastCode = 0;
unsigned long lastIRTime = 0;

bool forward(unsigned long code) {
  return (code == 0xFF18E7);
}

bool backward(unsigned long code) {
  return (code == 0xFF4AB5);
}

bool turn_left(unsigned long code) {
  return (code == 0xFF10EF);
}

bool turn_right(unsigned long code) {
  return (code == 0xFF5AA5);
}

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
}

// ปรับให้ return bool
bool ir_available(unsigned long &codeOut) {
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

  // ถ้าไม่มีข้อมูลใหม่
  if (millis() - lastIRTime > 200) {
    codeOut = 0;
  }

  return false;
}

void loop() {
  unsigned long currentCode = 0;

  if (ir_available(currentCode)) {
    if (forward(currentCode)) {
    Serial.println("Forward");
  } else if (backward(currentCode)) {
    Serial.println("Backward");
  } else if (turn_left(currentCode)) {
    Serial.println("Left");
  } else if (turn_right(currentCode)) {
    Serial.println("Right");
  }
  }else{
    Serial.println("no command");
  }

  

  delay(100);
}
