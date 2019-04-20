@echo off
echo GPEDITOR - F1ED V1.3 DTA Files
if not exist f1ed.dta goto nofile
echo Ready to patch F1ED.DTA (DEBUG.EXE must be in your path)
pause
echo Patching...
echo e 110 21  >  dtav13.pat
echo e 186 21  >> dtav13.pat
echo e 1fc 21  >> dtav13.pat
echo e 272 21  >> dtav13.pat
echo e 2e8 21  >> dtav13.pat
echo e 35e 21  >> dtav13.pat
echo e 3d4 21  >> dtav13.pat
echo e 44a 21  >> dtav13.pat
echo e 4c0 21  >> dtav13.pat
echo e 536 21  >> dtav13.pat
echo e 5ac 21  >> dtav13.pat
echo e 622 21  >> dtav13.pat
echo e 698 21  >> dtav13.pat
echo e 70e 21  >> dtav13.pat
echo e 784 21  >> dtav13.pat
echo e 7fa 21  >> dtav13.pat
echo e 870 21  >> dtav13.pat
echo e 8e6 21  >> dtav13.pat
echo e 95c 21  >> dtav13.pat
echo e 9d2 21  >> dtav13.pat
echo w         >> dtav13.pat
echo q         >> dtav13.pat

debug f1ed.dta < dtav13.pat
if exist dtav13.pat del dtav13.pat
echo Done
goto done

:nofile
echo DTAV13: can't open F1ED.DTA
:done
