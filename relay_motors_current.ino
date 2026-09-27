// this is the current arduino script, it just controls 2 motors and relay.
#include <Servo.h>

const int NUM_MOTORS = 2;
const int MOTOR_PINS[NUM_MOTORS] = {9, 10}; 
Servo motors[NUM_MOTORS];

const int RELAY_PIN = 7; 

int currentPWM[NUM_MOTORS] = {1500, 1500};
unsigned long lastCommandTime = 0;

const unsigned long FAILSAFE_TIMEOUT_MS = 500000; 

void setup() {
  Serial.begin(115200);
  
  // Initialize Motors
  for (int i = 0; i < NUM_MOTORS; i++) {
    motors[i].attach(MOTOR_PINS[i]);
    motors[i].writeMicroseconds(1500);
  }
  
  // Initialize Relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Default to OFF
  
  // Give the Apis Queen ESC 5 seconds to initialize and arm
  delay(5000); 
}

void loop() {
  // 1. Handle incoming motor & relay data
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    int parsedValues[3]; 
    int parseCount = 0;
    
    int commaIndex = 0;
    int nextCommaIndex = data.indexOf(',');
    
    while (nextCommaIndex != -1 && parseCount < 2) {
      parsedValues[parseCount] = data.substring(commaIndex, nextCommaIndex).toInt();
      commaIndex = nextCommaIndex + 1;
      nextCommaIndex = data.indexOf(',', commaIndex);
      parseCount++;
    }
    
    if (parseCount == 2) {
      parsedValues[2] = data.substring(commaIndex).toInt();
      
      for (int i = 0; i < NUM_MOTORS; i++) {
        currentPWM[i] = constrain(parsedValues[i], 1000, 2000);
        motors[i].writeMicroseconds(currentPWM[i]);
      }
      
      if (parsedValues[2] > 0) {
        Serial.println("HIGH");
        digitalWrite(RELAY_PIN, HIGH);
      } else {
        Serial.println("LOW");
        digitalWrite(RELAY_PIN, LOW);
      }
      
      lastCommandTime = millis(); 
    }
  }

  // 2. FAILSAFE
  if (millis() - lastCommandTime > FAILSAFE_TIMEOUT_MS) {
    for (int i = 0; i < NUM_MOTORS; i++) {
      motors[i].writeMicroseconds(1500);
    }
    digitalWrite(RELAY_PIN, LOW);
  }
}
