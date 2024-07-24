#include <HardwareSerial.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>
// #include <MFRC522.h>
#include <ArduinoJson.h>

// #define SS_PIN 21 // SS pin for RFID reader
// #define RST_PIN 22 // RST pin for RFID reader

// WiFi credentials
const char* ssid = "Quantanics";
const char* password = "Qu@nt@nics18";

WiFiClient espClient;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
// MFRC522 mfrc522(SS_PIN, RST_PIN);
#include <Adafruit_PN532.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define SCK  (13)
#define MOSI (11)
#define SS   (10)
#define MISO (12)

// Create an instance of the PN532 library using SPI
Adafruit_PN532 nfc(SS);

// Define products
struct Product {
    const char* uid;
    const char* name;
    int price;
    int quantity;
    bool inCart;
};

Product products[] = {
    {"83 32 f5 00", "Milk Bikis", 12, 0, false},
    {"f1 94 e9 fe", "Good Day", 16, 0, false},
    {"43 3c b6 08", "Hide and Seek", 32, 0, false},
    {"83 57 55 fb", "Dark Fantasy", 30, 0, false},
    {"d1 2c 24 1c", "Bounce", 54, 0, false}
};

// Store scanned products in order
Product* scannedProducts[sizeof(products)/sizeof(products[0])] = {nullptr};
int totalPrice = 0;
bool productAddedOrRemoved = false;

void setup() {
    Serial.begin(115200);
    // SPI.begin();
    // mfrc522.PCD_Init();
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
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Setup web server
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", generateHTML());
    });

    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);

    server.begin();
}

void loop() {
    // if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      if(nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)){
        String uid = "";
        // for (byte i = 0; i < mfrc522.uid.size; i++) {
        //     uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX) + " ";
        // }
        for (uint8_t i=0; i < uidLength; i++) {
          uid += String(nfc.uid.uidByte[i] < 0x10 ? "0" : "") + String(nfc.uid.uidByte[i], HEX) + " ";
          //Serial.print(" 0x");Serial.print(uid[i], HEX);
          //0x43 0x3C 0xB6 0x8 Apple
          //0x83 0x57 0x55 0xFB Orange
          //0x83 0x32 0xF5 0x0 Banana
          //0xF1 0x94 0xE9 0xFE Graps
          //0xD1 0x2C 0x24 0x1C Berry
        }
        uid.trim();
        handleProduct(uid.c_str());
        // mfrc522.PICC_HaltA();
    }

    if (productAddedOrRemoved) {
        String json = generateJSON();
        if (json != "{}") {
            ws.textAll(json);
        }
        productAddedOrRemoved = false;
    }
}

void handleProduct(const char* uid) {
    for (int i = 0; i < sizeof(products)/sizeof(products[0]); i++) {
        if (strcmp(products[i].uid, uid) == 0) {
            if (products[i].inCart) {
                products[i].quantity -= 1;
                if (products[i].quantity == 0) {
                    products[i].inCart = false;
                    removeScannedProduct(i);
                }
            } else {
                products[i].quantity += 1;
                products[i].inCart = true;
                addScannedProduct(&products[i]);
            }
            updateTotalPrice();
            productAddedOrRemoved = true;
            break;
        }
    }
}

void addScannedProduct(Product* product) {
    for (int i = 0; i < sizeof(scannedProducts)/sizeof(scannedProducts[0]); i++) {
        if (scannedProducts[i] == nullptr) {
            scannedProducts[i] = product;
            break;
        }
    }
}

void removeScannedProduct(int productIndex) {
    for (int i = 0; i < sizeof(scannedProducts)/sizeof(scannedProducts[0]); i++) {
        if (scannedProducts[i] == &products[productIndex]) {
            for (int j = i; j < sizeof(scannedProducts)/sizeof(scannedProducts[0]) - 1; j++) {
                scannedProducts[j] = scannedProducts[j + 1];
            }
            scannedProducts[sizeof(scannedProducts)/sizeof(scannedProducts[0]) - 1] = nullptr;
            break;
        }
    }
}

void updateTotalPrice() {
    totalPrice = 0;
    for (int i = 0; i < sizeof(products)/sizeof(products[0]); i++) {
        totalPrice += products[i].price * products[i].quantity;
    }
}

String generateHTML() {
    String html = "<html><head><style>";
    html += "body { font-family: Arial, sans-serif; margin: 0; padding: 0; text-align: center; background-color: #f4f4f4; }";
    html += "table { width: 60%; margin: 20px auto; border-collapse: collapse; }";
    html += "table, th, td { border: 1px solid #ddd; padding: 8px; }";
    html += "th { background-color: #4CAF50; color: white; }";
    html += "tr:nth-child(even) { background-color: #f2f2f2; }";
    html += "tr:hover { background-color: #ddd; }";
    html += "h1 { color: #333; }";
    html += "</style></head><body><h1>E-Cart</h1><table><tr><th>S.no</th><th>Product Name</th><th>Quantity</th><th>Price</th></tr>";
    for (int i = 0; i < sizeof(scannedProducts)/sizeof(scannedProducts[0]); i++) {
        if (scannedProducts[i] != nullptr && scannedProducts[i]->quantity > 0) {
            html += "<tr><td>" + String(i+1) + "</td><td>" + String(scannedProducts[i]->name) + "</td><td>" + String(scannedProducts[i]->quantity) + "</td><td>" + String(scannedProducts[i]->price * scannedProducts[i]->quantity) + "</td></tr>";
        }
    }
    html += "<tr><td colspan='3'>Total</td><td>" + String(totalPrice) + "</td></tr>";
    html += "</table><script>var ws = new WebSocket('ws://' + window.location.hostname + '/ws');";
    html += "ws.onmessage = function(event) { var data = JSON.parse(event.data); updateTable(data); };";
    html += "function updateTable(data) { var table = '<tr><th>S.no</th><th>Product Name</th><th>Quantity</th><th>Price</th></tr>'; for (var i = 0; i < data.products.length; i++) { table += '<tr><td>' + (i+1) + '</td><td>' + data.products[i].name + '</td><td>' + data.products[i].quantity + '</td><td>' + (data.products[i].price) + '</td></tr>'; } table += '<tr><td colspan=\"3\">Total</td><td>' + data.total + '</td></tr>'; document.querySelector('table').innerHTML = table; }";
    html += "</script></body></html>";
    return html;
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.println("WebSocket client connected");
        client->text(generateJSON());
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.println("WebSocket client disconnected");
    }
}

String generateJSON() {
    StaticJsonDocument<512> doc;
    JsonArray array = doc.createNestedArray("products");
    for (int i = 0; i < sizeof(scannedProducts)/sizeof(scannedProducts[0]); i++) {
        if (scannedProducts[i] != nullptr && scannedProducts[i]->quantity > 0) {
            JsonObject obj = array.createNestedObject();
            obj["name"] = scannedProducts[i]->name;
            obj["quantity"] = scannedProducts[i]->quantity;
            obj["price"] = scannedProducts[i]->price * scannedProducts[i]->quantity;
        }
    }
    doc["total"] = totalPrice;

    String json;
    serializeJson(doc, json);
    return json;
}
