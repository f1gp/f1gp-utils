SURFACE_TYPE_TRACK      equ     0
SURFACE_TYPE_CURB_LOW   equ     1
SURFACE_TYPE_CURB_HIGH  equ     2
SURFACE_TYPE_GRASS      equ     3

GP_VER_UNK              equ     -1
GP_VER_EUR              equ     0
GP_VER_USA              equ     1
GP_VER_ITA              equ     2

; ---------------------------------------------------------------------------

Segment         struc ; (sizeof=0x2E)
wAngleZ         dw ?
wAngleX         dw ?
wPosX           dw ?
wPosZ           dw ?
wPosY           dw ?
flagsTexture    dw ?
wLeftAndRightSideX dw ?
wLeftAndRightSideY dw ?
flags_10        db ?                    ; Number of following Segs where ccLine is within
                                        ; border (maximum of left and right side).
                                        ; If left side value was used, bit 80 is also set
bExtraSideX     db ?
flags_12        db ?                    ; 2=s/f 10=tc0x92 20=tc0x93
                                        ; 40=wCCLine outside right border  80 wCCLine outside left border
bExtraSideY     db ?
wAngleZChangeMulHalfPI dw ?
wCCLine         dw ?
wCCLineRAngle   dw ?
wNr             dw ?
wSegDist        dw ?
bObjectID       db ?
flagsPitlaneJoin db ?
bViewDistance   db ?                    ; 60 or from cmd 0xa9 (pitlane only), or from cmd 81/82/90/91
bFineOffset?_21 db ?                    ; lo nibble X, hi nibble Y
bKerbColourFlags db ?                   ; if 0Ch set kerb colour changes on segment
bKerbColourIndex db ?                   ; kerb colour table index
flags_TC0xa6And0xa7 dw ?
flags_26        db ?                    ; pitlane parking zone: bits C0 set.
field_27        db ?
field_28        db ?                    ; count for consecutive Segs with bit 20h set in flags_26.
                                        ; Segs with even number hold count when going from back to front,
                                        ; Segs with odd numbers hold count when goint from front to back.
field_29        db ?                    ; like field_28, but for bit 10h of flags_26
field_2A        db ?                    ; pitlane parking zone: bit 80 set
field_2B        db ?
field_2C        dw ?
Segment         ends

; ---------------------------------------------------------------------------

Car                     struc ; (sizeof=0xC0)
wSpeedAngleZ            dw ?                                ; like wAngleZ, but rotates 180 degrees when going backwards
wCarAngleX              dw ?                                ; seg X angle * cos(abs(seg Z angle - Car Z angle)), -ve going downhill, +ve going uphill
gapToCarInFront_lo      dw ?
gapToCarInFront_hi      dw ?
wCarPosZ                dw ?                                ; weighted average of seg Z pos + track deviation
wSegPosX                dw ?
wSegPosY                dw ?
wSegPosCCLine           dw ?                                ; weighted average of current and next seg ccline
wSpeed                  dw ?
pSeg                    dd ?
wSegLength              dw ?                                ; length of segment at wSegPosX, so 400h on straights, less on inside of corner, more on outside of corner
flags_18                db ?                                ; 80 = unk
flags_19                db ?
wCarAngleZ              dw ?
wSegDist                dw ?                                ; range around 0...400h, but can be shifted below 0 in outside of corner, or shifted beyond 400h in inside of corner
wSegDistCompleted       dw ?                                ; wSegDist / wSegLength, range around 0 to 4000h
wSegPosXFactor_20       dw ?
bCurLap                 db ?
flags_23                db ?                                ; 1=CCReverseGear 2=unk 4=SpeedBelowMinus469 8=unk 10=unk (speed rel.) 20=in pitlane 40=unk 80=pitting
bGear                   db ?
bTeamNr?                db ?
wSegNr?                 dw ?
wPosX_lo                dw ?
wPosX_hi                dw ?
wPosY_lo                dw ?
wPosY_hi                dw ?
wSpeedX_lo              dw ?
wSpeedX_hi              dw ?
wSpeedY_lo              dw ?
wSpeedY_hi              dw ?
wSpeedZ_lo              dw ?
wSpeedZ_hi              dw ?
flags_3C                db ?                                ; 10=retired 20=pit crew ready? 40=pit crew busy?
flagsNonLinearControl   db ?                                ; 20=nonlinear brake, 40=nonlinear throttle, 80=nonlinear steering
wSegPosX_3E             dw ?
timeLast_lo             dw ?
timeLast_hi             dw ?
weight                  dw ?
tyreGrip                dw ?                                ; of current set, starts at a certain value depending on tyre type
wSteeringAngle          dw ?
field_4A                dw ?                                ; some sort of angle
field_4C                dw ?                                ; front wing efficiency
field_4E                dw ?                                ; rear wing efficiency
field_50                dd ?
timeAbsLast_lo          dw ?
timeAbsLast_hi          dw ?
field_58                dw ?
field_5A                dw ?
field_5C                dw ?                                ; copied from wSpeed
flags_5E                db ?                                ; 1=session over 10=retired 40=in Pit sequence 80=unk
bBrakeRaw               db ?
field_60                dw ?
wRevs                   dw ?
field_64                db ?
field_65                db ?                                ; 2=brake
bTrackOrderIndex        db ?
bPitSeq                 db ?                                ; 0=on track 1=crossed pit s/f 2=on jacks 3=pit menu 4=want leave pit 5=off jacks 6=jacks away 7=lollipop away 8=driving to pit exit
dSpeedSquared           dd ?
field_6C                dw ?
field_6E                dw ?                                ; empty weight / actual weight
wCCSpeed                dw ?                                ; used for CC autobrake and pit speed limit
wAcc                    dw ?                                ; difference with old speed
field_74                dw ?                                ; related to segPosX
field_76                dw ?
field_78                dw ?
pCar_7A                 dw ?
wSteerRaw               dw ?
autoFlags               db ?                                ; 1=steer rel. 2=gear rel. 4=CC  10=spin gear  20=unk  40=unk  80=gear rel.  rest=related to trouble
bDigitalControl         db ?                                ; 1=throttle 2=brake 4=left 8=right 10=gear change 80=using downshift
wNumFeetIntoLap?        dw ?
pCar_82                 dw ?
field_84                db ?
bDrvAids                db ?
field_86                dw ?
field_88                db ?
flagsAnalogControl      db ?                                ; 4=unk, 8=analog throttle, 10=analog steering, 20=analog brake
wSegPosX_8A             dw ?                                ; related to pit? 20 in box, 18 in pitlane, 0 on track
wTrackDeviation         dw ?                                ; height above track
field_8E                dw ?
wPrevAngleZ             dw ?
timerPitSeq_lo          dw ?                                ; also used for pulling away
timerPitSeq_hi          dw ?
flags_96                db ?                                ; 20=no driver in car 40=stop car 80=show cc fuel?
flags_97                db ?                                ; 1=use pitstop strategy in 75%+ race (off=no stops)  2=engine on  4=car on jacks
wThrottle               dw ?
flagsDamage             db ?                                ; 80=wing damage 40=rw 20=fw
bThrottleRaw            db ?
field_9C                dw ?
bFrontWingSetup         db ?
bRearWingSetup          db ?
dTyreWear               dd ?                                ; abs(speed * delta(z angle))
wDrivenDistInSegs       dd ?
wPower                  dw ?
bRacePos                db ?                                ; (race pos - 1) * 2
field_AB                db ?                                ; unused
bID                     db ?
bTeamPitPos             db ?
timeBest_lo             dw ?
timeBest_hi             dw ?
tyreCompound            db ?                                ; 0=A 1=B 2=C 3=D 4=Q 5=W
flags_B3                db ?                                ; 4=pitlight?  20=unk
field_B4                dw ?
field_B6                dw ?
field_B8                dw ?
field_BA                dw ?
flags_BC                db ?                                ; 1=SpeedAbove1877 2=Unk left/right (50% chance each) 80=RevsTooLowToShiftUp
bNumHotseatSwitchesDone db ?
wCornerSpeedFactor      dw ?                                ; derived from driver skill
Car                     ends
