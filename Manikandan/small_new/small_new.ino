#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

// Constants for the sensors and MQTT
#define DHTPIN 23         // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11     // DHT 11
#define TRIGGER_PIN1 16   // GPIO pin connected to the first ultrasonic sensor's trig pin
#define ECHO_PIN1 17      // GPIO pin connected to the first ultrasonic sensor's echo pin
#define TRIGGER_PIN2 18   // GPIO pin connected to the second ultrasonic sensor's trig pin
#define ECHO_PIN2 19      // GPIO pin connected to the second ultrasonic sensor's echo pin
#define LDR_SENSOR_PIN 36 // GPIO pin connected to the LDR sensor (digital)
#define SSID "Quantanics"
#define PASSWORD "Qu@nt@nics18"
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883
#define CLIENT_ID "db032482-2f62-4ba8-9782-da6521020775"
#define MQTT_TOPIC "/quantanics/industry/Small_Board"

// Initialize objects for the sensors and LCD
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize the LCD with address 0x27
WiFiClient espClient;
PubSubClient client(espClient);

// Timing variables
unsigned long previousMillis = 0;
const long interval = 2000; // Interval for distance measurement and MQTT connection check

void setupWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(CLIENT_ID)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(TRIGGER_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIGGER_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);
  pinMode(LDR_SENSOR_PIN, INPUT);

  setupWiFi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
}

float getDistance(int trigPin, int echoPin) {
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

float getDistance2(int trigPin, int echoPin) {
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void publishData(float humidity, float temperature, float distance1, float distance2, int ldrValue) {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["humidity"] = humidity;
  jsonDoc["temperature"] = temperature;
  jsonDoc["distance1"] = distance1;
  jsonDoc["distance2"] = distance2;
  jsonDoc["ldrValue"] = ldrValue;

  String data;
  serializeJson(jsonDoc, data);
  client.publish(MQTT_TOPIC, data.c_str());

  Serial.println("Published to MQTT: " + data);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    float distance1 = getDistance(TRIGGER_PIN1, ECHO_PIN1);
    float distance2 = getDistance2(TRIGGER_PIN2, ECHO_PIN2);
    int ldrValue = digitalRead(LDR_SENSOR_PIN);
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      h = 0;
      t = 0;
    }

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C\t");
    Serial.print("Distance1: ");
    Serial.print(distance1);
    Serial.print(" cm\t");
    Serial.print("Distance2: ");
    Serial.print(distance2);
    Serial.print(" cm\t");
    Serial.print("LDR Value: ");
    Serial.println(ldrValue);

    lcd.setCursor(0, 0);
    lcd.print("H: ");
    lcd.print(h);
    lcd.print("% T: ");
    lcd.print(t);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("D1: ");
    lcd.print(distance1);
    lcd.print("cm D2: ");
    lcd.print(distance2);
    lcd.print("cm");

    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LDR: ");
    lcd.print(ldrValue);

    if (!client.connected()) {
      reconnect();
    }

    publishData(h, t, distance1, distance2, ldrValue);
  }

  client.loop();
}
