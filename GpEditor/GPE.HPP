/*
** File   : gpe.hpp
** Author : TK
** Date   : 22/02/94
**
** $Header:   L:/GPEDITOR/VCS/GPE.HP_   1.3   01 Oct 1994 15:27:52   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
*/

#ifndef   gpe_hpp
#define   gpe_hpp

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define GPE_HEADER_MAGIC                0x2a455047L    /* 'GPE*' */
#define GPE_RECORD_MAGIC                0x4b54

#define GPE_RT_EOF                      1
#define GPE_RT_COMMENT                  2
#define GPE_RT_EXE_CAR_COLOURS          3
#define GPE_RT_EXE_HELMET_COLOURS       4
#define GPE_RT_EXE_CAR_POWER            5
#define GPE_RT_EXE_CAR_NUMBERS          6
#define GPE_RT_EXE_DRIVER_QUALIFYING    7
#define GPE_RT_EXE_DRIVER_RACE          8
#define GPE_RT_GAME_NAMES               9
#define GPE_RT_EXE_AI_CAR_GRIP         10
#define GPE_RT_EXE_PLAYERS_BHP         11

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct {
     unsigned long  magic;
     unsigned char  gpeditor_version_major;
     unsigned char  gpeditor_version_minor;
     unsigned long  time;
} GPE_HEADER;

typedef struct {
     unsigned short magic;     
     unsigned char  type;     
     unsigned short size;
     unsigned short crc;
} GPE_RECORD;

typedef struct {
     char                     comment[1];    /* Size defined by rec.size */     
} GPE_REC_COMMENT;

typedef struct {
     GP_EXE_CAR_COLOURS       car_colours;     
} GPE_REC_EXE_CAR_COLOURS;

typedef struct {
     GP_EXE_HELMET_COLOURS    helmet_colours;     
} GPE_REC_EXE_HELMET_COLOURS;

typedef struct {
     unsigned char            driving_aids[5];     
} GPE_REC_EXE_DRIVING_AIDS;

typedef struct {
     GP_EXE_CAR_POWER         car_power;
     GP_EXE_DRIVER_QUALIFYING driver_qualifying;
     GP_EXE_DRIVER_RACE       driver_race;
} GPE_REC_EXE_PERFORMANCE;

typedef struct {
     GP_EXE_CAR_NUMBER        car_numbers;     
} GPE_REC_EXE_CAR_NUMBERS;

typedef struct {
     GP_NAMES                 names;     
} GPE_REC_GAME_NAMES;

typedef struct {
     unsigned short           grip;
} GPE_REC_AI_CAR_GRIP;

typedef struct {
     unsigned short           bhp;
} GPE_REC_PLAYERS_BHP;

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/


#endif    /* gpe_hpp */
/*---------------------------------------------------------------------------
** End of File
*/

