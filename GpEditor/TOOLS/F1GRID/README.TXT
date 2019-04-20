F1Grid V1.2 31/Jan/95 - Modify F1GP grid positions.
Copyright (c) T.Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.

F1Grid V1.1 allows you to setup the entire grid for a race in the Microprose
F1 Grand Prix (World Circuit) simulator (US & European V1.05). F1Grid allows
you to edit the car positions for a saved qualifying session.

Installation
------------
Copy the F1GRID.EXE file to your GPSAVES directory (normally C:\GPRIX\GPSAVES).

Usage
-----
     f1grid [-h?] [-u] [Saved Game] [Text file]
       -?/-h            This usage message.
       -u               Update saved game from text file.
       [Saved Game]     Specify another name for F1GRID.SAV.
       [Text File]      Specify another name for F1GRID.TXT.

Modifying The Grid
------------------
Startup F1GP up with the desired drivers names and car colours and go into
qualifying at the desired track. At the start of the qualifying session (you
need to ensure a computer car crosses the start/finish line on a timed lap
when you reload the modified save file) press <Esc> and save the qualifying
session to a file (F1GRID.SAV).

Now quit F1GP. Change directory to your GPSAVES directory. You now need to
extract the list of driver names and car numbers from the save file:

    F1GRID DEMO.SAV DEMO.TXT

(you can type just 'F1GRID' on its own and it will use the default names).

Now using any ASCII editor (e.g., Windows NOTEPAD) edit the newly created
.TXT file and change the first number to indicate the drivers new starting
position. If you wish you can cut and paste whole lines and then renumber
them sequentially. F1Grid just reads the grid position and car number (it
ignores the driver name), it doesn't matter what order the grid positions are
specified in.

F1Grid will tell you if you have made a mistake and specified the same grid
position twice. Note that if you leave a space on the grid all the cars will
be shuffled up to fill the gap.

Having edited the *.TXT file you need to update the saved game file with the
new grid positions using the '-u' option:

    F1GRID -u DEMO.SAV DEMO.TXT

(you can type just 'F1GRID -u' on its own and it will use the default names).

Now reload F1GP. Use the games "Load Game" option, you will return to the
qualifying session with the game in pause mode. When you unpause the game the
qualifying session will have 10mins remaining, press accelerated time and
when the next computer car crosses the start/finish line at the end of a
timed lap you will see the grid positions change on your TV monitor. F1Grid
sets the 1st placed car time to 0:00:001, 2nd to 0:00:002, 3rd to 0:00:003
etc. Note that you *must* let a CC cross the s/f line to get the sorting to
happen (so saving the qualifying right at the start will ensure that a CC
will cross the s/f line in the remaining 10min of the qualifying session).

Now go and race with your preset grid!

How F1Grid Works
----------------
F1Grid reads your modified text file which specifies what the grid positions
should be, it then sets each cars time starting from 1st on the grid to be
1/1000s, 2/1000s, 3/1000s etc. F1Grid also sets the current qualifying
session time to 0 so that the qualifying session terminates immediately.
The game just thinks that the cars have qualified with the superfast times
and sorts the grid order appropriately.

Further Info
------------
Please send any suggestions, comments, or bug reports (include the version 
of F1Grid you are using and the version of the game EXE) to the author at:

CompuServe:    100331,2330
Internet:      100331.2330@compuserve.com

This program may be freely copied and distributed, as long as it is not
modified in any way, no fee is charged and this file is always included.
This program is provided "as is" with no warranties expressed or implied.
The author will not be held responsible for any losses incurred, either
directly or indirectly by the use of this program.

