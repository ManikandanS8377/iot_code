/*
 * ESP32 Voltage Sensor Example
 */

// Define the analog input pin for the voltage sensor
#define VOLTAGE_SENSOR_PIN 23

// Define the reference voltage (usually 3.3V or 5V)
#define REFERENCE_VOLTAGE 5

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Voltage Sensor Example");
}

void loop() {
  // Read the analog value from the voltage sensor
  int analogValue = analogRead(VOLTAGE_SENSOR_PIN);

  // Convert the analog value to voltage
  float voltage = (analogValue * REFERENCE_VOLTAGE) / 4095.0;

  // Print the voltage value
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  // Delay for a short period
  delay(1000);
}
