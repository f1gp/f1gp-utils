/*
** File   : ccpit.h
** Author : TK
** Date   :  9/06/94
**
** $Header:   L:/ccpit/vcs/ccpit.h__   1.3   31 Oct 1994 22:30:24   tk  $
**
** Computer Car Pit Strategy.
*/

#ifndef   ccpit_h
#define   ccpit_h

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define MAX_CARS                   26

#define MAX_GROUPS                 2
#define MAX_PITS_PER_GROUP         3

#define DEFAULT_MAX_CARS_IN_PIT    10

/*
** Magic numbers, note these are used as the driver's names.
*/
#define MAGIC1SG1                  0x00393323L    /* "#39" */
#define MAGIC1SG2                  0x00303423L    /* "#40" */
#define MAGIC1SG3                  0x00373323L    /* "#37" */
#define MAGIC1SG4                  0x00383323L    /* "#38" */
#define MAGIC2                     0x55ac

/*
** Car Data structure SI indexes.
*/
#define CAR_DATA_SI_LAP_NUMBER     0x22

/*
** Tyre compounds
*/
#define COMPOUND_A                 0
#define COMPOUND_B                 1
#define COMPOUND_C                 2
#define COMPOUND_D                 3
#define COMPOUND_W                 4
#define COMPOUND_Q                 5

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef unsigned char    bool;
typedef unsigned char    byte;
typedef unsigned short   word;

/*
** Must keep this structure size * MAX_GROUPS <= 18 bytes.
** Currently: (1 + 1 + 1 + 1 + 3) * 2 = 14 bytes.
*/
typedef struct {
     byte      current_index;
     byte      max_index;
     byte      num_cars;
     byte      tyres;
     byte      percent[MAX_PITS_PER_GROUP];
} PIT_GROUP;

/*
** Keep this structure size <= 24 bytes.
*/
typedef struct {
     long      magic1;                            /*  4 bytes */
     PIT_GROUP pit_groups[MAX_GROUPS];            /* 18 bytes */
     word      magic2;                            /*  2 bytes */
} SAVE_GAME1;                                     /* 24 bytes */

/*
** Keep this structure size <= 24 bytes.
*/
typedef struct {
     long      magic1;                            /*  4 bytes */
     byte      seed_group[MAX_CARS / 2];          /* 13 bytes */
     byte      max_cars_in_pit;                   /*  1 bytes */
     byte      randomise;                         /*  1 byte  */
     byte      tyres;                             /*  1 byte  */
     byte      multiplayer;                       /*  1 byte  */
     word      magic2;                            /*  2 bytes */
} SAVE_GAME2;                                     /* 23 bytes */

/*
** Keep this structure size <= 24 bytes.
*/
typedef struct {
     long      magic1;                            /*  4 bytes */
     byte      last_pitstop1[MAX_CARS / 2];       /* 13 bytes */
     word      magic2;                            /*  2 bytes */
} SAVE_GAME3;                                     /* 19 bytes */

typedef struct {
     long      magic1;                            /*  4 bytes */
     byte      last_pitstop2[MAX_CARS / 2];       /* 13 bytes */
     word      magic2;                            /*  2 bytes */
} SAVE_GAME4;                                     /* 19 bytes */

typedef struct {
     unsigned char si[0xc0];
} CAR;

/*
** Per car setup.
*/
typedef struct {
     byte      front_wing;
     byte      rear_wing;
     byte      gear1;
     byte      gear2;
     byte      gear3;
     byte      gear4;
     byte      gear5;
     byte      gear6;
     byte      race_tyres;
     byte      brake_bias;
} CAR_SETUP;

/*---------------------------------------------------------------------------
** Data
*/

extern CAR          far *pFirstCar;
extern SAVE_GAME1   far *pSaveGame1;
extern SAVE_GAME2   far *pSaveGame2;
extern SAVE_GAME3   far *pSaveGame3;
extern SAVE_GAME4   far *pSaveGame4;
extern byte         far *hook_nc_value;

extern PIT_GROUP    tmp_pit_groups[MAX_GROUPS];
extern byte         tmp_max_cars_in_pit;
extern bool         tmp_randomise;
extern byte         tmp_tyres;
extern bool         tmp_multiplayer;

extern char         unload_flag;
extern char         far *cmd_line;
extern char         cmd_line_len;
extern char         near *msg_text;

/*---------------------------------------------------------------------------
** Functions
*/

byte rnd(void);

#endif    /* ccpit_h */
/*---------------------------------------------------------------------------
** End of File
*/


