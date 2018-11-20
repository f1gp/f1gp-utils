GpTimLog V6.1 4th Apr 1996                  Trevor Kellaway (CIS:100331,2330)

Grand Prix/World Circuit Lap Time Logger.

GPLAPTIM logs the times for *all* the cars to disk as they cross the start/
finish line (in the MicroProse Grand Prix/World Circuit simulation game). It
also adds a split section timer giving you three intermediate times for laps.

GPLAPTIM has been tested with V1.05 of the European, US, Italian and Spanish 
versions of GP.EXE. This version of GPLAPTIM now includes the functionality
of the SPLITTIM utility so you *shouldn't* load SPLITTIM anymore otherwise
the logging of your split points to the .LOG may not happen.

*** PLEASE READ THE REST OF THIS FILE, IT MAY ANSWER YOUR QUESTIONS! <g> ***

FILE LIST
---------
GPLAPTIM.COM   TSR which writes all lap times to disk file GPLAPTIM.LOG.
GPLAP.EXE      DOS command line program which views file GPLAPTIM.LOG.
GPLAPW.EXE     Windows program which views file GPLAPTIM.LOG.
GPLAPW.HLP     Windows help file for GPLAPW.
README.TXT     This file!
CTL3D.DLL      Windows 3-D control library (optional).

Installation
------------
Copy the files to any directory on your hard disk.

How To Log Laps
---------------
Run the GPLAPTIM.COM TSR. It will install itself, you only need to load the
TSR once after having booted DOS. (You can unload the TSR with the -u option).

Startup GP.EXE, "GpLapTim activated OK" should be displayed for about 1/2
second on the screen. If you run F1GP.BAT it will be displayed at the top
of the intro screen, if you run GP.EXE directly it will be displayed at
the bottom of the text mode screen.

Drive your practice/race, GPLAPTIM.LOG will be automatically created and
entries appended to it every time a car crosses the start/finished line.

Quit GP.EXE, run GPLAP (or GPLAPW) to view the packed GPLAPTIM.LOG file.

Usage: GPLAPTIM [-h?]  [-aN -bN -cN -s] [-p] [-u] [-f(filename)]
       -f(name)  Specify log filename's location.
       -p        Log player's times only.
       -aN       1st split time percentage (default 25%).
       -bN       2nd split time percentage (default 50%).
       -cN       3rd split time percentage (default 75%).
       -sN       Stop (freeze) delay in seconds (default 10s).
       -h,-?     This help message.
       -u        Unload TSR.

Usage: GPLAP [-tqrcplfbasmkeginxw] [log filename]
       -?/-h  This usage message.
       -t     Show Practice (Training).
       -q     Show Qualifying.
       -r     Show Race.
       -c     Show Computer.
       -p     Show Player.
       -l     Show Leader.
       -f     Show Fastest Laps.
       -b     Show Best Laps.
       -a     Show Authentication.
       -s     Show Average Speed.
       -m     Average Speed in MPH.
       -k     Average Speed in KPH.
       -e     Elapsed Race Time.
       -g     Show Car Setup (Garage).
       -i     Show Pit Stops (In/out).
       -n     Number of laps.
       -x     Write authentication file (AUTHLAP.TXT).
       -w     Watch log file grow (<Esc> to quit).
       [...]  Specify another name for GPLAPTIM.LOG.

For GPLAPW see the Windows help file included.

Authentication of Hot Laps
--------------------------
GPLAPTIM will write authentication records for your best laps to the log
file. These may be submitted to F1SA in the Compuserve SPRTSIMS forum (GO
SPRTSIMS) section #14 (Racing Circuits), see GPLAP and GPLAPW. To maintain a
"level playing field" GPLAPTIM ensures that the player's AIG is always set to
0 and that the player's BHP is always as specified (by default the game will
randomly degrade the engine performance). GPLAPTIM now also detects if you
touch the grass on a qualifying lap. When you touch the grass the red X in
your pit indicator will come on (it still means pit occupied in a race).
GPLAPTIM still writes your authenticated time but it will mark it as having
used the grass (currently F1SA doesn't accept laps which have used the
grass). If you need to see the state of the yellow pit call light when the
red X has appeared then just toggle the pit call-in twice and the indicator
will return to its normal state. Note that when you cross the start/finish
line for the start of the next lap the red X will automatically be cancelled.

Information Logged
------------------

Qualifying example:

Pos Car Lap     Driver               Lap Time  Best Lap   Race Time
-----------------------------------------------------------------------------
 -  #24   2 Michele Alboreto         -:--.--- -:--.---   
 -  #34   2 Bertrand Gachot          -:--.--- -:--.---   
 -  #19   2 Erik Comas               -:--.--- -:--.---   
 -  #4    2 Mark Blundell            -:--.--- -:--.---   
 -  #6    2 Jos Verstappen           -:--.--- -:--.---   
 -  #26   2 Olivier Panis            -:--.--- -:--.---   
 -  #23   2 Pierluigi Martini        -:--.--- -:--.---   
Q 1 #24   3 Michele Alboreto         1:23.966 1:23.966!FQ
Q 1 #19   3 Erik Comas               1:21.995 1:21.995!FQ
Q 2 #4    3 Mark Blundell            1:22.999 1:22.999!  
Q 2 #6    3 Jos Verstappen           1:22.949 1:22.949!  
Q 6 #34   3 Bertrand Gachot          1:25.349 1:25.349!  

For qualifying a 'Q' is shown in the position field along with the current
grid position for the driver. thus Alboreto got pole then Comas took it. 
For practive a 'T' (training) is show in the position field, for pre-race
practice 'P' and for free practice 'F' is used.

"-:--.---" is displayed when the driver is on their "out" lap, i.e. at the
start of the first flying lap.

"!" is displayed next to the "Best Lap" if this is the driver's best lap.

"FQ" is displayed if this is the fastest qualifying time for the session,
and hence the pole time.

"DNQ" is displayed in the position field if the driver's current time 
wouldn't get onto the 26 car grid.

A '*' is displayed in front of the car number for the human player's car.
A '%' is displayed in front of the car number for the human player's car
when the computer is driving it in a multi-player game.
A '#' is displayed in front of the car number for the computer's AI cars.

Race example:

Pos Car Lap     Driver               Lap Time  Best Lap   Race Time
-----------------------------------------------------------------------------
 1. #6   31 Jos Verstappen           1:28.746 1:28.329    44:57.723
 2. #1   31 Ayrton Senna             1:29.553 1:28.542     +3.522
19. #25  30 Eric Bernard             1:35.411 1:31.221     1 lap +9.214
20. #30  30 Heinz Frentzen           1:35.045 1:31.425     1 lap +9.688
 3. #5   31 Michael Schumacher       1:29.001 1:28.711     +10.190
21. #26  30 Olivier Panis            1:32.487 1:31.606     1 lap +11.043
22. #4   30 Mark Blundell            1:30.230 1:29.052     1 lap +11.207
24. #9   30 Christian Fittipaldi     1:32.365 1:31.684     1 lap +16.399
 4. #20  31 Olivier Beretta          1:29.432 1:29.053     +38.979
 5. #27  31 Jean Alesi               1:29.479 1:29.178     +40.601
 6. #33  31 Paul Belmondo            1:30.032 1:29.050     +42.685
 7. #32  31 Ratzenberger             1:30.110 1:29.851     +45.189
 8. #2   31 Damon Hill               -:--.--- 1:29.432     +47.015
 9. #16  31 Eddie Irvine             1:30.577 1:30.199     +50.239
25. #12  30 Pedro Lamy               -:--.--- 1:32.186     1 lap +54.745
10. #28  31 Gerhard Berger           -:--.--- 1:29.652     +56.484
11. #22  31 Nico Napoli              1:30.411 1:30.242     +59.627
12. #19  31 Erik Comas               -:--.--- 1:29.937     +1:11.264
13. #15  31 Eddie Irvine             1:32.866 1:30.578     +1:12.404
14. #7   31 Mika Hakkinen            1:33.296 1:30.494     +1:12.621
15. #23  31 Pierluigi Martini        1:31.171 1:30.074     +1:12.825
16. #24  31 Michele Alboreto         1:31.086 1:30.924     +1:19.366
 1. #6   32 Jos Verstappen           1:28.495 1:28.329    46:26.218
 2. #1   32 Ayrton Senna             1:28.922 1:28.542     +3.949
17. #3   31 Ukyo Katayama            -:--.--- 1:30.708     1 lap +10.489

"-:--.---" is displayed at the start of the race and when the cars pit
(actually this is the first flying lap after they pit). Note no entry is
logged for the lap when the driver is in the pits.

"!" is displayed next to the "Best Lap" if this is the driver's best lap.

"FL" is displayed if this is the fastest lap time for the race.

Lapped cars entries appear in the list as they cross the line, thus the
order can become a bit muddled!

A '*' is displayed in front of the car number for the human player's car.

A '#' is displayed in front of the car number for the computers AI cars.

Tips
---- 
Add GPLAPTIM.COM to your AUTOEXEC.BAT file. Note that GPLAPTIM.COM will
create its GPLAPTIM.LOG file in the directory where you first load
GPLAPTIM.COM (if you don't use the -f option to specify a pathname). Also
note that GPLAPTIM.LOG can grow in size quite fast, so automatically
deleting it each time you boot your PC would be a good idea. If you want
to keep a log file then rename it and specify the optional filename when
running GPLAP (or GPLAPW) to display its contents.

So use something like:

     AUTOEXEC.BAT fragment
     ---------------------
     CD \GPRIX
     GPLAPTIM
     IF EXIST GPLAPTIM.LOG DEL GPLAPTIM.LOG
     CD \

The GPLAPTIM TSR only takes up 2.5KB of your DOS memory so you shouldn't
have a problem with loading this and running GP.EXE. If you have upper
memory blocks available then you can load the TSR high.

To save the output of GPLAP use:

     GPLAP > INFO.TXT

Use the "Save As" option in GPLAPW (or the "Copy" to clipboard menu item).

How GpLapTim Works
------------------
When you run GPLAPTIM.COM the TSR installs itself in memory and sits and
waits for GP.EXE to be run.

When GP.EXE is run GPLAPTIM hooks itself into the game. Every time a car
crosses the start/finish line a small binary data record is appended to
the GPLAPTIM.LOG file. If GPLAPTIM.LOG doesn't exist then it is automatically
created.

The record size for each car is currently 44 bytes, so for a 71 lap
race GPLAPTIM.COM will create a GPLAPTIM.LOG file of about:

      44 bytes * 26 cars * 71 laps = 81244 (79Kb).

If your disk is full GPLAPTIM will just stop logging entries until free
space is available again. Note that if your disk did fill up then a partial
record may have been written and this may cause problems.

Every time you load GP.EXE a date/time entry is added to GPLAPTIM.LOG
to aid identication of particular races/qualifying.

Multi-taskers
-------------
For those of you with Windows or a real operating system like OS/2 <g>
you can run GPLAP.EXE in one DOS window and GP.EXE in another window.

This is useful when watching a qualifying session, as you can see who
goes out to qualify and see the entire grid rather than just the small
window the game shows you.

Use the GPLAP "-w" option to cause GPLAP to wait forever for new entries
in the GPLAPTIM.LOG file. Press <Esc> to terminate GPLAP.

GPLAP is multi-tasker friendly and will release CPU cycles when it has
nothing to do. GPLAPTIM.COM and GPLAP both support file sharing of 
GPLAPTIM.LOG, so both programs accessing the same file at the same time
is possible.

Bugs/Features
-------------
If you have a slow hard disk the game will pause very slightly as entries
are written to the disk. 

The best solution is to use a RAM disk as the output for the log file, use
the -f option to specify the pathname. The RAM disk should have at least
150Kb of free disk space, as a 100% race distance and qualifying could fill
this up.

Another solution to this is to use the -p option to only log the player's
car, thus the pause will only occur whilst you cross the start/finish line
(and are hopefully travelling in a straight line!).

Other things that help are keeping the GPLAPTIM.LOG file as small as
possible. Running SMARTDRV with write-behind caching enabled is another
solution.

Further Info
------------
Please send any suggestions, comments, or bug reports (include the version 
of GPLAPTIM you are using and the version of the game EXE) to the author at:

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