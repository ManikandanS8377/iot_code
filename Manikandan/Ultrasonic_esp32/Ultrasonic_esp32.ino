#include <Arduino.h>

// Define pins for ultrasonic sensor
const int trigPin = 3; // Trigger pin
const int echoPin = 1; // Echo pin

void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Clear the trigger pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Set the trigger pin high for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Measure the duration of the echo pulse
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in centimeters
  // Speed of sound is 343 meters/second
  // Formula: distance = (duration * speed of sound) / 2
  float distance_cm = (duration * 0.0343) / 2;
  
  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
  
  // Wait for some time before taking the next reading
  delay(1000);
}
