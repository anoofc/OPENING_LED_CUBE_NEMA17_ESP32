#define DEBUG         1

#define BT_NAME  "OPENING_Cube"

#define TRIGGER       4

#define LIMIT_1       32
#define LIMIT_2       33
#define LIMIT_3       25
#define LIMIT_4       26

#define MOTOR_1_DIR   27
#define MOTOR_1_PUL   14

#define MOTOR_2_DIR   13
#define MOTOR_2_PUL   23

#define MOTOR_3_DIR   22
#define MOTOR_3_PUL   21

#define MOTOR_4_DIR   19
#define MOTOR_4_PUL   18

#include <Arduino.h>
#include <Preferences.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
Preferences preferences;

bool homing = false;
bool launchSuccess = false;

uint32_t stepper1_steps = 10000;
uint32_t stepper2_steps = 10000;
uint32_t stepper3_steps = 10000;
uint32_t stepper4_steps = 10000;


bool lastTriggerState = HIGH;
uint32_t lastMillis = 0;


void saveConfig() {
  preferences.begin("CUBE", false);
  preferences.putUInt("step1", stepper1_steps); // Save input port
  preferences.putUInt("step2", stepper2_steps); // Save output port
  preferences.putUInt("step3", stepper3_steps); // Save output port
  preferences.putUInt("step4", stepper4_steps); // Save output port
  preferences.end();
}

void getConfig() {
  preferences.begin("CUBE", true);
  stepper1_steps = preferences.getUInt("step1", 10000); // Load input port
  stepper2_steps = preferences.getUInt("step2", 10000); // Load output port
  stepper3_steps = preferences.getUInt("step3", 10000); // Load output port
  stepper4_steps = preferences.getUInt("step4", 10000); // Load output port
  preferences.end();

  SerialBT.println("✅ Configuration Loaded:");
  SerialBT.println("Stepper 1 Steps: " + String(stepper1_steps));
  SerialBT.println("Stepper 2 Steps: " + String(stepper2_steps));
  SerialBT.println("Stepper 3 Steps: " + String(stepper3_steps));
  SerialBT.println("Stepper 4 Steps: " + String(stepper4_steps));
}

void loadConfig() {
  preferences.begin("CUBE", true);
  preferences.getUInt("step1", stepper1_steps); // Load input port
  preferences.getUInt("step2", stepper2_steps); // Load output port
  preferences.getUInt("step3", stepper3_steps); // Load output port
  preferences.getUInt("step4", stepper4_steps); // Load output port
  preferences.end();
}

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
  if (DEBUG){ Serial.println("Motor 1 homed."); }

  while(digitalRead(LIMIT_2) == HIGH){
    // Move motor 2 towards home
    digitalWrite(MOTOR_2_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_2_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_2_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses 
  }
  if (DEBUG){ Serial.println("Motor 2 homed."); }

  while(digitalRead(LIMIT_3) == HIGH){
    digitalWrite(MOTOR_3_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_3_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_3_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
  }
  if (DEBUG){ Serial.println("Motor 3 homed."); }

  while(digitalRead(LIMIT_4) == HIGH){
    digitalWrite(MOTOR_4_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_4_PUL, HIGH);
    delayMicroseconds(1000); // Pulse width
    digitalWrite(MOTOR_4_PUL, LOW);
    delayMicroseconds(1000); // Time between pulses
  }
  if (DEBUG){ Serial.println("Motor 4 homed."); }
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

void processData(String data){
  if (data.startsWith("SET_STEP1")){
    stepper1_steps = data.substring(10, data.length()).toInt(); SerialBT.println("✅ Stepper 1 steps set to " + String(stepper1_steps));
    saveConfig();
  }
  else if (data.startsWith("SET_STEP2")){
    stepper2_steps = data.substring(10, data.length()).toInt(); SerialBT.println("✅ Stepper 2 steps set to " + String(stepper2_steps));
    saveConfig();
  }
  else if (data.startsWith("SET_STEP3")){
    stepper3_steps = data.substring(10, data.length()).toInt(); SerialBT.println("✅ Stepper 3 steps set to " + String(stepper3_steps));
    saveConfig();
  }
  else if (data.startsWith("SET_STEP4")){
    stepper4_steps = data.substring(10, data.length()).toInt(); SerialBT.println("✅ Stepper 4 steps set to " + String(stepper4_steps));
    saveConfig();
  } 
  else if (data == "GET_CONFIG"){
    getConfig();
  }
  else { SerialBT.println("❌ Unknown command: " + data);}
}

void readBTSerial(){
  if (SerialBT.available()){
    String incoming = SerialBT.readStringUntil('\n');
    incoming.trim();
    processData(incoming);
  }
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

  // Load saved configuration
  loadConfig();

  SerialBT.begin(BT_NAME); // Bluetooth device name
  if (DEBUG){ Serial.println("Bluetooth begin with ID: " + String(BT_NAME)); }  
  
  initGPIO();   // Initialize GPIO pins
  
  // homingSequence(); // Perform homing sequence
  delay(500);
  if (DEBUG){ Serial.println("Homing sequence completed."); }
}

void loop(){
  readInputs();
  readSerial();
  readBTSerial();
  // inputCheck();  // DEBUG: Check input states
}