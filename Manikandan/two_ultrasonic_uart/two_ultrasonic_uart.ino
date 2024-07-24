#include <HardwareSerial.h>

// HardwareSerial MySerial1(1);
HardwareSerial MySerial2(2);

void setup() {
    Serial.begin(115200);
    //MySerial1.begin(9600, SERIAL_8N1, 16, 17); // 16 Trigger, 17 Echo
    MySerial2.begin(9600, SERIAL_8N1, 3, 1); //3 Trigger, 1 Echo
    Serial.println("Setup complete");
}

void loop() {
    // Read from first ultrasonic sensor
    // int distance1 = readUltrasonic(MySerial1);
    // Serial.print("Distance 1: ");
    // Serial.println(distance1);

    // Read from second ultrasonic sensor
    int distance2 = readUltrasonic(MySerial2);
    Serial.print("Distance 2: ");
    Serial.println(distance2);

    delay(1000);
}

int readUltrasonic(HardwareSerial &mySerial) {
    mySerial.write(0x01);
    delayMicroseconds(10);
    int duration = pulseIn(mySerial, HIGH, 25000);
    int distance = duration * 0.034 / 2;
    return distance;
}