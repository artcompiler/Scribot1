Starting | Scribot1
===================

## Calibrating

Before you can draw accurately with Scribot1 you need to calibrate the bot. This is done by executing the 'calibrate()' function and adjusting the values for DEFAULT_CALIBRATION, LF_CALIBRATION, RF_CALIBRATION, LB_CALIBRATION and RB_CALIBRATION.

Begin by adjusting DEFAULT_CALIBRATION until the bot draws circles where the drawing error is long in at least one direction and short in at least one other. Then iterate while adjusting the four specific calibration factors.

It should be possible to draw circles where the error is less than 1mm in all four directions.

## Drawing

The Scribot1 code implements two drawing interfaces. The lowest level interface (Level 0) reflects the primitive operations of the machine, in particular moving the pen up and down and moving the wheels forward and backward. A slightly higher level interface (Level 1) expresses commands for drawing basic shapes.

These commands (functions or procedures) can be called from the ```setup()``` function of the main program file.

### Level 0 Drawing Commands

The most primitive interface to Scribot consists of three commands:

* ```penUp()```
* ```penDown()```
* ```step(leftCount, rightCount)```

#### penUp()

Moves the pen to the up position to avoid contact with the drawing surface.

#### penDown()

Move the pen to the down position to make contact with the drawing surface.

### step(leftCount, rightCount)

Move the left and right wheels by the specified step counts. The bot is calibrated
so that a complete rotation of the bot occurs with every 3600 steps. For example,
```step(3600, 0)``` will cause the bot to pivot 360 degrees on its right wheel, and ```step(1800, -1800)```
will cause the bot to rotate 360 degrees around its pen.

### Level 1 Drawing Commands

* ```line(length)```
* ```arc(radius, angle)```
* ```circle(radius)```
* ```square(length)```
* ```triangle(length)```
* ```turn(angle)```
* ```move(distance)```

### line(distance)

Draw a line the specified length as millimeters. The pen is automatically moved to the down position to ensure marking of the drawing surface.

### arc(radius, angle)

Draw an arc with the specified radius as millimeters and angle as degrees. The pen is automatically moved to the down position to ensure marking of the drawing surface.

### circle(radius)

Draw a circle with the specified radius as millimeters. The pen is automatically moved to the down position to ensure marking of the drawing surface.

### square(length)

Draw a square with the specified side length as millimeters. The pen is automatically moved to the down position to ensure marking of the drawing surface.

### triangle(length)

Draw an equilateral triangle with the specified side length as millimeters. The pen is automatically moved to the down position to ensure marking of the drawing surface.

### turn(angle)

Turn the bot the specified angle as degrees. The pen is automatically moved to the up position to avoid marking the drawing surface.

### move(distance)

Move the bot the specified distance in millimeters. The pen is automatically moved to the up position to avoid marking the drawing surface.
