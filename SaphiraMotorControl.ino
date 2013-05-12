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
const unsigned int motorTurn = 15;  // Offset for turning

Servo motorControllerRight;
Servo motorControllerLeft;

unsigned int joyX = 0;  // X joystick value
unsigned int joyY = 0;  // Y joystick value
unsigned int joyButton = 0;  // Joystick button value

boolean inhibited = true;

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
    motorControllerRight.write(90);
    motorControllerLeft.write(90);
    inhibited = true;
    Serial.println("Inhibited");
    delay(500);
    return;
  } else if (inhibited) {
    // Coming out of inhibited mode. Wait a bit
    // to avoid intermittent readins when plugging
    // in joystick.
    inhibited = false;
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
    // Turn left - forward right, backward left. But since
    // the motors are mounted 180 degrees apart, turn them
    // both forward.
    motorControllerRight.write(90 + motorTurn);
    motorControllerLeft.write(90 + motorTurn);
    Serial.println("Left");
  } else if (joyX > joyMax - 10) {
    // Turn right - backward right, forward left. 
    motorControllerRight.write(90 - motorTurn);
    motorControllerLeft.write(90 - motorTurn);
    Serial.println("Right");
  } else {
    // Go forward/backward, according to Y axis. Send
    // both motors the same value. Our Y axis is "upside down"
    // which is why we reverse the range in the map() call.
    // Also, enlarge the "dead zone" in the middle of the range
    unsigned int value = map(joyY, joyMin, joyMax, motorMax, motorMin);
    int offset = (int)value - 90;
    if (abs(offset) < 10) {
      offset = 0;
    } else {
      offset = map(offset, 0, 90, 0, 20);
    }
    motorControllerRight.write(90 + offset);
    motorControllerLeft.write(90 - offset);
    Serial.print("Forward: ");
    Serial.print("Raw: ");
    Serial.print(value);
    Serial.print(" R: ");
    Serial.print(90 + offset);
    Serial.print(" L: ");
    Serial.println(90 - offset);
  }
  digitalWrite(buttonLEDPin, !joyButton);
  delay(10);                     
}
