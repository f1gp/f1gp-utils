@echo off
echo GPEDITOR - F1ED V1.2 DTA Files
if not exist f1ed.dta goto nofile
echo Ready to patch F1ED.DTA (DEBUG.EXE must be in your path)
pause
echo Patching...
echo e 143 21  >  dtav12.pat
echo e 196 21  >> dtav12.pat
echo e 1e9 21  >> dtav12.pat
echo e 23c 21  >> dtav12.pat
echo e 28f 21  >> dtav12.pat
echo e 2e2 21  >> dtav12.pat
echo e 335 21  >> dtav12.pat
echo e 388 21  >> dtav12.pat
echo e 3db 21  >> dtav12.pat
echo e 42e 21  >> dtav12.pat
echo e 481 21  >> dtav12.pat
echo e 4d4 21  >> dtav12.pat
echo e 527 21  >> dtav12.pat
echo e 57a 21  >> dtav12.pat
echo e 5cd 21  >> dtav12.pat
echo e 620 21  >> dtav12.pat
echo e 673 21  >> dtav12.pat
echo e 6c6 21  >> dtav12.pat
echo w         >> dtav12.pat
echo q         >> dtav12.pat

debug f1ed.dta < dtav12.pat
if exist dtav12.pat del dtav12.pat
echo Done

goto done

:nofile
echo DTAV12: can't open F1ED.DTA

:done

