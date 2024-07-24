const int PIR_SENSOR_OUTPUT_PIN = 23; // Output PIN
#define buzzerPin 32 // Buzzer PIN
#define LED_PIN_D 33 // LED for Detection
#define LED_PIN_A 25 // LED for Alarm
int warm_up;



void setup() {
  Serial.begin(115200); // Serial Communication

  pinMode(buzzerPin, OUTPUT);
  pinMode(PIR_SENSOR_OUTPUT_PIN, INPUT);
  pinMode(LED_PIN_D, OUTPUT);
  pinMode(LED_PIN_A, OUTPUT);

  Serial.println("Waiting For Power On Warm Up");
  delay(20000); /* Power On Warm Up Delay */
  Serial.println("Mission Acticated!");
}

void loop() {
  int sensor_output;
  sensor_output = digitalRead(PIR_SENSOR_OUTPUT_PIN);
  if( sensor_output == LOW )
  {
    digitalWrite(LED_PIN_D, LOW);
    if( warm_up == 1 )
     {
      Serial.print("Warming Up...\n\n");
      warm_up = 0;
      delay(2000);
    }
    Serial.print("No object in the Range...\n\n");
    digitalWrite(buzzerPin, LOW);
    delay(1000);
  }
  else
  {
    Serial.print("Object detected in the Range...\n\n");   
    digitalWrite(LED_PIN_D, HIGH);
    warm_up = 1;
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(LED_PIN_A, HIGH);
    delay(100);
    while (digitalRead(PIR_SENSOR_OUTPUT_PIN) == HIGH) {
      digitalWrite(buzzerPin, LOW);
      digitalWrite(LED_PIN_A, LOW);
      delay(100);
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(LED_PIN_A, HIGH);
      delay(100);
    }
    digitalWrite(buzzerPin, LOW);
    digitalWrite(LED_PIN_A, LOW);
    delay(100);
  } 
}