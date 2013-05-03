/*
 Motor control for Saphira
 Gordon Good (velo27@yahoo.com)
 - Joystick X axis to analog 0
 - Joystick Y axis to analog 1
 - Joystick button to digital 6
 
 - Right motor controller from digital 9
 - Left motor controller from digital 10
 - LED (or whatever you want to actuate on button press)
   from digital pin 7
   
 - The motors will not move unless pin 2 is
   pulled low. The intent is that unplugging
   the control joystick cable will inhibit
   movement by disconnecting the pulldown for
   that pin. Otherwise, the motors might turn
   on randomly because the analog inputs are
   floating. No one wants a runaway fire-breathing
   animatronic robot. 
 */

#include <Servo.h>

// Inputs
const unsigned int joyXPin = A0;  // Joystick X axis
const unsigned int joyYPin = A1;  // Joystick Y axis
const unsigned int joyButtonPin = 6;  // Joystick button
const unsigned int inhibitPin = 2;  // Must be pulled low to enable motors

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
  Serial.begin(9600);
  pinMode(joyButtonPin, INPUT_PULLUP);
  pinMode(inhibitPin, INPUT_PULLUP);
  pinMode(buttonLEDPin, OUTPUT);
  motorControllerRight.attach(motorRightPin);
  motorControllerRight.write(90);
  motorControllerLeft.attach(motorLeftPin);
  motorControllerLeft.write(90);
}

void loop() {
  // Check for inhibit (pull low to allow movement)

  if (digitalRead(inhibitPin) == HIGH) {
    // Inhibited - wait, then check again
    delay(500);
    return;
  }
  // Read the joystick
  joyX = analogRead(joyXPin); 
  joyY = analogRead(joyYPin);
  joyButton = digitalRead(joyButtonPin);
  
  // If the X axis of the joystick is close to either
  // end of its range of motion, do a turn. Otherwise,
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
    // which is why we reverse the range in the map() call.
    unsigned int value = map(joyY, joyMin, joyMax, motorMax, motorMin);
    motorControllerRight.write(value);
    motorControllerLeft.write(value);
    Serial.print("Forward ");
    Serial.println(value);
  }
  digitalWrite(buttonLEDPin, !joyButton);
  delay(10);                     
}
