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

void setup(void)
{
  // Initailize serial port
  Serial.begin(115200);

  // Initialize steppers and servo.
  stepper1.setMaxSpeed(SPEED);
  stepper2.setMaxSpeed(SPEED);
  myservo.attach(4);

  // PUT COMMANDS HERE
//  calibrateLevel();  // Upon first use, calibrate your bot.
//  calibrateRightTurns();
//  calibrateRightTurns();
  calibrateCircles();  // Upon first use, calibrate your bot.
//  drawShapes();
//  stars();
  // RETURN PEN TO UP POSITION
  penUp();
}

void loop(void)
{
}

const int PEN_DOWN = 30;
const int PEN_UP = 60;
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
  penUp();
  int steps = millimetersToSteps(distance);
  step(steps, steps);
}

/*
  Draw an arc with radius in millimeters and angle in desgres
*/
void arc(float radius, float angle)
{
  penDown();
  float diameterRatio = 2 * radius / CIRCLE_DIAM;
  float angleRatio = angle / 360;
  float leftSteps = (1800 + 1800 * diameterRatio);
  float rightSteps = (leftSteps - 3600);
  leftSteps *= angleRatio;
  rightSteps *= angleRatio;
  step(leftSteps, rightSteps);
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
penUp();
step(250, -250);
penUp();
step(135, 135);
penUp();
step(-250, 250);
penDown();
step(200, 200);
penUp();
step(-700, 700);
penDown();
step(200, 200);
penUp();
step(400, -400);
penDown();
step(200, 200);
penUp();
step(-700, 700);
penDown();
step(200, 200);
penUp();
step(400, -400);
penDown();
step(200, 200);
penUp();
step(-700, 700);
penDown();
step(200, 200);
penUp();
step(400, -400);
penDown();
step(200, 200);
penUp();
step(-700, 700);
penDown();
step(200, 200);
penUp();
step(400, -400);
penDown();
step(200, 200);
penUp();
step(-700, 700);
penDown();
step(200, 200);
penUp();
step(400, -400);
penDown();
step(200, 200);
penUp();
step(-700, 700);
penDown();
step(200, 200);
penUp();
step(400, -400);
penUp();
step(-650, 650);
penUp();
step(135, 135);
penUp();
step(650, -650);
penUp();
step(250, -250);
penUp();
step(101, 101);
penUp();
step(-250, 250);
penDown();
step(150, 150);
penUp();
step(-700, 700);
penDown();
step(150, 150);
penUp();
step(400, -400);
penDown();
step(150, 150);
penUp();
step(-700, 700);
penDown();
step(150, 150);
penUp();
step(400, -400);
penDown();
step(150, 150);
penUp();
step(-700, 700);
penDown();
step(150, 150);
penUp();
step(400, -400);
penDown();
step(150, 150);
penUp();
step(-700, 700);
penDown();
step(150, 150);
penUp();
step(400, -400);
penDown();
step(150, 150);
penUp();
step(-700, 700);
penDown();
step(150, 150);
penUp();
step(400, -400);
penDown();
step(150, 150);
penUp();
step(-700, 700);
penDown();
step(150, 150);
penUp();
step(400, -400);
penUp();
step(-650, 650);
penUp();
step(101, 101);
penUp();
step(650, -650);
penUp();
step(250, -250);
penUp();
step(67, 67);
penUp();
step(-250, 250);
penDown();
step(100, 100);
penUp();
step(-700, 700);
penDown();
step(100, 100);
penUp();
step(400, -400);
penDown();
step(100, 100);
penUp();
step(-700, 700);
penDown();
step(100, 100);
penUp();
step(400, -400);
penDown();
step(100, 100);
penUp();
step(-700, 700);
penDown();
step(100, 100);
penUp();
step(400, -400);
penDown();
step(100, 100);
penUp();
step(-700, 700);
penDown();
step(100, 100);
penUp();
step(400, -400);
penDown();
step(100, 100);
penUp();
step(-700, 700);
penDown();
step(100, 100);
penUp();
step(400, -400);
penDown();
step(100, 100);
penUp();
step(-700, 700);
penDown();
step(100, 100);
penUp();
step(400, -400);
penUp();
step(-650, 650);
penUp();
step(67, 67);
penUp();
step(650, -650);
penUp();
step(250, -250);
penUp();
step(33, 33);
penUp();
step(-250, 250);
penDown();
step(50, 50);
penUp();
step(-700, 700);
penDown();
step(50, 50);
penUp();
step(400, -400);
penDown();
step(50, 50);
penUp();
step(-700, 700);
penDown();
step(50, 50);
penUp();
step(400, -400);
penDown();
step(50, 50);
penUp();
step(-700, 700);
penDown();
step(50, 50);
penUp();
step(400, -400);
penDown();
step(50, 50);
penUp();
step(-700, 700);
penDown();
step(50, 50);
penUp();
step(400, -400);
penDown();
step(50, 50);
penUp();
step(-700, 700);
penDown();
step(50, 50);
penUp();
step(400, -400);
penDown();
step(50, 50);
penUp();
step(-700, 700);
penDown();
step(50, 50);
penUp();
step(400, -400);
penUp();
step(-650, 650);
penUp();
step(33, 33);
penUp();
step(650, -650);
}

void step(long lsteps, long rsteps)
{
  Serial.print("lsteps="); Serial.println(lsteps);
  Serial.print("rsteps="); Serial.println(rsteps);
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
