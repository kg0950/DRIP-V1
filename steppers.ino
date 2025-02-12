#include <Arduino.h>
  #include "A4988.h"

  #define RELAY_PIN_1 A0
  #define RELAY_PIN_2 A1
  #define RELAY_PIN_3 A3 // Additional relay on pin A3
  #define PUMP_PIN A2
  #define IR_SENSOR_PIN_1 A4
  #define IR_SENSOR_PIN_2 A5

  int Step1 = 3;
  int Dire1 = 2;
  int Sleep1 = 4;
  int MS11 = 7; // GPIO7 in Arduino UNO --- MS1 for A4988
  int MS21 = 6; // GPIO6 in Arduino UNO --- MS2 for A4988
  int MS31 = 5; // GPIO5 in Arduino UNO --- MS3 for A4988

  int Step2 = 9;  // GPIO3 in Arduino UNO --- Step of stepper motor driver
  int Dire2 = 8;  // GPIO2 in Arduino UNO --- Direction of stepper motor driver
  int Sleep2 = 10; // GPIO4 in Arduino UNO --- Control Sleep Mode on A4988
  int MS12 = 13;  // GPIO7 in Arduino UNO --- MS1 for A4988
  int MS22 = 12;  // GPIO6 in Arduino UNO --- MS2 for A4988
  int MS32 = 11;  // GPIO5 in Arduino UNO --- MS3 for A4988

  // Motor Specs
  const int spr = 250;    // Steps per revolution
  int RPM = 15;            // Motor Speed in revolutions per minute
  int Microsteps = 1;      // Stepsize (1 for full steps, 2 for half steps, 4 for quarter steps, etc)

  // Providing parameters for motor control
  A4988 stepper1(spr, Dire1, Step1, MS11, MS21, MS31);
  A4988 stepper2(spr, Dire2, Step2, MS12, MS22, MS32);

  unsigned long previousMillisMotor = 0;
  unsigned long previousMillisPizo = 0;
  const long motorInterval = 10; // Interval at which to move motors (milliseconds)
  const long pizoOnTime = 60000; // 5 seconds
  const long pizoOffTime = 5000;  // 5 seconds
  const long initialPizoDuration = 5000; // Initial piezo buzzer duration

  bool relay3State = false; // State of relay connected to pin A3
  unsigned long lastRelay3ToggleTime = 0; // Last time relay3 was toggled

  void setup() {
    Serial.begin(9600);
    
    // Motor setup
    pinMode(Step1, OUTPUT);  // Step pin as output
    pinMode(Dire1, OUTPUT);  // Direction pin as output
    pinMode(Sleep1, OUTPUT); // Set Sleep OUTPUT Control button as output
    digitalWrite(Step1, LOW);  // Currently no stepper motor movement
    digitalWrite(Dire1, LOW);
    digitalWrite(Sleep1, HIGH); // A logic high allows normal operation of the A4988 by removing from sleep
    
    pinMode(Step2, OUTPUT);  // Step pin as output
    pinMode(Dire2, OUTPUT);  // Direction pin as output
    pinMode(Sleep2, OUTPUT); // Set Sleep OUTPUT Control button as output
    digitalWrite(Step2, LOW);  // Currently no stepper motor movement
    digitalWrite(Dire2, LOW);
    digitalWrite(Sleep2, HIGH); // A logic high allows normal operation of the A4988 by removing from sleep
    
    // Set target motor RPM to and microstepping setting
    stepper1.begin(RPM, Microsteps);
    stepper2.begin(RPM, Microsteps);

    // Relay setup
    pinMode(RELAY_PIN_1, OUTPUT);
    pinMode(RELAY_PIN_2, OUTPUT);
    pinMode(RELAY_PIN_3, OUTPUT); // Additional relay pin setup
    digitalWrite(RELAY_PIN_1, HIGH); // Turn on relays immediately after board power-up
    digitalWrite(RELAY_PIN_2, HIGH);
    digitalWrite(RELAY_PIN_3, HIGH); // Turn off the additional relay initially
    
    // Piezo buzzer setup
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, HIGH); // Make sure the buzzer is initially off
    
    // IR sensor setup
    pinMode(IR_SENSOR_PIN_1, INPUT_PULLUP);
    pinMode(IR_SENSOR_PIN_2, INPUT_PULLUP);
    
    Serial.println("Setup complete.");
    
    // Initial piezo buzzer activation
    digitalWrite(PUMP_PIN, LOW);
    delay(initialPizoDuration);
    //digitalWrite(PUMP_PIN, LOW);
  }

void loop() {
  unsigned long currentMillis = millis();
  
  // Delay for 10 seconds before starting motor movement
  if (currentMillis > 0) {
    // Move motors in direction -1
    digitalWrite(Sleep1, HIGH);
    digitalWrite(Sleep2, HIGH); // A logic high allows normal operation of the A4988 by removing from sleep
    stepper1.move(-1); // Move motor1 counterclockwise
    stepper2.move(-1); // Move motor2 clockwise
    digitalWrite(RELAY_PIN_1, LOW);
    digitalWrite(RELAY_PIN_2, LOW);
    digitalWrite(RELAY_PIN_3, LOW); // Turn on the additional relay on pin A3
    relay3State = true;

    // Check if it's time to toggle the relay connected to pin A3
    if (relay3State && currentMillis - lastRelay3ToggleTime >= 1000) { // Check every second
      lastRelay3ToggleTime = currentMillis;
      toggleRelay3(); // Function to toggle the relay connected to pin A3
    }

    // Delay for motor interval
    delay(motorInterval);
  }
  else {
    // Read IR sensor states
    bool irSensor1State = digitalRead(IR_SENSOR_PIN_1);
    bool irSensor2State = digitalRead(IR_SENSOR_PIN_2); 

    // Motor movement control
    if (irSensor1State == 1) {
      // Move motors in direction -1 until IR sensor 2 is triggered
      while (!irSensor2State) {
        // Move motors one step
        digitalWrite(Sleep1, HIGH);
        digitalWrite(Sleep2, HIGH); // A logic high allows normal operation of the A4988 by removing from sleep
        stepper1.move(-1); // Move motor1 counterclockwise
        stepper2.move(-1); // Move motor2 clockwise
        digitalWrite(RELAY_PIN_1, LOW);
        digitalWrite(RELAY_PIN_2, LOW);
        digitalWrite(RELAY_PIN_3, LOW); // Turn on the additional relay on pin A3
        relay3State = true;

        // Update IR sensor states
        irSensor1State = digitalRead(IR_SENSOR_PIN_1);
        irSensor2State = digitalRead(IR_SENSOR_PIN_2); 

        // Check if it's time to toggle the relay connected to pin A3
        if (relay3State && currentMillis - lastRelay3ToggleTime >= 1000) { // Check every second
          lastRelay3ToggleTime = currentMillis;
          toggleRelay3(); // Function to toggle the relay connected to pin A3
        }

        // Delay for motor interval
        delay(motorInterval);
      }
    } 
    else if (irSensor2State == 1) {
      // Move motors in direction 1 until IR sensor 1 is triggered
      while (!irSensor1State) {
        // Move motors one step
        digitalWrite(Sleep1, HIGH);
        digitalWrite(Sleep2, HIGH); // A logic high allows normal operation of the A4988 by removing from sleep
        stepper1.move(1); // Move motor1 clockwise
        stepper2.move(1); // Move motor2 clockwise
        digitalWrite(RELAY_PIN_1, LOW);
        digitalWrite(RELAY_PIN_2, LOW);
        digitalWrite(RELAY_PIN_3, LOW); // Turn on the additional relay on pin A3
        relay3State = true;

        // Update IR sensor states
        irSensor1State = digitalRead(IR_SENSOR_PIN_1);
        irSensor2State = digitalRead(IR_SENSOR_PIN_2); 

        // Check if it's time to toggle the relay connected to pin A3
        if (relay3State && currentMillis - lastRelay3ToggleTime >= 1000) { // Check every second
          lastRelay3ToggleTime = currentMillis;
          toggleRelay3(); // Function to toggle the relay connected to pin A3
        }

        // Delay for motor interval
        delay(motorInterval);
      }
    }

    // Piezo buzzer control
    unsigned long elapsedPizoTime = currentMillis - previousMillisPizo;
    if (elapsedPizoTime < pizoOnTime) {
      // Turn on the piezo buzzer
      digitalWrite(PUMP_PIN, HIGH);
    } else if (elapsedPizoTime < pizoOnTime + pizoOffTime) {
      // Turn off the piezo buzzer
      digitalWrite(PUMP_PIN, LOW);
    } else {
      // Restart the cycle
      previousMillisPizo = currentMillis;
    }
  }
}

  // Function to toggle the relay connected to pin A3
  void toggleRelay3() {
    if (relay3State) {
      // Turn off relay
      digitalWrite(RELAY_PIN_3, HIGH);
      relay3State = false;
    } else {
      // Turn on relay
      digitalWrite(RELAY_PIN_3, LOW);
      relay3State = true;
    }
  }
