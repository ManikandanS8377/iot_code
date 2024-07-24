#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

void connectToWiFi() {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting...");
    }

    Serial.println("Connected to WiFi");
}

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue().c_str();
        
        if (value.length() > 0) {
            Serial.println("*********");
            Serial.print("New value: ");
            
            // Convert std::string to String for Serial printing
            for (char c : value) {
                Serial.print(c);
            }
            
            Serial.println();
            Serial.println("*********");

            // Call the WiFi connect function
            connectToWiFi();
        }
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

    BLEDevice::init("ESP32_BLE");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(BLEUUID((uint16_t)0xFFE0));
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                           BLEUUID((uint16_t)0xFFE1),
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE
                                         );

    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Hello World");
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}

void loop() {
    // Nothing to do here
}
