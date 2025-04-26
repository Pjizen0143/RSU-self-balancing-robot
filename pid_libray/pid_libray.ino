#include <MPU6050_tockn.h>
#include <Wire.h>
#include <PID_v1.h> // เพิ่มไลบรารี PID

#define IN1 8
#define IN2 9
#define ENA 11
#define ENB 10
#define IN3 12
#define IN4 13

MPU6050 mpu6050(Wire); // สร้างออบเจกต์ MPU6050

// PID parameters
double kp = 80, ki = 0, kd = 2; // ค่า PID เบื้องต้น
double setpoint = 7.42;            // มุมที่ต้องการ
double input, output;             // input คือ pitch, output คือค่าที่ส่งให้ motor

PID pid(&input, &output, &setpoint, kp, ki, kd, DIRECT); // สร้าง PID object

String cmdstring;
String cmdcode;
String parmstring;
bool newcmd = 0;
int sepIndex;
bool noparm = 0;

void motorcontrol(double output) {
    int motorSpeed = constrain(output, -255, 255);
    analogWrite(ENA, abs(motorSpeed));
    digitalWrite(IN2, motorSpeed > 0 ? LOW : HIGH);
    digitalWrite(IN1, motorSpeed > 0 ? HIGH : LOW);
    analogWrite(ENB, abs(motorSpeed));
    digitalWrite(IN4, motorSpeed > 0 ? LOW : HIGH);
    digitalWrite(IN3, motorSpeed > 0 ? HIGH : LOW);
}

void setup() {
    Serial.begin(9600);
    Wire.begin(); // เริ่มการสื่อสาร I2C
    mpu6050.begin(); // เริ่มการทำงานของเซ็นเซอร์
    mpu6050.setGyroOffsets(-1.51, 1.15, -1.38);
    Serial.println("mpu6050 started!");
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    pid.SetMode(AUTOMATIC);       // เปิด PID
    pid.SetSampleTime(10);        // ตั้ง Sample Time = 10ms
    pid.SetOutputLimits(-255, 255); // จำกัด output
}

void loop() {
    mpu6050.update(); // อ่านค่าจาก MPU6050
    input = mpu6050.getAngleY(); // ให้ input = pitch (แกน Y)

    Serial.print("angleX : ");
    Serial.print(mpu6050.getAngleX());
    Serial.print("\t angleY : ");
    Serial.print(input);
    Serial.print("\t angleZ : ");
    Serial.println(mpu6050.getAngleZ());

    pid.Compute(); // คำนวณ PID
    motorcontrol(output); // ส่ง output ไปควบคุมมอเตอร์

    if (Serial.available() > 0) { // detect new input
        cmdstring = Serial.readString();
        newcmd = 1;
    }

    if (newcmd == 1) {
        Serial.println(cmdstring);
        callcmd();
        newcmd = 0;
    }

    delay(10); // หน่วง 10ms ตามที่ต้องการ
}

void callcmd() {
    cmdstring.trim();
    sepIndex = cmdstring.indexOf('=');
    if (sepIndex == -1) {
        cmdcode = cmdstring;
        noparm = 1;
    } else {
        cmdcode = cmdstring.substring(0, sepIndex);
        cmdcode.trim();
        parmstring = cmdstring.substring(sepIndex + 1);
        parmstring.trim();
        noparm = 0;
    }

    if (cmdcode.equalsIgnoreCase("kp")) {
        if (noparm == 0) {
            kp = parmstring.toDouble();
            pid.SetTunings(kp, ki, kd); // ต้อง SetTunings ใหม่
            Serial.print("Kp: ");
            Serial.println(kp);
        }
    }
    else if (cmdcode.equalsIgnoreCase("ki")) {
        if (noparm == 0) {
            ki = parmstring.toDouble();
            pid.SetTunings(kp, ki, kd); // ต้อง SetTunings ใหม่
            Serial.print("Ki: ");
            Serial.println(ki);
        }
    }
    else if (cmdcode.equalsIgnoreCase("kd")) {
        if (noparm == 0) {
            kd = parmstring.toDouble();
            pid.SetTunings(kp, ki, kd); // ต้อง SetTunings ใหม่
            Serial.print("Kd: ");
            Serial.println(kd);
        }
    }
}
