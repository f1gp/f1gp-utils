bcc -S -c -O2 -1 -u- -zCTSR_TMP -zDTSR_TMP -zRTSR_TMP -zATSR -zBTSR -zTTSR -zGTSR -zSTSR %1.c
if exist %1.code del %1.cod
if exist %1.asm ren %1.asm %1.cod
