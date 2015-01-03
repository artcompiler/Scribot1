/* -*- Mode: c++; c-indent-level: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vim: set shiftwidth=2 tabstop=2 autoindent cindent expandtab: */
/*
  Before you can draw accurately with Scribot1 you need to calibrate the bot.
  This is done by executing the 'calibrate()' function and adjusting the values
  for DEFAULT_CALIBRATION, LF_CALIBRATION, RF_CALIBRATION, LB_CALIBRATION
  and RB_CALIBRATION.

  Begin by adjusting DEFAULT_CALIBRATION until the bot draws circles where the
  drawing error is long in at least one direction and short in at least one
  other. Then interate while adjusting the four specific calibration factors.

  It should be possible to draw circles where the error is less than 1mm in all
  four directions.
*/

// BEGIN STEPPER DECLS
#include <AccelStepper.h>

// Define motor pins.
#define motorPin1  A0     // Blue   - 28BYJ48 pin 1
#define motorPin2  A1     // Pink   - 28BYJ48 pin 2
#define motorPin3  A2     // Yellow - 28BYJ48 pin 3
#define motorPin4  A3     // Orange - 28BYJ48 pin 4

#define motorPin5  6      // Blue   - 28BYJ48 pin 1
#define motorPin6  7      // Pink   - 28BYJ48 pin 2
#define motorPin7  8      // Yellow - 28BYJ48 pin 3
#define motorPin8  9      // Orange - 28BYJ48 pin 4

#define BACKWARD 1
#define FORWARD -1
#define SPEED 1000
#define DELAY 1500        // Minimum delay to allow the motors to complete one step

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

// CALIBRATE THESE VALUES FOR YOUR BOT
const float DEFAULT_CALIBRATION = 3.63;
const float LF_CALIBRATION = DEFAULT_CALIBRATION + 0.01;
const float RF_CALIBRATION = DEFAULT_CALIBRATION + 0.01;
const float LB_CALIBRATION = DEFAULT_CALIBRATION + 0.02;
const float RB_CALIBRATION = DEFAULT_CALIBRATION - 0.01;
const float CIRCLE_DIAM = 131.0;   // Diameter of calibration circles in millimeters
const float CIRCLE_CIRCUM = CIRCLE_DIAM * PI;
const float CIRCLE_STEPS = 3600 * DEFAULT_CALIBRATION;
const float STEP_LENGTH = 2 * CIRCLE_CIRCUM / CIRCLE_STEPS; // 2x because wheel travels twice as far

void setup(void)
{
  Serial.begin(115200);
  // Initialize steppers and servo.
  stepper1.setMaxSpeed(SPEED);
  stepper2.setMaxSpeed(SPEED);
  myservo.attach(A4);
  penUp();  // Pen up to avoid unintended marks
  // PUT COMMANDS HERE
  //  calibrate();
  //  square(100);
  // triangle(100);
  arc(10, 3600);
}

void loop(void)
{
}

float millimetersToSteps(float distance)
{
  float calibration = distance > 0
    ? (LF_CALIBRATION + RF_CALIBRATION) / 2
    : (LB_CALIBRATION + RB_CALIBRATION) / 2;  // Get average of left and right
  return distance / STEP_LENGTH / calibration;
}

void calibrate()
{
  penDown();
  step(3600, 0);  // Left Forward (LF)
  penUp();
  step(-450, 450);
  step(100, 100);
  step(-450, 450);
  penDown();
  step(0, 3600);  // Right Forward (RF)
  penUp();
  step(450, -450);
  step(100, 100);
  step(450, -450);
  penDown();
  step(-3600, 0);  // Left Backward (LB)
  penUp();
  step(-450, 450);
  step(100, 100);
  step(-450, 450);
  penDown();
  step(0, -3600);  // Right Backward (RB)
  penUp();
}

void line(float distance)
{
  int steps = millimetersToSteps(distance);
  penDown();
  step(steps, steps);
  penUp();
}

void move(float distance)
{
  int steps = millimetersToSteps(distance);
  penUp();
  step(steps, steps);
}

void arc(int radius, int angle)
{
  // FIXME this function is not done yet.
  int leftSteps, rightSteps;
  int ratio = 2 * radius / CIRCLE_DIAM;
  leftSteps = 1800 + CIRCLE_STEPS * ratio;
  rightSteps = -3600 + leftSteps;
  penDown();
  step(leftSteps, rightSteps);
  penUp();
}  

void circle(int radius)
{
  penDown();
  step(-1000, 2600);
  penUp();
}  

void square(int size)
{
  line(size);
  step(450, -450);
  line(size);
  step(450, -450);
  line(size);
  step(450, -450);
  line(size);
  step(450, -450);
}  

void triangle(int size)
{
  line(size);
  step(600, -600);
  line(size);
  step(600, -600);
  line(size);
  step(600, -600);
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
  int dirL = (lsteps > 0) ? FORWARD : BACKWARD;
  int dirR = (rsteps > 0) ? FORWARD : BACKWARD;
  lsteps = abs(lsteps) * (dirL == FORWARD ? LF_CALIBRATION : LB_CALIBRATION);
  rsteps = abs(rsteps) * (dirR == FORWARD ? RF_CALIBRATION : RB_CALIBRATION);
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
