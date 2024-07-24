#include <Wire.h>
#include <Adafruit_PN532.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
#include <ArduinoJson.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define SCK  (13)
#define MOSI (11)
#define SS   (10)
#define MISO (12)

// Create an instance of the PN532 library using SPI
Adafruit_PN532 nfc(SS);

// WiFi credentials
const char* ssid = "Quantanics";
const char* password = "Qu@nt@nics18";

WiFiClient espClient;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Define products
struct Product {
    const char* uid;
    const char* name;
    int price;
    int quantity;
    bool inCart;
};
Product products[] = {
    {"0x43 0x3C 0xB6 0x8", "Milk Bikis", 12, 0, false},
    {"0x83 0x57 0x55 0xFB", "Good Day", 16, 0, false},
    {"0x83 0x32 0xF5 0x0", "Hide and Seek", 32, 0, false},
    {"0xF1 0x94 0xE9 0xFE", "Dark Fantasy", 30, 0, false},
    {"0xD1 0x2C 0x24 0x1C", "Bounce", 54, 0, false}
};

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an NFC card ...");
}

void loop(void) {
  uint8_t success;                          // Flag to check if there was a successful read
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on the card type)

  // Wait for an NFC card to appear
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    for (uint8_t i=0; i < uidLength; i++) {
      Serial.print(" 0x");Serial.print(uid[i], HEX);
    }
    Serial.println("");

    
    
    // Wait 1 second before continuing
    delay(1000);
  }
}
