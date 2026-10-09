#include <Servo.h>
#include "DFRobotDFPlayerMini.h"
bool running = true;
#define mySerial Serial1

// Output Pins
const int MOTOR_PIN = 4;
const int GATE_SERVO_PIN = 5; //Done
const int KRAB_SERVO_PIN = 23; //Done
const int FLIPPER_L_PIN = 2; //Done
const int FLIPPER_R_PIN = 8; //Done
const int DS_PIN = 11;
const int STCP_PIN = 12;
const int SHCP_PIN = 13;
const int SCREEN1_PIN = 10;
const int SCREEN2_PIN = 9;

// Input Pins
const int KRAB_LED_PIN = 3; //Done
const int GATE_LED_PIN = 22; //Done
const int PIEZO1_PIN = A0;  //Done
const int PIEZO2_PIN = A1;  //Done
const int BUTTON_L_PIN = 40; //Done
const int BUTTON_R_PIN = 47; //Done

Servo gateServo;
int gateServoAngle;
Servo krabServo;
int krabServoAngle;

DFRobotDFPlayerMini myDFPlayer;

const unsigned long debounceDelay = 20;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  // Servo
  gateServo.attach(GATE_SERVO_PIN);
  gateServo.write(0);
  krabServo.attach(KRAB_SERVO_PIN);
  krabServo.write(0);
  Serial.println("Servo Connected");
  // Inputs
  pinMode(KRAB_LED_PIN, INPUT_PULLUP);
  pinMode(GATE_LED_PIN, INPUT_PULLUP);
  pinMode(PIEZO1_PIN, INPUT);
  pinMode(PIEZO2_PIN, INPUT);
  pinMode(BUTTON_L_PIN, INPUT_PULLUP);
  pinMode(BUTTON_R_PIN, INPUT_PULLUP);
  Serial.println("Inputs Connected");
  // Outputs
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(FLIPPER_L_PIN, OUTPUT);
  pinMode(FLIPPER_R_PIN, OUTPUT);
  pinMode(DS_PIN, OUTPUT);
  pinMode(STCP_PIN, OUTPUT);
  pinMode(SHCP_PIN, OUTPUT);
  pinMode(SCREEN1_PIN, OUTPUT);
  pinMode(SCREEN2_PIN, OUTPUT);


  // Check if the module is responding and if the SD card is found
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer not detected");
    while(true);
  }
  myDFPlayer.setTimeOut(500);  // Serial timeout 500ms
  myDFPlayer.volume(100);        // Volume 5
  myDFPlayer.EQ(0);            // Normal equalization

  Serial.println("System Ready.");
}
int displayVal = 0;
int point = 0;
int round_count = 0;

void piezo_points(int piezo1, int piezo2){
  if(piezo1 > 150){
    point += 1;
    myDFPlayer.playFolder(1, 003);
  }
  if(piezo2 > 20){
    point += 1;
    myDFPlayer.playFolder(1, 001);
  }
}

// int piezoThresh = 950;
// unsigned long lastDebounceTime_piezo1 = 0;
// int lastSteadyState_piezo1 = LOW;
// int lastRawState_piezo1 = LOW;
// void control_piezo(int piezoNo, int piezoVal){
//   if(piezoNo == 1){
//     if(piezoVal > piezoThresh)
// if (input != lastRawState_LED_gate) {
//     lastDebounceTime_LED_gate = millis(); // Reset timer
//   }
//   if ((millis() - lastDebounceTime_LED_gate) > debounceDelay) {
//     if (input != lastSteadyState_LED_gate) {
//       lastSteadyState_LED_gate = input;
//       if (lastSteadyState_LED_gate == HIGH) {
//         round_count += 1;
//         gate_timer = millis();
//         if(round_count >= 3){
//           gateServo.write(180);
//           gate_timer = millis();
//         }else{
//           gateServo.write(0);
//           gate_timer = millis();
//         }
//       }
//     }
//   }
//   lastRawState_LED_gate = input;
//   }
//   if(piezoNo == 2){

//   }
// }

void loop() {
  int buttonL_sense = digitalRead(BUTTON_L_PIN);
  int buttonR_sense = digitalRead(BUTTON_R_PIN);
  int krab_LED_sense = digitalRead(KRAB_LED_PIN);
  int gate_LED_sense = digitalRead(GATE_LED_PIN);
  int piezo1_sense = analogRead(PIEZO1_PIN);
  int piezo2_sense = analogRead(PIEZO2_PIN);
  if(buttonL_sense == LOW && buttonR_sense == LOW && gate_LED_sense == HIGH){
    running = true;
    restart();
  }
  if(running){
    control_run();
    control_gate(gate_LED_sense);
    control_button_L(buttonL_sense);
    control_button_R(buttonR_sense);
    control_krab_arm(krab_LED_sense);
    digitalWrite(MOTOR_PIN, 2);
    piezo_points(piezo1_sense, piezo2_sense);

    writeToScreen(point);
    if(point >= 100){ 
      point = 0; 
      running = false;
    } // end game, plays a sound, then stops the running

    // Serial.print("Piezo1:");
    // Serial.println(piezo1_sense);
    // Serial.print("Piezo2:");
    Serial.println(gate_LED_sense);
    // Serial.print("LED:");
    // Serial.println(buttonL_sense);
    // Serial.print("---------------------points--------------------");
    // Serial.println(point);




    delay(10);
  }else{// to start press both bottom at once
    if(buttonL_sense == LOW && buttonR_sense == LOW && gate_LED_sense == HIGH){
      running = true;
      restart();
    }
    writeToScreen(0);
  }
}

// Serial force stop code
void control_run(){
  if (Serial.available() > 0) {
    char incomingByte = Serial.read(); // Read the incoming byte
    // Check if the character is 'S' (for Stop)
    if (incomingByte == 'S') {
      running = false; // Set the flag to false to stop the process
      Serial.println("Stop command received. Program paused.");
    }
    // Check if the character is 'R' (for Run/Restart)
    else if (incomingByte == 'R') {
      running = true; // Set the flag to true to resume
      Serial.println("Run command received. Program resumed.");
    }
  }
}

// Display mechanism
int digits [10][8] {
  {0,1,1,1,1,1,1,0}, // 0
  {0,0,1,1,0,0,0,0}, // 1
  {0,1,1,0,1,1,0,1}, // 2
  {0,1,1,1,1,0,0,1}, // 3
  {0,0,1,1,0,0,1,1}, // 4
  {0,1,0,1,1,0,1,1}, // 5
  {0,1,0,1,1,1,1,1}, // 6
  {0,1,1,1,0,0,0,0}, // 7
  {0,1,1,1,1,1,1,1}, // 8
  {0,1,1,1,1,0,1,1}, // 9
};
int dec_digits [10] {126,48,109,121,51,91,95,112,127,123};
void writeToScreen(int value){
  int tens = value/10;
  int ones = value%10;
  digitalWrite(SCREEN1_PIN, HIGH);
  digitalWrite(SCREEN2_PIN, LOW);
  digitalWrite(STCP_PIN,LOW);
  shiftOut(DS_PIN, SHCP_PIN, LSBFIRST, dec_digits[tens]);
  digitalWrite(STCP_PIN,HIGH);
  delay(5);
  digitalWrite(SCREEN1_PIN, LOW);
  digitalWrite(SCREEN2_PIN, HIGH);
  digitalWrite(STCP_PIN,LOW);
  shiftOut(DS_PIN, SHCP_PIN, LSBFIRST, dec_digits[ones]);
  digitalWrite(STCP_PIN,HIGH);
  delay(5);
}

// Gating mechanism
unsigned long lastDebounceTime_LED_gate = 0;
int lastSteadyState_LED_gate = LOW;
int lastRawState_LED_gate = LOW;

unsigned long gate_timer = 0;
void control_gate(int input){
  if (input != lastRawState_LED_gate) {
    lastDebounceTime_LED_gate = millis(); // Reset timer
  }
  if ((millis() - lastDebounceTime_LED_gate) > debounceDelay) {
    if (input != lastSteadyState_LED_gate) {
      lastSteadyState_LED_gate = input;
      if (lastSteadyState_LED_gate == HIGH) {
        round_count += 1;
        gate_timer = millis();
        if(round_count > 3){
          gateServo.write(180);
          gate_timer = millis();
        }else{
          gateServo.write(0);
          gate_timer = millis();
        }
        myDFPlayer.playFolder(1, 004);
      }
    }
  }
  lastRawState_LED_gate = input;
  if(millis() - gate_timer > 1000){
    gateServo.write(180);
  }
  Serial.print("round_count-------------------");
  Serial.println(round_count);
  
}

// Krab mechanism
unsigned long lastDebounceTime_LED_krab = 0;
int lastSteadyState_LED_krab = LOW;
int lastRawState_LED_krab = LOW;
int count_krab = 0;
void control_krab_arm(int input){
  if (input != lastRawState_LED_krab) {
    lastDebounceTime_LED_krab = millis(); // Reset timer
  }
  if ((millis() - lastDebounceTime_LED_krab) > debounceDelay) {
    if (input != lastSteadyState_LED_krab) {
      lastSteadyState_LED_krab = input;
      if (lastSteadyState_LED_krab == HIGH) {
        count_krab += 1;
        point += 1;
        myDFPlayer.playFolder(1, 002);
      }
    }
  }
  lastRawState_LED_krab = input;
  int cases = count_krab%2;
  switch(cases){
    case 0: 
      krabServoAngle = 150;
      break;
    case 1: 
      krabServoAngle = 50;
      break;
  }
  // Serial.println(count_krab);
  krabServo.write(krabServoAngle);
}

// Left Button
unsigned long lastDebounceTime_button_L = 0;
int lastSteadyState_button_L = LOW;
int lastRawState_button_L = LOW;
void control_button_L(int input){
  if (input != lastRawState_button_L) { 
    lastDebounceTime_button_L = millis(); 
  }
  if ((millis() - lastDebounceTime_button_L) > debounceDelay) {
    if (input != lastSteadyState_button_L) {
      lastSteadyState_button_L = input;
      if (lastSteadyState_button_L == HIGH) {
        digitalWrite(FLIPPER_L_PIN, LOW);
        // point += 1;
        delay(10);
      }else{
        digitalWrite(FLIPPER_L_PIN, HIGH);
        delay(10);
      }
    }
  }
    lastRawState_button_L = input;
}

// Right Button
unsigned long lastDebounceTime_button_R = 0;
int lastSteadyState_button_R = LOW;
int lastRawState_button_R = LOW;
void control_button_R(int input){
  if (input != lastRawState_button_R) {
    lastDebounceTime_button_R = millis(); // Reset timer
  }
  if ((millis() - lastDebounceTime_button_R) > debounceDelay) {
    if (input != lastSteadyState_button_R) {
      lastSteadyState_button_R = input;
      if (lastSteadyState_button_R == HIGH) {
        digitalWrite(FLIPPER_R_PIN, LOW);
        delay(10);
      }else{
        digitalWrite(FLIPPER_R_PIN, HIGH);
        delay(10);
      }
    }
  }
    lastRawState_button_R = input;
}

void restart(){
    krabServo.write(0);
    gateServo.write(180);
    digitalWrite(MOTOR_PIN, 10);
    round_count = 0;
    point = 0;
    displayVal = 0;
    count_krab = 0;
    gateServo.write(150);
    delay(50);
    gateServo.write(180);

}