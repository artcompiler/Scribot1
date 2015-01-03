Starting | Scribot1
===================

Before you can draw accurately with Scribot1 you need to calibrate the bot.
This is done by executing the 'calibrate()' function and adjusting the values
for DEFAULT_CALIBRATION, LF_CALIBRATION, RF_CALIBRATION, LB_CALIBRATION
and RB_CALIBRATION.

Begin by adjusting DEFAULT_CALIBRATION until the bot draws circles where the
drawing error is long in at least one direction and short in at least one
other. Then iterate while adjusting the four specific calibration factors.

It should be possible to draw circles where the error is less than 1mm in all
four directions.

