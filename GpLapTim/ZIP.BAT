@echo off
set GPLAPVER=gplap
if exist %GPLAPVER%.zip del %GPLAPVER%.zip
pkzip %GPLAPVER%.zip readme.txt whatsnew.txt gplaptim\gplaptim.com gplap\gplap.exe 
pkzip %GPLAPVER%.zip gplapw\gplapw.exe gplapw\gplapw.hlp ctl3d.dll
