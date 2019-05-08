/*
** File   : gplaptim.h
** Author : TK 
** Date   : 23/04/94
**
** $Header:   F:/TK/GPLAPTIM/VCS/GPLAPTIM.H__   1.7   09 Oct 1995 22:28:20   tk  $
**
** Grand Prix Lap Time Logger for MPS Grand Prix/World Circuit.
*/

#ifndef   gplaptim_h
#define   gplaptim_h

#pragma pack(push, _AFX_PACKING)

#pragma pack(1)

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*
** Record Types.
*/
#define REC_TYPE_LAP          1              /* Car lap entry              */
#define REC_TYPE_DATE         2              /* Date/time GP loaded        */
#define REC_TYPE_AUTH         3              /* Player's authentication    */
#define REC_TYPE_SETUP        4              /* Car Setup                  */
#define REC_TYPE_PIT_IN       5              /* Pit onto jacks             */
#define REC_TYPE_PIT_OUT      6              /* Pit off jacks              */
#define REC_TYPE_SPLIT        7              /* Split Time                 */
#define REC_TYPE_AUTH_RACE    8              /* Player's race auth         */

/*
** Circuits (stored in top nibble of game_mode).
*/
#define CIR_MASK              0xf0
#define CIRCUIT(game_mode)    (((game_mode) & CIR_MASK) >> 4)

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
** Game Modes (stored in bottom nibble of game_mode).
*/
#define GM_MASK               0x0f
#define GAME_MODE(game_mode)  ((game_mode) & GM_MASK)

#define GM_PRACTICE           0              /* Practice menu option       */
#define GM_FREE_PRACTICE      1              /* Race menu - Free Practice  */
#define GM_QUALIFYING         2              /* Race menu - Qualifying     */
#define GM_PRE_RACE_PRACTICE  3              /* Race menu - Pre-Race Pract */
#define GM_RACE               4              /* Race or Quick Race         */

#define IS_PRACTICE(gm)            (GAME_MODE(gm) == GM_PRACTICE)
#define IS_FREE_PRACTICE(gm)       (GAME_MODE(gm) == GM_FREE_PRACTICE)
#define IS_QUALIFYING(gm)          (GAME_MODE(gm) == GM_QUALIFYING)
#define IS_PRE_RACE_PRACTICE(gm)   (GAME_MODE(gm) == GM_PRE_RACE_PRACTICE)
#define IS_RACE(gm)                (GAME_MODE(gm) == GM_RACE)

/*
** Driving Aids.
*/
#define DRV_AID_TRACTION      0x80
#define DRV_AID_STEERING      0x40
#define DRV_AID_F6            0x20
#define DRV_AID_F5            0x10
#define DRV_AID_F4            0x08
#define DRV_AID_F3            0x04
#define DRV_AID_F2            0x02
#define DRV_AID_F1            0x01

/*
** Car numbers.
** IS_PLAYER_CC is set in a local mutli-player game when the CC takes control
** of the players car.
*/
#define CAR_NUMBER_MASK       0x3f
#define PLAYERS_CAR           0x80
#define PLAYERS_CAR_CC        0x40
#define CAR_NUMBER(car_num)   ((car_num) & CAR_NUMBER_MASK)
#define IS_PLAYER(car_num)    ((car_num) & PLAYERS_CAR)
#define IS_PLAYER_CC(car_num) ((car_num) & PLAYERS_CAR_CC)


/*
** Time mask.
** All times are stored in ms. The game uses the top three bits for other
** purposes. If these are set then the time isn't valid.
*/
#define TIME_MASK             0xe0000000UL

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef int              bool;
typedef unsigned char    uchar;
typedef unsigned short   ushort;
typedef unsigned long    ulong;

/*
** Note all records occupy the same same size as a LAP_RECORD.
** This entry is logged whenever a car crosses the s/f line.
*/
typedef struct {
     uchar     lr_record_type;
     uchar     lr_game_mode;
     uchar     lr_car_number;
     uchar     lr_lap_number;
     uchar     lr_qual_position;
     uchar     lr_car_position;
     ulong     lr_lap_time;
     ulong     lr_best_time;
     ulong     lr_race_time;
     uchar     lr_driver[24];
     ushort    lr_crc;
} LAP_RECORD;

/*
** Authenticated lap record. Logged when a best session time is recorded.
** Only logged for player.
*/
typedef struct {
     uchar     ar_record_type;
     uchar     ar_game_mode;
     ulong     ar_lap_time;
     ushort    ar_bhp;
     uchar     ar_ai_grip;
     uchar     ar_drv_aids;
     uchar     ar_skill;
     uchar     ar_seconds;
     uchar     ar_minutes;
     uchar     ar_hours;
     uchar     ar_day;
     uchar     ar_month;
     ushort    ar_year;
     uchar     ar_driver[24];
     ushort    ar_crc;
} AUTH_RECORD;

/*
** Authenticated race time. Logged at end of 100% race.
** Only logged for player.
*/
typedef struct {
     uchar     rr_record_type;
     uchar     rr_game_mode;
     ulong     rr_race_time;
     ushort    rr_bhp;
     uchar     rr_ai_grip;
     uchar     rr_drv_aids;
     uchar     rr_skill;
     uchar     rr_seconds;
     uchar     rr_minutes;
     uchar     rr_hours;
     uchar     rr_day;
     uchar     rr_month;
     ushort    rr_year;
     uchar     rr_driver[24];
     ushort    rr_crc;
} AUTH_RACE_RECORD;

/*
** Date/time GP.EXE is run.
*/
typedef struct {
     uchar     dr_record_type;
     ushort    dr_year;
     uchar     dr_month;
     uchar     dr_day;
     uchar     dr_week_day;
     uchar     dr_hours;
     uchar     dr_minutes;
     uchar     dr_seconds;
} DATE_RECORD;

/*
** Car setup, logged when car crosses s/f line on first lap out.
** Only logged for player.
*/
typedef struct {
     uchar     sr_record_type;
     uchar     sr_game_mode;
     uchar     sr_front_wing;
     uchar     sr_rear_wing;
     uchar     sr_gear1;
     uchar     sr_gear2;
     uchar     sr_gear3;
     uchar     sr_gear4;
     uchar     sr_gear5;
     uchar     sr_gear6;
     uchar     sr_race_tyres;
     char      sr_brake_bias;
     uchar     sr_using_tyres;
} SETUP_RECORD;

/*
** Pit stop onto jacks.
*/
typedef struct {
     uchar     pi_record_type;
     uchar     pi_game_mode;
     uchar     pi_car_number;
     uchar     pi_lap_number;
     uchar     pi_car_position;
     ulong     pi_in_time;
     uchar     pi_driver[24];
} PIT_IN_RECORD;

/*
** Pit stop off jacks.
** (Out - (In + 0.3s)) = ~Duration.
*/
typedef struct {
     uchar     po_record_type;
     uchar     po_game_mode;
     uchar     po_car_number;
     uchar     po_lap_number;
     uchar     po_car_position;
     ulong     po_out_time;
     uchar     po_driver[24];
} PIT_OUT_RECORD;

/*
** Split Time.
*/
typedef struct {
     uchar     st_record_type;
     uchar     st_game_mode;
     uchar     st_car_number;
     uchar     st_lap_number;
     uchar     st_qual_position;
     uchar     st_car_position;
     ulong     st_split_time;
     uchar     st_point;
     ushort    st_percentage;
     uchar     st_driver[24];
} SPLIT_TIME_RECORD;

//
// All record types
//
typedef union {
	LAP_RECORD		lr;
	AUTH_RECORD		ar;
	AUTH_RACE_RECORD	arr;
	DATE_RECORD		dr;
	SETUP_RECORD		sr;
	PIT_IN_RECORD		pi;
	PIT_OUT_RECORD		po;
	SPLIT_TIME_RECORD	st;
} ALL_RECORDS;

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/

#pragma pack(pop, _AFX_PACKING)

#endif    /* gplaptim_h */
/*---------------------------------------------------------------------------
** End of File
*/
