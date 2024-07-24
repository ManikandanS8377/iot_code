// Define the pin for the gas sensor
const int gasSensorPin = 36; // GPIO pin for gas sensor

void setup() {
  Serial.begin(9600);
  pinMode(gasSensorPin, INPUT);
}

void loop() {
  int gasSensorValue = analogRead(gasSensorPin);
  
  // Convert the analog value to a voltage (assuming 3.3V reference)
  float voltage = gasSensorValue * (3.3 / 4095.0);
  
  // Print the voltage and the corresponding gas level to the Serial Monitor
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.print("V, Gas Level: ");
  
  if (voltage < 1.0) {
    Serial.println("Low");
  } else if (voltage >= 1.0 && voltage < 2.0) {
    Serial.println("Moderate");
  } else {
    Serial.println("High");
  }
  
  delay(1000); // Delay for a second before taking the next reading
}
