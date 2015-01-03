/* -*- Mode: c++; c-indent-level: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vim: set shiftwidth=2 tabstop=2 autoindent cindent expandtab: */

// BEGIN STEPPER DECLS

#include <AccelStepper.h>

// Define motor pins.
#define motorPin1  A0     // Blue   - 28BYJ48 pin 1
#define motorPin2  A1     // Pink   - 28BYJ48 pin 2
#define motorPin3  A2     // Yellow - 28BYJ48 pin 3
#define motorPin4  A3     // Orange - 28BYJ48 pin 4

#define motorPin5  6    // Blue   - 28BYJ48 pin 1
#define motorPin6  7     // Pink   - 28BYJ48 pin 2
#define motorPin7  8    // Yellow - 28BYJ48 pin 3
#define motorPin8  9    // Orange - 28BYJ48 pin 4

#define BACKWARD -1
#define FORWARD 1
#define SPEED 1000
#define DELAY 1500  // Minimum delay to allow the motors to complete one step

// Construct stepper objects.
AccelStepper stepper1(AccelStepper::FULL4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper2(AccelStepper::FULL4WIRE, motorPin5, motorPin7, motorPin6, motorPin8);

// END STEPPER DECLS

// BEGIN SERVO DECLS

#include <Servo.h> 

// Construct servo object.
Servo myservo;
int pos = 60;    // Servo position 

// END SERVO DECLS

// ADJUST THIS FOR YOUR BOT
const float CALIBRATE = 3.630;
const float WHEEL_BASE = 131.0; // mm
const float CIRCLE_DIAM = 131.0;
const float CIRCLE_CIRCUM = CIRCLE_DIAM * PI;
const float CIRCLE_STEPS = 13068;  // From console
const float STEP_LENGTH = 2 * CIRCLE_CIRCUM / CIRCLE_STEPS;

int leftStepCount = 0;
int rightStepCount = 0;

void setup(void)
{
  Serial.begin(115200);

  // Initialize steppers and servo.
  stepper1.setMaxSpeed(SPEED);
  stepper2.setMaxSpeed(SPEED);
  myservo.attach(A4);
  penUp();  // Pen up to avoid unintended marks

  // PUT COMMANDS HERE
  circle();
  penUp();
  step(-450, 450);
  move(-50);
  square();
  step(225, -225);
  move(100);
  triangle();
  penUp();  // Pen up to avoid unintended marks
}

void loop(void)
{
}

float millimetersToSteps(float length)
{
  return length / STEP_LENGTH / CALIBRATE;
}

void line(float len)
{
  int steps = millimetersToSteps(len);
  penDown();
  step(steps, steps);
  penUp();
}

void move(float len)
{
  int steps = millimetersToSteps(len);
  penUp();
  step(steps, steps);
}

void circle()
{
  penDown();
  step(-1000, 2600);
  penUp();
}  

void circleLeft()
{
  penDown();
  step(0, 3600);
  penUp();
}  

void circleRight()
{
  penDown();
  step(3600, 0);
  penUp();
}  

void square()
{
  penDown();
  line(100);
  step(450, -450);
  line(100);
  step(450, -450);
  line(100);
  step(450, -450);
  line(100);
  step(450, -450);
  penUp();
}  

void triangle()
{
  penDown();
  line(70);
  step(600, -600);
  line(70);
  step(600, -600);
  line(70);
  step(600, -600);
  penUp();
}  

void penUp()
{
  for(; pos < 60; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                 // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
    // waits 15ms for the servo to reach the position 
  } 
  pos = 60;
  myservo.write(pos);              // tell servo to go to position in variable 'pos' 
}

void penDown()
{
  for(; pos > 0; pos -= 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
    // waits 15ms for the servo to reach the position 
  }
  pos = 0;
  myservo.write(pos);              // tell servo to go to position in variable 'pos' 
}

void step(long lsteps, long rsteps)
{
  lsteps = CALIBRATE * lsteps;
  rsteps = CALIBRATE * rsteps;
  leftStepCount += lsteps;
  rightStepCount += rsteps;
  int dirL = (lsteps > 0) ? BACKWARD : FORWARD;
  int dirR = (rsteps > 0) ? BACKWARD : FORWARD;
  lsteps = abs(lsteps);
  rsteps = abs(rsteps);
  if (lsteps >= rsteps)
  {
    double offset = 0;
    if (rsteps > 0) {
      double delta = double(lsteps - rsteps) / rsteps;  // 3
      for ( ; rsteps > 0; ) {
        offset += delta;
        stepOne(dirL, dirR, SPEED);
        lsteps--;
        rsteps--;
        for(; offset >= 1; offset--) {  // 3 * 0 | 3 * 1
          stepOneLeft(dirL, SPEED);
          lsteps--;
        }
      }
    }
    for(; lsteps > 0; lsteps--) {
      stepOneLeft(dirL, SPEED);
    }
  } 
  else {
    double offset = 0;
    if (lsteps > 0) {
      double delta = double(rsteps - lsteps) / lsteps;
      for ( ; lsteps > 0; ) {
        offset += delta;
        stepOne(dirL, dirR, SPEED);
        lsteps--;
        rsteps--;
        for(; offset >= 1; offset--) {  // 3 * 0 | 3 * 1
          stepOneRight(dirR, SPEED);
          rsteps--;
        }
      }
    }
    for(; rsteps > 0; rsteps--) {
      stepOneRight(dirR, SPEED);
    }
  }
}

void stepOne(int dirL, int dirR, float speed) {
  delayMicroseconds(DELAY); 
  stepper1.move(dirR*1);
  stepper1.setSpeed(speed);
  stepper1.runSpeedToPosition();
  stepper2.move(dirL*-1);
  stepper2.setSpeed(speed);
  stepper2.runSpeedToPosition();
}

void stepOneLeft(int dirL, float speed) {
  delayMicroseconds(DELAY); 
  stepper2.move(dirL*-1);
  stepper2.setSpeed(speed);
  stepper2.runSpeedToPosition();
}

void stepOneRight(int dirR, float speed) {
  delayMicroseconds(DELAY); 
  stepper1.move(dirR);
  stepper1.setSpeed(speed);
  stepper1.runSpeedToPosition();
}
