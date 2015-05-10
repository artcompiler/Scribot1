/* -*- Mode: c++; c-indent-level: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vim: set shiftwidth=2 tabstop=2 autoindent cindent expandtab: */
/*
  Copyright 2015 Art Compiler LLC
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

/*
  Before you can draw accurately with Scribot you need to calibrate the bot.
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
#define motorPin1  5     // Blue   - 28BYJ48 pin 1
#define motorPin2  6     // Pink   - 28BYJ48 pin 2
#define motorPin3  7     // Yellow - 28BYJ48 pin 3
#define motorPin4  8     // Orange - 28BYJ48 pin 4

#define motorPin5  9     // Blue   - 28BYJ48 pin 1
#define motorPin6  10    // Pink   - 28BYJ48 pin 2
#define motorPin7  11    // Yellow - 28BYJ48 pin 3
#define motorPin8  12    // Orange - 28BYJ48 pin 4

#define servoPin     A6      // Servo
#define leftLinePin  A1
#define rightLinePin A2
#define trigPin      2
#define echoPin      3

#define BACKWARD 1
#define FORWARD -1
#define SPEED 1000
#define DELAY 1500        // Minimum delay to allow the motors to complete one step

// Construct stepper objects.
#include "Arduino.h"
long microsecondsToInches(long microseconds);
long microsecondsToCentimeters(long microseconds);
void setup(void);
void follow(void);
void turnLeft(int a);
void turnRight(int a);
int distance(void);
void loop(void);
void penUp();
void penDown();
void calibrateLevel();
void calibrateRightTurns();
void calibrateLeftTurns();
void calibrateCircles();
void drawShapes();
float millimetersToSteps(float distance);
void line(float distance);
void move(float distance);
void arc(float radius, float angle);
void turn(float angle);
void circle(float radius);
void square(int size);
void triangle(int size);
void stars();
void step(long lsteps, long rsteps);
void stepOne(int dirL, int dirR, float speed);
void stepOneRight(int dir, float speed);
void stepOneLeft(int dir, float speed);
#line 59
AccelStepper stepper1(AccelStepper::FULL4WIRE, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper2(AccelStepper::FULL4WIRE, motorPin5, motorPin7, motorPin6, motorPin8);
// END STEPPER DECLS

// BEGIN SERVO DECLS
#include <Servo.h> 

// Construct servo object.
Servo myservo;
// END SERVO DECLS

// CALIBRATE THESE VALUES FOR YOUR BOT
// -- After calibrating the lines, the total calibration needs to stay the same
//    for each combintation of DEFAULT+LF+RB and DEFAULT+RF+LB
const float DEFAULT_CALIBRATION = 3.660;
const float LF_CALIBRATION = DEFAULT_CALIBRATION - 0.015;
const float RF_CALIBRATION = DEFAULT_CALIBRATION - 0.005;
const float LB_CALIBRATION = DEFAULT_CALIBRATION - 0.015;
const float RB_CALIBRATION = DEFAULT_CALIBRATION - 0.005;
const float CIRCLE_DIAM = 128.0;   // Diameter of calibration circles in millimeters
const float CIRCLE_CIRCUM = CIRCLE_DIAM * PI;
const float CIRCLE_STEPS = 3600 * DEFAULT_CALIBRATION;
const float STEP_LENGTH = 2 * CIRCLE_CIRCUM / CIRCLE_STEPS; // 2x because wheel travels twice as far

long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

void setup(void)
{
  // Initailize serial port
  Serial.begin(115200);

  // Initialize steppers and servo.
  stepper1.setMaxSpeed(SPEED);
  stepper2.setMaxSpeed(SPEED);
  myservo.attach(servoPin);

  // PUT COMMANDS HERE
  //  calibrateLevel();  // Upon first use, calibrate your bot.
  //  calibrateRightTurns();
  //  calibrateLeftTurns();
  //  calibrateCircles();  // Upon first use, calibrate your bot.
  //  drawShapes();
  //  stars();
  // RETURN PEN TO UP POSITION
  penUp();
}

const int THREASHOLD = 100;
const int NONE = 0;
const int LEFT = 1;
const int RIGHT = 2;
const int BOTH = 3;
int state = NONE, event;
int angle = 0;
int d;
void follow(void)
{
//  int d1 = distance();
//  int d2 = distance();
//  int dist;
//  // Throw away the lower, which might be noise.
//  if (d1 > d2) {
//    dist = d1;
//  } else {
//    dist = d2;
//  }
//  if (dist < 10) {
//    // Deflect in the direction already turning.
//    if (angle < 0) {
//      turn(-10);
//    } else {
//      turn(10);
//    }
//  }
  int leftSignal = analogRead(leftLinePin);
  int rightSignal = analogRead(rightLinePin);
  bool left = leftSignal > THREASHOLD;
  bool right = rightSignal > THREASHOLD;
  if (left && right) {
    event = BOTH;
  } else if (left) {
    event = LEFT;
  } else if (right) {
    event = RIGHT;
  } else {
    event = NONE;
  }
  switch (state) {
  case NONE:
    // Look for a line.
    switch (event) {
    case LEFT:
      // Joining left or right edge. Turn left.
      state = LEFT;
      angle = -1;
      break;
    case RIGHT:
      // Joining left or right edge. Turn right.
      state = RIGHT;
      angle = 1;
      break;
    case BOTH:
      // Joining perpendicular. Veer left.
      state = BOTH;
      angle = -1;
      break;
    case NONE:
      // Status quo.
      state = NONE;
      break;
    }
    break;
  case BOTH:
    // Look for a line.
    switch (event) {
    case LEFT:
      // Leaving right edge. Turn left.
      state = LEFT;
      angle = -1;
      break;
    case RIGHT:
      // Leaving left edge. Turn right.
      state = RIGHT;
      angle = 1;
      break;
    case BOTH:
      // No change. Keep doing what you are doing.
      state = BOTH;
//      angle = 0;
      break;
    case NONE:
      // Leaving perpendicular. Keep doing what you are doing.
      state = NONE;
      break;
    }
    break;
  case LEFT:
    switch (event) {
    case BOTH:
      // Right joining right edge. Veer right.
      state = BOTH;
      angle = 1;
      break;
    case LEFT:
      // Status quo.
      state = LEFT;
      angle = -1;
      break;
    case RIGHT:
      // Right joining right edge. Veer right.
      state = RIGHT;
      angle = 1;
      break;
    case NONE:
      // Left leaving right or left edge.
      state = NONE;
      break;
    }
    break;
  case RIGHT:
    switch (event) {
    case BOTH:
      // Left joining left edge. Go straight.
      state = BOTH;
      angle = -1;
      break;
    case LEFT:
      // Left joining left edge. Go straight.
      state = LEFT;
      angle = -1;
      break;
    case RIGHT:
      // Status quo.
      state = RIGHT;
      angle = 1;
      break;
    case NONE:
      // Right leaving right or left edge. Go straight.
      state = NONE;
      //angle = 0;
      break;
    }
    break;
  default:
    Serial.print("ERROR: invalid state "); Serial.println(state);
    break;
  }
  moveArc(angle);
}

void turnRight(int a) {
  arc(50, a);
}

void moveArc(int a) {
  if (a == 0) {
    move(1);
  } else {
    arc(100, a);
  }
}

void turnLeft(int a) {
  arc(50, -a);
}

int distance(void) {
  // establish variables for duration of the ping, 
  // and the distance result in inches and centimeters:
  long duration, inches, cm;

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);  
  duration = pulseIn(echoPin, HIGH);
  
  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);  
  return cm;
}

void loop(void)
{
  follow();
}

const int PEN_DOWN = 0;
const int PEN_UP = 180;
int pos = PEN_UP;
void penUp()
{
  for(; pos < PEN_UP; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                 // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  myservo.write(pos);              // tell servo to go to position in variable 'pos' 
}

void penDown()
{
  for(; pos > PEN_DOWN; pos -= 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }
  myservo.write(pos);              // tell servo to go to position in variable 'pos' 
}

void calibrateLevel()
{
  penDown();
  step(1800, -1800);
  step(30, 30);
  step(-1800, 1800);
  penUp();
}

void calibrateRightTurns()
{
  penDown();
  step(500, 500);
  step(900, -900);
  step(500, 500);
  step(900, -900);
  step(500, 500);
  step(900, -900);
  step(500, 500);
  step(900, -900);
  penUp();
}

void calibrateLeftTurns()
{
  penDown();
  step(500, 500);
  step(-900, 900);
  step(500, 500);
  step(-900, 900);
  step(500, 500);
  step(-900, 900);
  step(500, 500);
  step(-900, 900);
  penUp();
}

void calibrateCircles()
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
  step(450, -450);
  step(100, 100);
  step(450, -450);
}

void drawShapes() 
{
  penDown();
  square(80);
  turn(180);
  move(10);
  turn(90);
  move(10);
  triangle(85);
  move(50);
  turn(90);
  circle(50);
  penUp();
}

float millimetersToSteps(float distance)
{
  float calibration = distance > 0
    ? (LF_CALIBRATION + RF_CALIBRATION) / 2
    : (LB_CALIBRATION + RB_CALIBRATION) / 2;  // Get average of left and right
  return distance / STEP_LENGTH / calibration;
}

void line(float distance)
{
  penDown();
  int steps = millimetersToSteps(distance);
  step(steps, steps);
}

void move(float distance)
{
//  penUp();
  int steps = millimetersToSteps(distance);
//  Serial.print("move() steps="); Serial.println(steps);
  step(steps, steps);
}

/*
  Draw an arc with radius in millimeters and angle in degrees
 */
void arc(float radius, float angle)
{
//  penDown();
  bool right = angle > 0;
  angle = abs(angle);
  float diameterRatio = 2 * radius / CIRCLE_DIAM;
  float angleRatio = angle / 360;
  float leftSteps = (1800 + 1800 * diameterRatio);
  float rightSteps = (leftSteps - 3600);
  leftSteps *= angleRatio;
  rightSteps *= angleRatio;
  if (right) {
    step(rightSteps, leftSteps);
  } else {
    step(leftSteps, rightSteps);
  }
}  

void turn(float angle)
{
  penUp();
  const int fullSteps = 1800;
  float angleRatio = angle / 360;
  int leftSteps = fullSteps * angleRatio;
  int rightSteps = -fullSteps * angleRatio;
  step(leftSteps, rightSteps);
}

void circle(float radius)
{
  arc(radius, 360);
}  

void square(int size)
{
  line(size);
  turn(90);
  line(size);
  turn(90);
  line(size);
  turn(90);
  line(size);
  turn(90);
}

void triangle(int size)
{
  line(size);
  turn(120);
  line(size);
  turn(120);
  line(size);
  turn(120);
}

void stars()
{
}

void step(long lsteps, long rsteps)
{
//  Serial.print("LS="); 
//  Serial.print(lsteps);
//  Serial.print(" RS="); 
//  Serial.println(rsteps);
  int dirL = (lsteps > 0) ? FORWARD : BACKWARD;
  int dirR = (rsteps > 0) ? BACKWARD : FORWARD;
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
  stepper1.move(dirL);
  stepper1.setSpeed(speed);
  stepper1.runSpeedToPosition();
  stepper2.move(dirR*-1);
  stepper2.setSpeed(speed);
  stepper2.runSpeedToPosition();
}

void stepOneRight(int dir, float speed) {
  delayMicroseconds(DELAY); 
  stepper2.move(dir*-1);
  stepper2.setSpeed(speed);
  stepper2.runSpeedToPosition();
}

void stepOneLeft(int dir, float speed) {
  delayMicroseconds(DELAY); 
  stepper1.move(dir);
  stepper1.setSpeed(speed);
  stepper1.runSpeedToPosition();
}




