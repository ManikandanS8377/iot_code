#define FlowSensor_INPUT 10 // Arduino interrupt pin connected to flow sensor
#define FLOW_RATE_CONSTANT 7.5 // Flow rate constant for the sensor
#define PERIOD 1000 // Time period in milliseconds to calculate flow rate

volatile unsigned long pulse_counter = 0;
unsigned long old_time;

void setup() {
  Serial.begin(115200);
  pinMode(FlowSensor_INPUT, INPUT);
  attachInterrupt(digitalPinToInterrupt(FlowSensor_INPUT), interrupt_handler, RISING);
  old_time = millis();
}

void loop() {
  unsigned long new_time = millis() - old_time;
  
  if (new_time >= PERIOD) {
    noInterrupts();
    unsigned long pulse_count = pulse_counter;
    pulse_counter = 0;
    interrupts();
    
    old_time = millis();
    
    float pulse_rate = (pulse_count * 1000.0) / new_time; // Convert ms to seconds
    float flow_rate = pulse_rate * FLOW_RATE_CONSTANT;
    
    Serial.print("Flow Rate: ");
    Serial.println(flow_rate, 2); // Display flow rate
  }
}

void interrupt_handler() {
  pulse_counter++;
}
