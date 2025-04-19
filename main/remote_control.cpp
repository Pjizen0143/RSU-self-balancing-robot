
#include <IRremote.h>

// กำหนดหมายเลขขาสำหรับตัวรับสัญญาณ IR
#define RECV_PIN 4

// กำหนดหมายเลขขาสำหรับควบคุมมอเตอร์ด้านซ้าย
#define IN1 13
#define IN2 12
#define ENA 11 // ขา Enable สำหรับมอเตอร์ด้านซ้าย (ควบคุมความเร็ว)

// กำหนดหมายเลขขาสำหรับควบคุมมอเตอร์ด้านขวา
#define ENB 10 // ขา Enable สำหรับมอเตอร์ด้านขวา (ควบคุมความเร็ว)
#define IN3 9
#define IN4 8

// สร้างอ็อบเจ็กต์สำหรับตัวรับสัญญาณ IR
IRrecv irrecv(RECV_PIN);

// สร้างตัวแปรสำหรับเก็บผลลัพธ์จากการถอดรหัสสัญญาณ IR
decode_results results;

// ตัวแปรสำหรับเก็บค่าปุ่ม IR ล่าสุดที่ถูกกด
unsigned long lastCode = 0;

void setup()
{
    // เริ่มต้นการสื่อสารผ่าน Serial Port ที่ความเร็ว 115200 bps
    Serial.begin(115200);

    // เปิดใช้งานตัวรับสัญญาณ IR เพื่อเริ่มรับข้อมูล
    irrecv.enableIRIn();

    // กำหนดให้ขาควบคุมมอเตอร์เป็นขาออก
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

void loop()
{
    // ตรวจสอบว่ามีการรับข้อมูล IR หรือไม่
    if (irrecv.decode(&results))
    {
        // แสดงค่าที่ได้รับจากรีโมทในรูปแบบเลขฐาน 16 ผ่าน Serial Monitor
        Serial.println(results.value, HEX);

        // เก็บค่ารหัส IR ที่ได้รับ
        unsigned long code = results.value;

        // ตรวจสอบว่ามีการกดปุ่มค้างหรือไม่ (ค่าที่ได้จะเป็น 0xFFFFFFFF)
        if (code == 0xFFFFFFFF)
        {
            // หากกดค้าง ให้ใช้ค่ารหัสปุ่มล่าสุดที่เคยได้รับ
            code = lastCode;
        }
        else
        {
            // หากเป็นการกดปุ่มใหม่ ให้อัปเดตค่ารหัสปุ่มล่าสุด
            lastCode = code;
        }

        // ตรวจสอบว่ารหัสที่ได้รับตรงกับรหัสของปุ่ม Forward หรือไม่
        if (code == 0xFF18E7)
        {
            Serial.println("Forward");
            // สั่งให้มอเตอร์ด้านซ้ายหมุนไปข้างหน้า
            digitalWrite(ENA, HIGH); // เปิดใช้งานมอเตอร์ด้านซ้าย
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
            digitalWrite(ENB, HIGH); // เปิดใช้งานมอเตอร์ด้านขวา
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
        }

        // ตรวจสอบว่ารหัสที่ได้รับตรงกับรหัสของปุ่ม Backward หรือไม่
        if (code == 0xFF4AB5)
        {
            Serial.println("Backward");
            // สั่งให้มอเตอร์ด้านซ้ายหมุนไปข้างหลัง
            digitalWrite(ENA, HIGH); // เปิดใช้งานมอเตอร์ด้านซ้าย
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, HIGH);
            // สั่งให้มอเตอร์ด้านขวาหมุนไปข้างหลัง
            digitalWrite(ENB, HIGH); // เปิดใช้งานมอเตอร์ด้านขวา
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);
        }

        if (code == 0xFF10EF)
        {
            Serial.println("Left");
            // สั่งให้มอเตอร์ด้านซ้ายหยุุดและมอเตอร์ด้านขวาหมุนไปข้างหน้า
            digitalWrite(ENA, HIGH);
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, LOW); // หยุดมอเตอร์ด้านซ้าย
            digitalWrite(ENB, HIGH);
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
        }

        if (code == 0xFF5AA5)
        { // <--- รหัสสมมติสำหรับปุ่มเลี้ยวขวา คุณอาจต้องเปลี่ยนตามรีโมทของคุณ
            Serial.println("Right");
            // สั่งให้มอเตอร์ด้านขวาหยุด และมอเตอร์ด้านซ้ายหมุนไปข้างหน้า
            digitalWrite(ENA, HIGH);
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
            digitalWrite(ENB, HIGH);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, LOW); // หยุดมอเตอร์ด้านขวา
        }

        // ตรวจสอบว่ารหัสที่ได้รับตรงกับรหัสของปุ่ม Stop หรือไม่
        if (code == 0xFF22DD)
        {
            Serial.println("Stop");
            // สั่งให้มอเตอร์ทั้งสองหยุดหมุน
            digitalWrite(ENA, HIGH); // เปิดใช้งานมอเตอร์ด้านซ้าย (แต่กำหนดทิศทางให้หยุด)
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, LOW);
            digitalWrite(ENB, HIGH); // เปิดใช้งานมอเตอร์ด้านขวา (แต่กำหนดทิศทางให้หยุด)
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, LOW);
        }

        // เริ่มการรับสัญญาณ IR ครั้งต่อไป
        irrecv.resume();
    }
}
