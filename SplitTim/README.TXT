SplitTim V2.0 30th Sep 1994                 Trevor Kellaway (CIS:100331,2330)

             Grand Prix/World Circuit Split Section Timer
             ============================================

SPLITTIM is a small TSR which modifies the behaviour of GP.EXE. It adds split
section times to all GP circuits. Each circuit is divided into four sections
(by default at 25%,50% & 75%), as you pass each section the display on your
car's dash will show you your split (intermediate) time. The split time is
held on the display (by default for 10 seconds) then the timer will return to
its normal mode of counting.

SPLITTIM is very useful when you are trying to set the car up, as it enables
you to determine where you are losing/gaining time with different setups.

SPLITTIM works in all types of game session except for a race.

Installation
------------
Copy SPLITTIM.EXE to any directory on your hard disk.

Running
-------
Run SPLITTIM once to install it before you start F1GP (or GP.EXE).
SPLITTIM only takes up about 1Kb of memory. After you have finished using
it you can unload it with "SPLITTIM -u". If you have problems with GP
reporting that there isn't enough memory to load GP.EXE then try loading
SPLITTIM high (see the LOADHIGH or LH commands in your DOS or memory manager
manual).

Command Line Parameters
-----------------------
Usage: SplitTim [-fN -aN -bN -cN | -u]

       -h,-?     This help message.

       -aN       1st split time percentage (default 25%).
       -bN       2nd split time percentage (default 50%).
       -cN       3rd split time percentage (default 75%).

       -fN       Freeze delay in seconds (default 10).

       -u        Unload TSR.

By default just typing "SplitTim" on its own is equivalent to:

     SplitTim -f10 -a25 -b50 -c75

To change the freeze time to 15s and the split points to 20%, 45% and 65%:

     SplitTim -f15 -a20 -b45 -c65

Note that SplitTim will still work OK if you make the split points too near
and use a long freeze time, i.e., it doesn't matter if the display is held
from a previous split point when you reach the next split point; the display
will just be updated with the new spilt point time.

Change History
--------------
V1.0 04/09/94    First release.
V2.0 30/09/94    Command line hold delay and split points.

Further Info
------------
Please send any suggestions, comments, or bug reports (include the version 
of SPLITTIM you are using and the version of the game EXE) to the author
Trevor Kellaway at:

CompuServe:    100331,2330
Internet:      100331.2330@compuserve.com

This program may be freely copied and distributed, as long as it is not
modified in any way, no fee is charged and this file is always included.
This program is provided "as is" with no warranties expressed or implied.
The author will not be held responsible for any losses incurred, either
directly or indirectly by the use of this program.

This is *not* an MPS product. If you have problems with the game then try 
it without this program loaded.

