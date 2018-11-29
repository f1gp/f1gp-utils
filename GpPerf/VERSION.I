/*
** Version (change VERSION_MAJOR if log entries are incompatible).
**
** V1.0 12/11/94    First bash.
** V1.1 02/Jan/95   Added '-d' option.
**                  Fixed bug where <F7> & <F8> when car is on the jacks
**                  didn't update the LED due to some machines keyboard
**                  interrupt BIOS handling switching stacks.
**                  No automatic logging disabling when in a race.
** V2.0 11-Jun-95   Added logging of steering, acceleration, braking and
**                  car's X/Y position.
** V3.0 15-Oct-95   Fixed timing variation bug in game.
**                  In linked play mode only the local car is now logged.
**                  Fixed error messages which display 'ccpit'.
**                  Italian version now supported.
**                  Specified computer car can now be logged.
**                  Display of tyre grip added.
**                  Updated SHOWPERF for display of steering, accel & braking.
** V3.1 25-Oct-95   Corrected missing readme text file info and Usage message.
**
** V3.2 18-Nov-18   Made the yellow grip bar fill up twice as slow.
*/

#define VERSION          "V3.2 28-Nov-2018"

