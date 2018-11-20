GpLapTim
========
This new version of GpLapTim was created from a disassembled GPLAPTIM.COM.
All old features are still there, but some have been adapted and new ones
have been added.


Change history
--------------
Version 7.0 adds some changes for hotseat mode:

1. Set player grip to 1 only if it is the active Player (not CC controlled).
CCPlayer will use the regular AIG value instead.

2. When car control goes automatic (CCPlayer) or to control (Player),
the grip will be set accordingly.

3. The CCPlayer BHP can be set from the commandline using option -w, e.g.:

    gplaptim -w800

    Default value is 716.
    Randomization of this value will be disabled, just like for Player BHP.

4. The BHP logged in the authentication record and shown by GPLap will show
the either the value for CCPLayer or for Player, whoever is in control at
the time the car passes the s/f line.


Contact
-------
Rene Smit <gpxpatch@xs4all.nl>

http://rsdi.home.xs4all.nl/


Thanks to
---------
- Trevor Kellaway for creating the original version.
- Barrie Millar for asking Trevor to open source his F1GP utilities.
- Hrvoje Stimac for suggestions and testing.
