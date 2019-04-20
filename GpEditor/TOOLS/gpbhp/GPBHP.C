/*
** File   : gpbhp.c
** Author : TK 
** Date   : 30/07/95
**
** $Header:   F:/TK/GPEDITOR/TOOLS/GPBHP/VCS/GPBHP.C__   1.0   30 Jul 1995 17:04:42   tk  $
**
** DOS Command line util to change BHP and AIG.
**
** V3.0 30/07/95    First bash.
*/

#define VERSION     "V1.0 30-Jul-95"

/*lint -e754 Ignore unreferenced structure members */
/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <stdlib.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE                                 1
#define FALSE                                0

#define NUM_ELEMENTS(x)                      (sizeof(x) / sizeof(x[0]))

#define GP_EXE_NUM_TEAMS                     20

/*
** Location of car design data within game EXE.
*/
#define GP_EXE_V103_EUR_CAR_DESIGNS          0x024dc3L
#define GP_EXE_V104_USA_CAR_DESIGNS          0x026af8L
#define GP_EXE_V105_EUR_CAR_DESIGNS          0x026b24L
#define GP_EXE_V105_ITA_CAR_DESIGNS          0x026af0L

#define GP_EXE_V103_EUR_AI_CAR_GRIP          0x004e83L
#define GP_EXE_V104_USA_AI_CAR_GRIP          0x004ed7L
#define GP_EXE_V105_EUR_AI_CAR_GRIP          0x004ed7L
#define GP_EXE_V105_ITA_AI_CAR_GRIP          0x004ed7L

#define GP_EXE_V103_EUR_PLAYERS_PERF         0x004d44L
#define GP_EXE_V104_USA_PLAYERS_PERF         0x004d88L
#define GP_EXE_V105_EUR_PLAYERS_PERF         0x004d88L
#define GP_EXE_V105_ITA_PLAYERS_PERF         0x004d88L

#define ASM_NOP                              0x90
#define ASM_ADD_AX_BYTE1                     0x81
#define ASM_ADD_AX_BYTE2                     0xc0

/*
** Car colours actual file size after taking into account that the last
** two cars are in one packed record.
*/
#define COLOURS_FILE_FRAGMENT_SIZE           ((sizeof(GP_EXE_car_colours[0]) * 18) + sizeof(PACKED_REC))

/*
** AIG
*/
#define NORMAL_OFFSET         0x4000
#define OFFSET_TO_GRIP(v)     ((((v) - NORMAL_OFFSET) / 100) + 1)
#define GRIP_TO_OFFSET(v)     (((((v) - 1) * 100) + NORMAL_OFFSET))

#define AIG_NORMAL            1
#define AIG_MIN               1
#define AIG_MAX               100

/*
** BHP
*/
#define MEM_VAL_TO_BHP(mval)  (((mval) - 632U) / 22U)
#define BHP_TO_MEM_VAL(bhp)   ((((unsigned short) bhp) * 22U) + 632U)

#define BHP_NORMAL            716
#define BHP_MAX               999
#define BHP_MIN               450

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef unsigned int     BOOL;

typedef struct {
     unsigned char  pad1[3];
     unsigned char  colour;
     unsigned char  pad2[3];
} PACKED_REC;

typedef struct {
     char           pling;
     unsigned char  colours[15];
} GP_EXE_CAR_COLOURS;

typedef struct {
     unsigned char  opcodes[3];
     unsigned short grip;
} AI_CAR_GRIP;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void Usage(void);
static BOOL FileOpenGameExe(char *filename);
static BOOL IsGameExe(FILE *fp);
static BOOL IsGameExeColoursPresent(void);
static BOOL ReadAiCarGrip(FILE *fp);
static BOOL ReadPlayersCarPerf(FILE *fp);
static BOOL FileUpdateGameExe(char *filename);

/*---------------------------------------------------------------------------
** Data
*/

static char    title_msg[] = "@(#)"
                             "GpBHP " VERSION " - Modify BHP and AIG for Grand Prix/World Circuit.\n"
                             "Copyright (c) Trevor Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.\n\n";

GP_EXE_CAR_COLOURS            GP_EXE_car_colours[GP_EXE_NUM_TEAMS] = { 0 };
unsigned short                GP_EXE_AiCarGrip;
unsigned short                GP_EXE_PlayersCarPerf;

AI_CAR_GRIP                   CarGrip;

long                          lGameExeColourFileOffset;
long                          lGameExeAiCarGripFileOffset;
long                          lGameExePlayersCarPerfFileOffset;

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     char *exe_filename  = "GP.EXE";
     BOOL a_flag         = FALSE;
     BOOL b_flag         = FALSE;
     int  aig            = 1;
     int  bhp            = 716;

     for (--argc, ++argv; argc; argc--, argv++) {
          if (argv[0][0] == '-' || argv[0][0] == '/') {
               switch (argv[0][1]) {
               case 'a':
                    aig = atoi(&argv[0][2]);
                    a_flag = TRUE;
                    break;

               case 'b':
                    bhp = atoi(&argv[0][2]);
                    b_flag = TRUE;
                    break;

               case '?':
               case 'h':
                    Usage();
                    break;

               default:
                    (void) printf("GpGap: unknown option '%s'\n", argv[0]);
                    Usage();
                    break;
               }
          }
          else {
               exe_filename = argv[0];
          }
     }

     if (bhp < BHP_MIN || bhp > BHP_MAX) {
          (void) printf("GpGap: BHP must be between %u and %u.\n",
                                        BHP_MIN, BHP_MAX);
          exit(1);
     }

     if (aig < AIG_MIN || aig > AIG_MAX) {
          (void) printf("GpGap: AIG must be between %u and %u.\n",
                                        AIG_MIN, AIG_MAX);
          exit(1);
     }

     if (FileOpenGameExe(exe_filename)) {
          if (!a_flag && !b_flag) {
               (void) printf("Current settings are: BHP:%u AIG:%u.\n",
                         MEM_VAL_TO_BHP(GP_EXE_PlayersCarPerf),
                         OFFSET_TO_GRIP(GP_EXE_AiCarGrip));
          }
          else {
               GP_EXE_AiCarGrip         = (unsigned short) GRIP_TO_OFFSET(aig);
               GP_EXE_PlayersCarPerf    = (unsigned short) BHP_TO_MEM_VAL(bhp);

               if (FileUpdateGameExe(exe_filename)) {
                    (void) printf("Updated game EXE (%s), BHP:%u AIG:%u.\n",
                              exe_filename, bhp, aig);
               }
          }
     }

     return 0;
}

/*---------------------------------------------------------------------------
** Purpose:    Display usage message.
**
** Returns:    
**
** Notes  :    
*/
static void
Usage(
     void
) {
     (void) printf(&title_msg[4]);
     (void) printf("Usage: GpBHP [-?Ýh] [-aN] [-bN] [GP.EXE]\n");
     (void) printf("           -aN  Set AIG (grip level), range %u to %u, default %u.\n", AIG_MIN, AIG_MAX, AIG_NORMAL);
     (void) printf("           -bN  Set player's BHP (power), range %u to %u, default %u.\n", BHP_MIN, BHP_MAX, BHP_NORMAL);
     (void) printf("           -?h  This usage message.\n");
     exit(1);
}

/*---------------------------------------------------------------------------
** Purpose:    Attempts to open the user specified game EXE.
**
** Returns:    True if game EXE opened OK, false if not.
**
** Notes  :    Tries various intelligent checks to work out which version of
**             the game EXE it is.
*/
BOOL
FileOpenGameExe(
     char *filename                     /* In  Filename to open            */
) {
     BOOL      ok   = TRUE;
     FILE      *fp;

     if ((fp = fopen(filename, "rb")) != NULL) {
          ok = IsGameExe(fp);

          if (!ok) {
               (void) printf("GpBHP: '%s' is not a valid or known version of the game EXE.\n", filename);
          }
          (void) fclose(fp);
     }
     else {
          ok = FALSE;
          (void) printf("GpBHP: Unable to open '%s'\n", filename);
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Reads the game EXE car colour table and checks its validity.
**
** Returns:    True if car colour table read OK, false otherwise.
**
** Notes  :    Performs intelligent search for game EXE car colour table.
*/
BOOL
IsGameExe(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok;

     /*
     ** Check known locations first (smaller files first as well).
     */
     lGameExeColourFileOffset = GP_EXE_V103_EUR_CAR_DESIGNS;
     ok = (fseek(fp, lGameExeColourFileOffset, SEEK_SET) == 0);
     if (ok) {
          /*
          ** Note: not sizeof(GP_EXE_car_colours) !
          */
          ok = fread(GP_EXE_car_colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
     }


     if (ok) {
          if (IsGameExeColoursPresent()) {
               lGameExeAiCarGripFileOffset        = GP_EXE_V103_EUR_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V103_EUR_PLAYERS_PERF;
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               return TRUE;
          }
     }

     lGameExeColourFileOffset = GP_EXE_V105_EUR_CAR_DESIGNS;
     ok = (fseek(fp, lGameExeColourFileOffset, SEEK_SET) == 0);
     if (ok) {
          /*
          ** Note: not sizeof(GP_EXE_car_colours) !
          */
          ok = fread(GP_EXE_car_colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
     }

     if (ok) {
          if (IsGameExeColoursPresent()) {
               lGameExeAiCarGripFileOffset        = GP_EXE_V105_EUR_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V105_EUR_PLAYERS_PERF;
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               return TRUE;
          }
     }

     lGameExeColourFileOffset = GP_EXE_V104_USA_CAR_DESIGNS;
     ok = (fseek(fp, lGameExeColourFileOffset, SEEK_SET) == 0);
     if (ok) {
          /*
          ** Note: not sizeof(GP_EXE_car_colours) !
          */
          ok = fread(GP_EXE_car_colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
     }

     if (ok) {
          if (IsGameExeColoursPresent()) {
               lGameExeAiCarGripFileOffset        = GP_EXE_V104_USA_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V104_USA_PLAYERS_PERF;
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               return TRUE;
          }
     }

     lGameExeColourFileOffset = GP_EXE_V105_ITA_CAR_DESIGNS;
     ok = (fseek(fp, lGameExeColourFileOffset, SEEK_SET) == 0);
     if (ok) {
          /*
          ** Note: not sizeof(GP_EXE_car_colours) !
          */
          ok = fread(GP_EXE_car_colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
     }

     if (ok) {
          if (IsGameExeColoursPresent()) {
               lGameExeAiCarGripFileOffset        = GP_EXE_V105_ITA_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V105_ITA_PLAYERS_PERF;
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               return TRUE;
          }
     }

     return FALSE;
}

/*---------------------------------------------------------------------------
** Purpose:    Checks colour table data structure for known good values.
**
** Returns:    True if colour table contains valid data, false if not.
**
** Notes  :    
*/
BOOL
IsGameExeColoursPresent(
     void
) {
     BOOL           ok = TRUE;
     unsigned short i;
     unsigned short matches = 0;
     
     /*
     ** Note: We check the a percentage number of teams, not the actual
     ** number of teams as F1ED seems to splat the '!' for some of them.
     ** The last two cars aren't there as there in a packed record.
     */
     for (i = 0; ok && i < NUM_ELEMENTS(GP_EXE_car_colours) - 2; i++) {
          if (GP_EXE_car_colours[i].pling == '!') {
               ++matches;
          }
     }

     /*
     ** Assume >= 75% matches is OK.
     */
     if (matches < NUM_ELEMENTS(GP_EXE_car_colours) - (NUM_ELEMENTS(GP_EXE_car_colours) / 4)) {
          ok = FALSE;
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Read in AI Car Grip setting.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
ReadAiCarGrip(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     if (lGameExeAiCarGripFileOffset) {
          ok = (fseek(fp, lGameExeAiCarGripFileOffset, SEEK_SET) == 0);
          if (ok) {
               ok = fread(&CarGrip, sizeof(CarGrip), 1, fp);
               GP_EXE_AiCarGrip = CarGrip.grip;
               if (CarGrip.opcodes[1] == ASM_ADD_AX_BYTE1) {
                    /*
                    ** Change value from an offset from 0x4000 due to ADD.
                    */
                    GP_EXE_AiCarGrip += 0x4000;
               }
          }
     }
     else {
          ok = FALSE;
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Read in AI Car Grip setting.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
ReadPlayersCarPerf(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     if (lGameExePlayersCarPerfFileOffset) {
          ok = (fseek(fp, lGameExePlayersCarPerfFileOffset, SEEK_SET) == 0);
          if (ok) {
               ok = fread(&GP_EXE_PlayersCarPerf, sizeof(GP_EXE_PlayersCarPerf), 1, fp);
          }
     }
     else {
          ok = FALSE;
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Updates the game EXE file with the current colour table.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
FileUpdateGameExe(
     char *filename
) {
     BOOL ok   = TRUE;
     FILE *fp;
     
     if ((fp = fopen(filename, "rb+")) != NULL) {
          if (ok && lGameExeAiCarGripFileOffset) {
               ok = (fseek(fp, lGameExeAiCarGripFileOffset, SEEK_SET) == 0);
               if (ok) {
                    if (GP_EXE_AiCarGrip == 0x4000) {
                         /*
                         ** Restore original.
                         **
                         ** JNS ....
                         ** MOV AX, 4000H
                         */
                         CarGrip.opcodes[0] = 0x79;
                         CarGrip.opcodes[1] = 0x03;
                         CarGrip.opcodes[2] = 0xb8;
                    }
                    else {
                         /*
                         ** Change
                         **
                         ** NOP
                         ** ADD AX, offset
                         */
                         CarGrip.opcodes[0] = ASM_NOP;
                         CarGrip.opcodes[1] = ASM_ADD_AX_BYTE1;
                         CarGrip.opcodes[2] = ASM_ADD_AX_BYTE2;
                    }

                    CarGrip.grip = GP_EXE_AiCarGrip;
                    if (CarGrip.opcodes[1] == ASM_ADD_AX_BYTE1) {
                         /*
                         ** Change value to an offset from 0x4000 due to ADD.
                         */
                         CarGrip.grip -= 0x4000;
                    }
                    ok = fwrite(&CarGrip, sizeof(CarGrip), 1, fp);
               }
          }

          if (ok && lGameExePlayersCarPerfFileOffset) {
               ok = (fseek(fp, lGameExePlayersCarPerfFileOffset, SEEK_SET) == 0);
               if (ok) {
                    ok = fwrite(&GP_EXE_PlayersCarPerf, sizeof(GP_EXE_PlayersCarPerf), 1, fp);
               }
          }

          if (!ok) {
               (void) printf("GpBHP: Failed to write to '%s'\n", filename);
          }
          (void) fclose(fp);
     }
     else {
          ok = FALSE;
          (void) printf("GpBHP: Unable to open '%s'\n", filename);
     }

     return ok;
}

/*---------------------------------------------------------------------------
** End of File
*/

