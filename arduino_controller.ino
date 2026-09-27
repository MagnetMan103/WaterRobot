#include <Servo.h>

const int NUM_MOTORS = 4;
// Define pins for up to 4 motors. Currently using 9 for the front left.
const int MOTOR_PINS[NUM_MOTORS] = {9, 10, 11, 12}; 
Servo motors[NUM_MOTORS];

int currentPWM[NUM_MOTORS] = {1500, 1500, 1500, 1500};
unsigned long lastCommandTime = 0;
const unsigned long FAILSAFE_TIMEOUT_MS = 500; 

void setup() {
  Serial.begin(115200);
  
  for (int i = 0; i < NUM_MOTORS; i++) {
    motors[i].attach(MOTOR_PINS[i]);
    motors[i].writeMicroseconds(1500);
  }
  
  // Give the Apis Queen ESC 5 seconds to initialize and arm
  delay(5000); 
}

void loop() {
  // Check if the Pi Receiver has sent new data
  if (Serial.available() > 0) {
    // Read the incoming string until the newline character
    String data = Serial.readStringUntil('\n');
    int parsedValues[NUM_MOTORS];
    int parseCount = 0;
    
    int commaIndex = 0;
    int nextCommaIndex = data.indexOf(',');
    
    // Parse the comma-separated string into integers
    while (nextCommaIndex != -1 && parseCount < NUM_MOTORS - 1) {
      parsedValues[parseCount] = data.substring(commaIndex, nextCommaIndex).toInt();
      commaIndex = nextCommaIndex + 1;
      nextCommaIndex = data.indexOf(',', commaIndex);
      parseCount++;
    }
    
    // Grab the final value after the last comma
    if (parseCount == NUM_MOTORS - 1) {
      parsedValues[parseCount] = data.substring(commaIndex).toInt();
      
      // Constrain values for safety and write to motors
      for (int i = 0; i < NUM_MOTORS; i++) {
        currentPWM[i] = constrain(parsedValues[i], 1000, 2000);
        motors[i].writeMicroseconds(currentPWM[i]);
      }
      lastCommandTime = millis(); // Reset the failsafe timer
    }
  }

  // FAILSAFE: If no serial data is received for 500ms, force all motors to neutral
  if (millis() - lastCommandTime > FAILSAFE_TIMEOUT_MS) {
    for (int i = 0; i < NUM_MOTORS; i++) {
      motors[i].writeMicroseconds(1500);
    }
  }
}
