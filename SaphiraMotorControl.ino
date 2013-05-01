/*
 Motor control for Saphira.
 */

#include <Servo.h>

// Inputs
const unsigned int joyXPin = A0;  // Joystick X axis
const unsigned int joyYPin = A1;  // Joystick Y axis
const unsigned int joyButtonPin = 6;  // Joystick button

// Input ranges
const unsigned int joyMin = 0;
const unsigned int joyMax = 1023;
const unsigned int joyMid = (joyMax - joyMid) / 2;

// Outputs
const unsigned int motorRightPin = 9;  // PWM output for right motor
const unsigned int motorLeftPin = 10;  // PWM output for left motor
const unsigned int buttonLEDPin = 7;  // Turn on an LED when button pressed

// Output ranges
const unsigned int motorMin = 0;
const unsigned int motorMax = 179;

Servo motorControllerRight;
Servo motorControllerLeft;

unsigned int joyX = 0;  // X joystick value
unsigned int joyY = 0;  // Y joystick value
unsigned int joyButton = 0;  // Joystick button value

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(joyButtonPin, INPUT_PULLUP);
  pinMode(buttonLEDPin, OUTPUT);
  motorControllerRight.attach(motorRightPin);
  motorControllerRight.write(90);
  motorControllerLeft.attach(motorLeftPin);
  motorControllerLeft.write(90);
}

void loop() {
  // Read the joystick
  joyX = analogRead(joyXPin); 
  joyY = analogRead(joyYPin);
  joyButton = digitalRead(joyButtonPin);
  
  // If the X axis of the joystick is close to either
  // end of it range of motion, do a turn. Otherwise,
  // go forward or backward depending on the Y axis
  // value.
  
  if  (joyX < joyMin + 10){
    // Turn left - forward right, backward left
    motorControllerRight.write(motorMax);
    motorControllerLeft.write(motorMin);
    Serial.println("Left");
  } else if (joyX > joyMax - 10) {
    // Turn right - backward right, forward left
    motorControllerRight.write(motorMin);
    motorControllerLeft.write(motorMax);
    Serial.println("Right");
  } else {
    // Go forward/backward, according to Y axis. Send
    // both motors the same value. Our Y axis is "upside down"
    // hence reversing the rane 
    unsigned int value = map(joyY, joyMin, joyMax, motorMax, motorMin);
    motorControllerRight.write(value);
    motorControllerLeft.write(value);
    Serial.print("Forward ");
    Serial.println(value);
  }
  digitalWrite(buttonLEDPin, !joyButton);
  delay(10);                     
}
