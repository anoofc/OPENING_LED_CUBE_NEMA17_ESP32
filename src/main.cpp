#define DEBUG         1

#define BT_NAME       "OPENING_Cube"

#define TRIGGER       34
#define RESET         35

#define SENSOR        13
#define THRESHOLD     2000

#define LIMIT_1       32
#define LIMIT_2       33
#define LIMIT_3       25
#define LIMIT_4       26

#define MOTOR_1_DIR   27
#define MOTOR_1_PUL   14

#define MOTOR_2_DIR   22
#define MOTOR_2_PUL   23

#define MOTOR_3_DIR   18
#define MOTOR_3_PUL   4

#define MOTOR_4_DIR   21
#define MOTOR_4_PUL   19



#define TRIGGER_COMMAND       'T'
#define TRIGGER_END_COMMAND   'E'

#include <Arduino.h>
#include <Preferences.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
Preferences preferences;

bool homing = false;
bool launchSuccess = false;
bool reset = true;


uint32_t stepper1_steps = 10000;
uint32_t stepper2_steps = 10000;
uint32_t stepper3_steps = 10000;
uint32_t stepper4_steps = 10000;
uint32_t timerMillis    = 0;
uint32_t RAMP_STEPS     = 2000;        // How many steps to spend accelerating/decelerating
const uint16_t START_DELAY = 600;      // Slowest speed (microseconds)
const uint16_t TARGET_DELAY = 50;     // Your original speed (microseconds)

uint16_t resetTime = 60;


bool lastTriggerState = LOW;
uint32_t lastMillis = 0;


void saveConfig() {
  preferences.begin("CUBE", false);
  preferences.putUInt("step1", stepper1_steps); // Save input port
  preferences.putUInt("step2", stepper2_steps); // Save output port
  preferences.putUInt("step3", stepper3_steps); // Save output port
  preferences.putUInt("step4", stepper4_steps); // Save output port
  preferences.putUInt("accel", RAMP_STEPS);     // Save acceleration/deceleration steps
  preferences.putUInt("time", resetTime);     // Save reset time
  preferences.end();
}

void getConfig() {
  preferences.begin("CUBE", true);
  stepper1_steps = preferences.getUInt("step1", 10000); // Load input port
  stepper2_steps = preferences.getUInt("step2", 10000); // Load output port
  stepper3_steps = preferences.getUInt("step3", 10000); // Load output port
  stepper4_steps = preferences.getUInt("step4", 10000); // Load output port
  RAMP_STEPS     = preferences.getUInt("accel", 2000); // Load acceleration/deceleration steps
  resetTime      = preferences.getUInt("time",  60); // Load reset time
  preferences.end();

  SerialBT.println("✅ Configuration Loaded:");
  SerialBT.println("Stepper 1 Steps: " + String(stepper1_steps));
  SerialBT.println("Stepper 2 Steps: " + String(stepper2_steps));
  SerialBT.println("Stepper 3 Steps: " + String(stepper3_steps));
  SerialBT.println("Stepper 4 Steps: " + String(stepper4_steps));
  SerialBT.println("Acceleration Steps: " + String(RAMP_STEPS));
  SerialBT.println("Reset Time (S): " + String(resetTime));
}

void loadConfig() {
  preferences.begin("CUBE", true);
  stepper1_steps = preferences.getUInt("step1", 10000); // Load input port
  stepper2_steps = preferences.getUInt("step2", 10000); // Load output port
  stepper3_steps = preferences.getUInt("step3", 10000); // Load output port
  stepper4_steps = preferences.getUInt("step4", 10000); // Load output port
  RAMP_STEPS     = preferences.getUInt("accel", 2000); // Load acceleration/deceleration steps
  resetTime      = preferences.getUInt("time", 60); // Load reset time
  preferences.end();
}

// void launchSequence(){
//   if (DEBUG){ Serial.println("Launch sequence initiated."); }
//   // LAUNCH STEPPER ONE
//   for (uint32_t i = 0; i < stepper1_steps; i++){
//     digitalWrite(MOTOR_1_DIR, HIGH); // Set direction away from home
//     digitalWrite(MOTOR_1_PUL, HIGH);
//     delayMicroseconds(100); // Pulse width
//     digitalWrite(MOTOR_1_PUL, LOW);
//     delayMicroseconds(100); // Time between pulses
//     }
//   if (DEBUG){ Serial.println("Stepper 1 launched."); }
//   // LAUNCH STEPPER TWO
//   for (uint32_t i = 0; i < stepper2_steps; i++){
//     digitalWrite(MOTOR_2_DIR, HIGH); // Set direction away from home
//     digitalWrite(MOTOR_2_PUL, HIGH);
//     delayMicroseconds(100); // Pulse width
//     digitalWrite(MOTOR_2_PUL, LOW);
//     delayMicroseconds(100); // Time between pulses
//   }
//   if (DEBUG){ Serial.println("Stepper 2 launched."); }
//   // LAUNCH STEPPER THREE
//   for (uint32_t i = 0; i < stepper3_steps; i++){
//     digitalWrite(MOTOR_3_DIR, HIGH); // Set direction away from home
//     digitalWrite(MOTOR_3_PUL, HIGH);
//     delayMicroseconds(100); // Pulse width
//     digitalWrite(MOTOR_3_PUL, LOW);
//     delayMicroseconds(100); // Time between pulses
//   }
//   if (DEBUG){ Serial.println("Stepper 3 launched."); }
//   // LAUNCH STEPPER FOUR
//   for (uint32_t i = 0; i < stepper4_steps; i++){
//     digitalWrite(MOTOR_4_DIR, HIGH); // Set direction away from home
//     digitalWrite(MOTOR_4_PUL, HIGH);
//     delayMicroseconds(100); // Pulse width
//     digitalWrite(MOTOR_4_PUL, LOW);
//     delayMicroseconds(100); // Time between pulses
//   }
//   if (DEBUG){ Serial.println("Stepper 4 launched."); }
// }
// Configuration for the ramp

void moveStepperWithRamp(uint8_t pulsePin, uint8_t dirPin, uint32_t totalSteps, bool direction) {
  digitalWrite(dirPin, direction);

  for (uint32_t i = 0; i < totalSteps; i++) {
    uint32_t currentDelay = TARGET_DELAY;

    // Acceleration Phase
    if (i < RAMP_STEPS && i < totalSteps / 2) {
      currentDelay = map(i, 0, RAMP_STEPS, START_DELAY, TARGET_DELAY);
    } 
    // Deceleration Phase
    else if (i > totalSteps - RAMP_STEPS && i > totalSteps / 2) {
      currentDelay = map(i, totalSteps - RAMP_STEPS, totalSteps, TARGET_DELAY, START_DELAY);
    }

    // Pulse
    digitalWrite(pulsePin, HIGH);
    delayMicroseconds(currentDelay);
    digitalWrite(pulsePin, LOW);
    delayMicroseconds(currentDelay);
  }
}

void launchSequence() {
  if (DEBUG) Serial.println("Launch sequence initiated.");

  // LAUNCH STEPPER ONE
  moveStepperWithRamp(MOTOR_1_PUL, MOTOR_1_DIR, stepper1_steps, HIGH);
  if (DEBUG) Serial.println("Stepper 1 launched.");

  // LAUNCH STEPPER TWO
  moveStepperWithRamp(MOTOR_2_PUL, MOTOR_2_DIR, stepper2_steps, HIGH);
  if (DEBUG) Serial.println("Stepper 2 launched.");

  // LAUNCH STEPPER THREE
  moveStepperWithRamp(MOTOR_3_PUL, MOTOR_3_DIR, stepper3_steps, HIGH);
  if (DEBUG) Serial.println("Stepper 3 launched.");

  // LAUNCH STEPPER FOUR
  moveStepperWithRamp(MOTOR_4_PUL, MOTOR_4_DIR, stepper4_steps, HIGH);
  if (DEBUG) Serial.println("Stepper 4 launched.");

  if (DEBUG) Serial.println("Launch sequence completed.");
}

void resetLaunch(){
  if (DEBUG){ Serial.println("RESETTING"); }
  moveStepperWithRamp(MOTOR_1_PUL, MOTOR_1_DIR, stepper1_steps, LOW);
  if (DEBUG){ Serial.println("Stepper 1 reset to home."); }
  moveStepperWithRamp(MOTOR_2_PUL, MOTOR_2_DIR, stepper2_steps, LOW);
  if (DEBUG){ Serial.println("Stepper 2 reset to home."); }
  moveStepperWithRamp(MOTOR_3_PUL, MOTOR_3_DIR, stepper3_steps, LOW);
  if (DEBUG){ Serial.println("Stepper 3 reset to home."); }
  moveStepperWithRamp(MOTOR_4_PUL, MOTOR_4_DIR, stepper4_steps, LOW);

  if (DEBUG){ Serial.println("RESET COMPLETE"); }
}


void readInputs(){
  if ((digitalRead(TRIGGER)==HIGH) && ((millis() - lastMillis) > 500)){
    // Trigger pressed
    if (DEBUG){ Serial.println("Trigger pressed."); }

    lastMillis = millis();
    
    if (!launchSuccess && reset){
      Serial2.println(TRIGGER_COMMAND);
      launchSequence();
      launchSuccess = true;
      reset = false;
      timerMillis = millis();
      Serial2.println(TRIGGER_END_COMMAND);
    }
    if (DEBUG){ Serial.println("Launch successful."); }
  }

  if ((digitalRead(RESET)==HIGH) && ((millis() - lastMillis) > 500)){
    // Reset pressed
    if (DEBUG){ Serial.println("Reset pressed."); }

    lastMillis = millis();
    
    if (!reset && launchSuccess){
      resetLaunch();
      launchSuccess = false;
      reset = true;
    }
    if (DEBUG){ Serial.println("Reset successful."); }
  }
  // if (!launchSuccess && (digitalRead(TRIGGER) == LOW) && (lastTriggerState == HIGH) && ((millis() - lastMillis) > 500)){
  //   // Trigger pressed
  //   lastMillis = millis();
  //   lastTriggerState = LOW;
  //   launchSequence();
  //   launchSuccess = true;
  //   if (DEBUG){ Serial.println("Launch successful."); }
  // }
  // else if (digitalRead(TRIGGER) == HIGH){
  //   lastTriggerState = HIGH;
  // }
}


void homingSequence(){
  // TODO: Implement homing sequence
  while(digitalRead(LIMIT_1) == HIGH){
    // Move motor 1 towards home
    digitalWrite(MOTOR_1_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_1_PUL, HIGH);
    delayMicroseconds(200); // Pulse width
    digitalWrite(MOTOR_1_PUL, LOW);
    delayMicroseconds(200); // Time between pulses 
  } 
  if (DEBUG){ Serial.println("Motor 1 homed."); }

  while(digitalRead(LIMIT_2) == HIGH){
    // Move motor 2 towards home
    digitalWrite(MOTOR_2_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_2_PUL, HIGH);
    delayMicroseconds(200); // Pulse width
    digitalWrite(MOTOR_2_PUL, LOW);
    delayMicroseconds(200); // Time between pulses 
  }
  if (DEBUG){ Serial.println("Motor 2 homed."); }

  while(digitalRead(LIMIT_3) == HIGH){
    digitalWrite(MOTOR_3_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_3_PUL, HIGH);
    delayMicroseconds(200); // Pulse width
    digitalWrite(MOTOR_3_PUL, LOW);
    delayMicroseconds(200); // Time between pulses
  }
  if (DEBUG){ Serial.println("Motor 3 homed."); }

  while(digitalRead(LIMIT_4) == HIGH){
    digitalWrite(MOTOR_4_DIR, LOW); // Set direction towards home
    digitalWrite(MOTOR_4_PUL, HIGH);
    delayMicroseconds(200); // Pulse width
    digitalWrite(MOTOR_4_PUL, LOW);
    delayMicroseconds(200); // Time between pulses
  }
  if (DEBUG){ Serial.println("Motor 4 homed."); }
}

void checkTimer(){
  if (millis() - timerMillis >= resetTime * 1000 && resetTime > 0){
    resetLaunch();
    launchSuccess = false;
    reset = true;
  }
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
  else if (data.startsWith("SET_ACCEL")){
    RAMP_STEPS = data.substring(10, data.length()).toInt(); 
    SerialBT.println("✅ Acceleration/Deceleration steps set to " + String(RAMP_STEPS));
    saveConfig();
  }
  else if (data.startsWith("SET_TIME")){
    uint16_t time = data.substring(9, data.length()).toInt(); 
    
  }
  else if (data == "GET_CONFIG"){
    getConfig();
  }
  else if (data == "L"){
    if (!launchSuccess && reset){ 
      Serial2.println(TRIGGER_COMMAND);
      launchSequence();
      launchSuccess = true;
      timerMillis = millis();
      reset = false;
      Serial2.println(TRIGGER_END_COMMAND);
    }
  }
  else if (data == "R"){
    if (!reset && launchSuccess){ 
      resetLaunch();
      launchSuccess = false;
      reset = true;
    }
  }
  else if (data == "A1"){ moveStepperWithRamp(MOTOR_1_PUL, MOTOR_1_DIR, stepper1_steps, HIGH); }
  else if (data == "B1"){ moveStepperWithRamp(MOTOR_2_PUL, MOTOR_2_DIR, stepper2_steps, HIGH); }
  else if (data == "C1"){ moveStepperWithRamp(MOTOR_3_PUL, MOTOR_3_DIR, stepper3_steps, HIGH); }
  else if (data == "D1"){ moveStepperWithRamp(MOTOR_4_PUL, MOTOR_4_DIR, stepper4_steps, HIGH); }

  else if (data == "A0"){ moveStepperWithRamp(MOTOR_1_PUL, MOTOR_1_DIR, stepper1_steps, LOW);  }
  else if (data == "B0"){ moveStepperWithRamp(MOTOR_2_PUL, MOTOR_2_DIR, stepper2_steps, LOW);  }
  else if (data == "C0"){ moveStepperWithRamp(MOTOR_3_PUL, MOTOR_3_DIR, stepper3_steps, LOW);  }
  else if (data == "D0"){ moveStepperWithRamp(MOTOR_4_PUL, MOTOR_4_DIR, stepper4_steps, LOW);  }

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
      if (!reset && launchSuccess){ 
        resetLaunch();
        launchSuccess = false;
        reset = true;
      }
       
    } else if (command == 'l'){
      if (!launchSuccess && reset){ 
        Serial2.println(TRIGGER_COMMAND);
        launchSequence();
        timerMillis = millis();
        launchSuccess = true;
        reset = false;
        Serial2.println(TRIGGER_END_COMMAND);
      }
    }
  }
}


void readSensor(){
  static uint32_t sensorTriggerMillis = 0;
  if ((digitalRead(SENSOR)==LOW) && ((millis() - sensorTriggerMillis) > 500)){
    // Trigger pressed
    if (DEBUG){ Serial.println("Sensor pressed."); }

    sensorTriggerMillis = millis();
    
    if (!launchSuccess && reset){
      Serial2.println(TRIGGER_COMMAND);
      launchSequence();
      launchSuccess = true;
      reset = false;
      timerMillis = millis();
      Serial2.println(TRIGGER_END_COMMAND);
    }
    if (DEBUG){ Serial.println("Launch successful with Sensor."); }
  }
}

void inputCheck(){
  Serial.println("TRIG: " + String(digitalRead(TRIGGER)) + 
                 " RESET: " + String(digitalRead(RESET)) +
                 " L1: " + String(digitalRead(LIMIT_1)) +
                 " L2: " + String(digitalRead(LIMIT_2)) +
                 " L3: " + String(digitalRead(LIMIT_3)) +
                 " L4: " + String(digitalRead(LIMIT_4))
                );  
}

void initGPIO(){
  pinMode(TRIGGER, INPUT_PULLUP);
  pinMode(RESET, INPUT_PULLUP);
  pinMode(SENSOR, INPUT_PULLUP);

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
  
  homingSequence(); // Perform homing sequence
  delay(500);
  if (DEBUG){ Serial.println("Homing sequence completed."); }
}

void loop(){
  checkTimer();       // Check for reset timer
  readSensor();       // Read sensor input
  readInputs();       // Read physical inputs
  readSerial();       // Read Serial inputs
  readBTSerial();     // Read Bluetooth Serial inputs

  //  inputCheck();      // DEBUG: Check input states
}