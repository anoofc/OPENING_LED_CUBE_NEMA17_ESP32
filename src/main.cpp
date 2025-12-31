#define DEBUG     1

#define TRIGGER   4

#define LIMIT_1   32
#define LIMIT_2   33
#define LIMIT_3   25
#define LIMIT_4   26

#define MOTOR_1_DIR   27
#define MOTOR_1_PUL   14

#define MOTOR_2_DIR   13
#define MOTOR_2_PUL   23

#define MOTOR_3_DIR   22
#define MOTOR_3_PUL   21

#define MOTOR_4_DIR   19
#define MOTOR_4_PUL   18

#include <Arduino.h>

bool homing = false;
bool launchSuccess = false;

uint32_t stepper1_steps = 10000;
uint32_t stepper2_steps = 10000;
uint32_t stepper3_steps = 10000;
uint32_t stepper4_steps = 10000;


bool lastTriggerState = HIGH;
uint32_t lastMillis = 0;


void launchSequence(){
  if (DEBUG){ Serial.println("Launch sequence initiated."); }
  // LAUNCH STEPPER ONE
  for (uint32_t i = 0; i < stepper1_steps; i++){
    digitalWrite(MOTOR_1_DIR, HIGH); // Set direction away from home
    digitalWrite(MOTOR_1_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_1_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
    }
  if (DEBUG){ Serial.println("Stepper 1 launched."); }
  // LAUNCH STEPPER TWO
  for (uint32_t i = 0; i < stepper2_steps; i++){
    digitalWrite(MOTOR_2_DIR, HIGH); // Set direction away from home
    digitalWrite(MOTOR_2_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_2_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
  }
  if (DEBUG){ Serial.println("Stepper 2 launched."); }
  // LAUNCH STEPPER THREE
  for (uint32_t i = 0; i < stepper3_steps; i++){
    digitalWrite(MOTOR_3_DIR, HIGH); // Set direction away from home
    digitalWrite(MOTOR_3_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_3_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
  }
  if (DEBUG){ Serial.println("Stepper 3 launched."); }
  // LAUNCH STEPPER FOUR
  for (uint32_t i = 0; i < stepper4_steps; i++){
    digitalWrite(MOTOR_4_DIR, HIGH); // Set direction away from home
    digitalWrite(MOTOR_4_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_4_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
  }
  if (DEBUG){ Serial.println("Stepper 4 launched."); }
}


void readInputs(){
  if (!launchSuccess && (digitalRead(TRIGGER) == LOW) && (lastTriggerState == HIGH) && ((millis() - lastMillis) > 500) && !homing){
    // Trigger pressed
    lastMillis = millis();
    lastTriggerState = LOW;
    launchSequence();
    launchSuccess = true;
    if (DEBUG){ Serial.println("Launch successful."); }
  }
  else if (digitalRead(TRIGGER) == HIGH){
    lastTriggerState = HIGH;
  }
}


void homingSequence(){
  // TODO: Implement homing sequence
  while(digitalRead(LIMIT_1) == HIGH){
    // Move motor 1 towards home
    digitalWrite(MOTOR_1_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_1_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_1_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses 
  } 
  while(digitalRead(LIMIT_2) == HIGH){
    // Move motor 2 towards home
    digitalWrite(MOTOR_2_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_2_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_2_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses 
  }
  while(digitalRead(LIMIT_3) == HIGH){
    digitalWrite(MOTOR_3_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_3_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_3_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
  }
  while(digitalRead(LIMIT_4) == HIGH){
    digitalWrite(MOTOR_4_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_4_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_4_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
  }
}

void resetLaunch(){
  launchSuccess = false;
  if (DEBUG){ Serial.println("Launch reset."); }
  for (int i = stepper1_steps; i > 0; i--){
    digitalWrite(MOTOR_1_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_1_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_1_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
    }
  for (int i = stepper2_steps; i > 0; i--){
    digitalWrite(MOTOR_2_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_2_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_2_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
  }
  for (int i = stepper3_steps; i > 0; i--){
    digitalWrite(MOTOR_3_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_3_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_3_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
  }
  for (int i = stepper4_steps; i > 0; i--){
    digitalWrite(MOTOR_4_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_4_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_4_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
  }
  if (DEBUG){ Serial.println("Steppers reset to home."); }
}

void readSerial(){
  if (Serial.available()){
    char command = Serial.read();
    if (command == 'r'){
      resetLaunch();    
    }
  }
}

void inputCheck(){
  Serial.println("TRIG: " + String(digitalRead(TRIGGER)) + 
                 " L1: " + String(digitalRead(LIMIT_1)) +
                 " L2: " + String(digitalRead(LIMIT_2)) +
                 " L3: " + String(digitalRead(LIMIT_3)) +
                 " L4: " + String(digitalRead(LIMIT_4))
                );  
}

void initGPIO(){
  pinMode(TRIGGER, INPUT_PULLUP);

  pinMode(LIMIT_1, INPUT_PULLUP);
  pinMode(LIMIT_2, INPUT_PULLUP);
  pinMode(LIMIT_3, INPUT_PULLUP);
  pinMode(LIMIT_4, INPUT_PULLUP);

  pinMode(MOTOR_1_DIR, OUTPUT);
  pinMode(MOTOR_1_PUL, OUTPUT);

  pinMode(MOTOR_2_DIR, OUTPUT);
  pinMode(MOTOR_2_PUL, OUTPUT);

  pinMode(MOTOR_3_DIR, OUTPUT);
  pinMode(MOTOR_3_PUL, OUTPUT);

  pinMode(MOTOR_4_DIR, OUTPUT);
  pinMode(MOTOR_4_PUL, OUTPUT);
}

void setup(){
  Serial.begin(115200);
  Serial2.begin(115200);
  
  initGPIO();   // Initialize GPIO pins
  
  homingSequence(); // Perform homing sequence
}

void loop(){
  readInputs();
  readSerial();
  // inputCheck();  // DEBUG: Check input states
}