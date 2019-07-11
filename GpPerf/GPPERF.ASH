


; top of file "gpperf.ash"


gpperf_h            EQU     1h

TRUE                EQU     1

FALSE               EQU     0

VERSION_MAJOR       EQU     2

IN_PIT_LANE_MASK    EQU     0800h

CIR_PHOENIX         EQU     0

CIR_INTERLAGOS      EQU     1

CIR_IMOLA           EQU     2

CIR_MONTE           EQU     3

CIR_MONTREAL        EQU     4

CIR_MEXICO          EQU     5

CIR_MAGNY           EQU     6

CIR_SILVERSTONE     EQU     7

CIR_HOCKENHEIM      EQU     8

CIR_HUNGARORING     EQU     9

CIR_SPA             EQU     10

CIR_MONZA           EQU     11

CIR_ESTORIL         EQU     12

CIR_BARCELONA       EQU     13

CIR_SUZUKA          EQU     14

CIR_ADELAIDE        EQU     15

CAR_NUMBER_MASK     EQU     03fh

PLAYERS_CAR         EQU     080h

TIME_MASK           EQU     <0xe0000000UL>

TYRE_COMPOUND_A     EQU     0

TYRE_COMPOUND_B     EQU     1

TYRE_COMPOUND_C     EQU     2

TYRE_COMPOUND_D     EQU     3

TYRE_COMPOUND_Q     EQU     4

TYRE_COMPOUND_W     EQU     5

GEAR_REVERSE        EQU     255

GEAR_NEUTRAL        EQU     0

GEAR_1              EQU     1

GEAR_2              EQU     2

GEAR_3              EQU     3

GEAR_4              EQU     4

GEAR_5              EQU     5

GEAR_6              EQU     6

GEAR_SPIN           EQU     7

_byte               TYPEDEF BYTE 

_word               TYPEDEF WORD 

_dword              TYPEDEF DWORD 

tag$0               STRUC   
car_number          DB      ?
circuit             DB      ?
track_segment_id    DW      ?
time                DD      ?
tyre_compound       DB      ?
gear                DB      ?
revs                DW      ?
speed               DW      ?
steering            DW      ?
accelerator         DW      ?
brake               DW      ?
x                   DD      ?
y                   DD      ?

tag$0               ENDS

PERFORMANCE_ENTRY   TYPEDEF tag$0















; end of file "gpperf.ash"

