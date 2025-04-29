//#include "display.h"
//#include <U8g2lib.h>
//
//U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0);  // Page buffer – ประหยัดหน่วยความจำ
//
//GyverOLED<SSH1106_128x64> oled;
//
//void display_setup() {
//    oled.init();
//    oled.clear();
//}
//
//void showLeft(double setpoint, String F) {
//    showDirection("LEFT", "<", setpoint, F);
//}
//
//void showRight(double setpoint, String F) {
//    showDirection("RIGHT", ">", setpoint, F);
//}
//
//void showForward(double setpoint, String F) {
//    showDirection("FORWARD", "^", setpoint, F);
//}
//
//void showBackward(double setpoint, String F) {
//    showDirection("BACKWARD", "v", setpoint, F);
//}
//
//void showDirection(const String& direction, const String& arrow, double input, const String& A) {
//    oled.clear();
//
//    oled.setScale(2);
//    oled.setCursor(1, 0);
//    oled.print(direction);
//
//    oled.setScale(1);
//    oled.setCursor(1, 3);
//    oled.print(A);
//    oled.print(" = ");
//    oled.print(input, 1);
//
//    oled.setScale(2);
//    oled.setCursor(12, 5);
//    oled.print(arrow);
//
//    oled.update();
//}
