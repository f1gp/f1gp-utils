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
** V4.0 13-May-2019 Made the yellow grip bar fill up twice as slow, because
**                  on some circuits the maximum was beyond the bar's range.
**                  Use the game's tyre grip table as a base for the grip bar.
**                  Made grip bar work also in wet conditions.
** V4.1 26-Jun-2019 Added -e option to start with (manual) logging enabled.
*/

#define VERSION          "V4.1 26-Jun-2019"

