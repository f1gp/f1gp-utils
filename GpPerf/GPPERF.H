/*
** File   : gpperf.h
** Author : TK
** Date   :  9/06/94
**
** $Header:   F:/TK/GPPERF/VCS/GPPERF.H__   1.5   15 Oct 1995 16:06:34   tk  $
**
** Car Performance.
*/

#ifndef   gpperf_h
#define   gpperf_h

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE                  1
#define FALSE                 0

/*
** The current version of the log structure.
*/
#define VERSION_MAJOR         2

/*
** Top nibble of track segment ID stores the major version number of GpPerf.
** This is for viewer programs which should be able to support old log formats
** as the amount of data logged per entry increases as GpPerf is extended.
** If the top bit (0x0800) of the track segment is set the car is in the
** pit lane. Note that the start and end of the pit lane have unique segment
** ids, but the straight part of the pit uses the same value as the track,
** thus IN_PIT_LANE is required to differentiate between them.
*/
#define LOG_VERSION(v)        ((v >> 12) & 0x0f)
#define TRACK_SEGMENT_ID(v)   (v & 0x0fff)
#define IN_PIT_LANE_MASK      0x800

/*
** Circuits
*/
#define CIR_PHOENIX           0              /* Phoenix - United States    */
#define CIR_INTERLAGOS        1              /* Interlagos - Brazil        */
#define CIR_IMOLA             2              /* Imola - San Marino         */
#define CIR_MONTE             3              /* Monte-Carlo - Monaco       */
#define CIR_MONTREAL          4              /* Montreal - Canada          */
#define CIR_MEXICO            5              /* Mexico City - Mexico       */
#define CIR_MAGNY             6              /* Magny Cours - France       */
#define CIR_SILVERSTONE       7              /* Silverstone - Great Britain*/
#define CIR_HOCKENHEIM        8              /* Hockenheim - Germany       */
#define CIR_HUNGARORING       9              /* Hungaroring - Hungary      */
#define CIR_SPA              10              /* Spa-Francorchamps - Belgium*/
#define CIR_MONZA            11              /* Monza - Italy              */
#define CIR_ESTORIL          12              /* Estoril - Portugal         */
#define CIR_BARCELONA        13              /* Barcelona - Spain          */
#define CIR_SUZUKA           14              /* Suzuka - Japan             */
#define CIR_ADELAIDE         15              /* Adelaide - Australia       */

/*
** Car numbers.
*/
#define CAR_NUMBER_MASK       0x3f
#define PLAYERS_CAR           0x80
#define CAR_NUMBER(car_num)   ((car_num) & CAR_NUMBER_MASK)

/*
** Time mask.
** All times are stored in ms. The game uses the top three bits for other
** purposes. If these are set then the time isn't valid.
*/
#define TIME_MASK             0xe0000000UL

/*
** Tyres compounds.
*/
#define TYRE_COMPOUND_A       0
#define TYRE_COMPOUND_B       1
#define TYRE_COMPOUND_C       2
#define TYRE_COMPOUND_D       3
#define TYRE_COMPOUND_Q       4
#define TYRE_COMPOUND_W       5

/*
** Gears
*/
#define GEAR_REVERSE          255
#define GEAR_NEUTRAL          0
#define GEAR_1                1
#define GEAR_2                2
#define GEAR_3                3
#define GEAR_4                4
#define GEAR_5                5
#define GEAR_6                6
#define GEAR_SPIN             7

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef unsigned char    _byte;
typedef unsigned short   _word;
typedef unsigned long    _dword;

/*
** Note speed is always logged in MPH, to get KPH multiply by 1.609
*/
typedef struct {
     _byte     car_number;
     _byte     circuit;
     _word     track_segment_id;
     _dword    time;
     _byte     tyre_compound;
     _byte     gear;
     _word     revs;
     _word     speed;
     _word     steering;
     _word     accelerator;
     _word     brake;
     _dword    x;
     _dword    y;
} PERFORMANCE_ENTRY;

/*---------------------------------------------------------------------------
** Data
*/

extern char         unload_flag;
extern char    far  *cmd_line;
extern char         cmd_line_len;
extern char    near *msg_text;
extern char         beep_count;
extern char    far  *fname_ptr;
extern char         pending_enable;
extern char         always_enabled;
extern short        log_laps;
extern short        rev_limit1;
extern short        rev_limit2;
extern short        rev_limit3;
extern char         show_tyres;
extern short        log_car_num;

/*---------------------------------------------------------------------------
** Functions
*/

#endif    /* gpperf_h */
/*---------------------------------------------------------------------------
** End of File
*/


