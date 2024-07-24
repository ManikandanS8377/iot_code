#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Replace with your network credentials
const char* ssid = "Quantanics1";
const char* password = "Qu@nt@nics18";

WebServer server(80);
Servo myservo;  // create servo object to control a servo

int pos = 0;    // variable to store the servo position

void handleRoot() {
  server.send(200, "text/html", "<h1>ESP32 Servo Control</h1><p><a href=\"/left\">Move Left</a></p><p><a href=\"/right\">Move Right</a></p>");
}

void handleLeft() {
  Serial.println("Rorate Left");
  pos -= 10;
  if (pos < 0) pos = 0;
  myservo.write(pos);
  server.send(200, "text/html", "<p>Moved Left</p><p><a href=\"/\">Back</a></p>");
}

void handleRight() {
  Serial.println("Rorate Right");
  pos += 10;
  if (pos > 180) pos = 180;
  myservo.write(pos);
  server.send(200, "text/html", "<p>Moved Right</p><p><a href=\"/\">Back</a></p>");
}

void setup() {
  Serial.begin(115200);
  myservo.attach(23);  // attaches the servo on GPIO 18 to the servo object

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", handleRoot);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.begin();
  Serial.println("HTTP server started");

  // Initial position
  myservo.write(0);
}

void loop() {
  server.handleClient();
}
