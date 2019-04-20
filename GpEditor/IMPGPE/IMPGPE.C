/*
** File   : impgpe.c
** Author : TK 
** Date   :  4/06/94
**
** DOS Command line "Import GPE" utility.
**
** V3.0 12/06/94    First bash (started at V3.0 to stay in step with GPed).
** V4.0 25/09/94    Added as part of standard GPed release. Updated AIG.
** V4.1 23/06/95    Added support for Italian version.
*/

#define VERSION     "V4.1 23-Jun-95"

/*lint -e754 Ignore unreferenced structure members */
/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE                                 1
#define FALSE                                0

#define NUM_ELEMENTS(x)                      (sizeof(x) / sizeof(x[0]))

#define GP_EXE_NUM_TEAMS                     20
#define GP_EXE_NUM_DRIVERS                   (GP_EXE_NUM_TEAMS * 2)

/*
** Location of car design data within game EXE.
*/
#define GP_EXE_V103_EUR_CAR_DESIGNS          0x024dc3L
#define GP_EXE_V104_USA_CAR_DESIGNS          0x026af8L
#define GP_EXE_V105_EUR_CAR_DESIGNS          0x026b24L
#define GP_EXE_V105_ITA_CAR_DESIGNS          0x026af0L

#define GP_EXE_CAR_POWER_OFFSET              (-0x000078L)
#define GP_EXE_HELMET_COLOUR_OFFSET          (+0x000127L)
#define GP_EXE_CAR_NUMBERS                   (-0x000decL)

#define GP_EXE_V103_EUR_DRIVING_AIDS         0x01c7d6L
#define GP_EXE_V104_USA_DRIVING_AIDS         0x01e4d3L
#define GP_EXE_V105_EUR_DRIVING_AIDS         0x01e4ffL
#define GP_EXE_V105_ITA_DRIVING_AIDS         0x01e4cbL

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

#define GPE_HEADER_MAGIC                     0x2a455047L    /* 'GPE*' */
#define GPE_RECORD_MAGIC                     0x4b54

#define GPE_RT_EOF                           1
#define GPE_RT_COMMENT                       2
#define GPE_RT_EXE_CAR_COLOURS               3
#define GPE_RT_EXE_HELMET_COLOURS            4
#define GPE_RT_EXE_CAR_POWER                 5
#define GPE_RT_EXE_CAR_NUMBERS               6
#define GPE_RT_EXE_DRIVER_QUALIFYING         7
#define GPE_RT_EXE_DRIVER_RACE               8
#define GPE_RT_GAME_NAMES                    9
#define GPE_RT_EXE_AI_CAR_GRIP               10
#define GPE_RT_EXE_PLAYERS_BHP               11

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef unsigned int     BOOL;

typedef struct {
     unsigned short power;
} GP_EXE_CAR_POWER;

typedef struct {
     unsigned char  number;
} GP_EXE_CAR_NUMBER;

typedef struct {
     unsigned char  qualifying;
} GP_EXE_DRIVER_QUALIFYING;

typedef struct {
     unsigned char  race;
} GP_EXE_DRIVER_RACE;

typedef struct {
     char           pling;
     unsigned char  colours[15];
} GP_EXE_CAR_COLOURS;

typedef struct {
     unsigned char  driver01[16];
     unsigned char  driver02[16];
     unsigned char  driver03[16];
     unsigned char  driver04[16];
     unsigned char  driver05[16];
     unsigned char  driver06[16];
     unsigned char  driver07[16];
     unsigned char  driver08[16];
     unsigned char  driver09[16];
     unsigned char  driver10[16];
     unsigned char  driver11[16];
     unsigned char  driver12[16];
     unsigned char  driver13[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver14[16];  
     unsigned char  driver15[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver16[16];
     unsigned char  driver17[16];
     unsigned char  driver18[16];
     unsigned char  driver19[16];
     unsigned char  driver20[16];
     unsigned char  driver21[16];
     unsigned char  driver22[16];
     unsigned char  driver23[16];
     unsigned char  driver24[16];
     unsigned char  driver25[16];
     unsigned char  driver26[16];
     unsigned char  driver27[16];
     unsigned char  driver28[16];
     unsigned char  driver29[16];
     unsigned char  driver30[16];
     unsigned char  driver31[16];
     unsigned char  driver32[16];
     unsigned char  driver33[16];
     unsigned char  driver34[16];
     unsigned char  driver35[16];
     unsigned char  driver36[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver37[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver38[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver39[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver40[14];  /* Packed record, only use first 6 cols */
} GP_EXE_HELMET_COLOURS;

typedef struct {
     char           driver[24];
} GP_NAME_DRIVER;

typedef struct {
     char           team[13];
} GP_NAME_TEAM;

typedef struct {
     char           engine[13];
} GP_NAME_ENGINE;

typedef struct {
     GP_NAME_DRIVER drivers[GP_EXE_NUM_DRIVERS];
     GP_NAME_TEAM   teams[GP_EXE_NUM_TEAMS];
     GP_NAME_ENGINE engines[GP_EXE_NUM_TEAMS];
     unsigned short crc1;
     unsigned short crc2;
} GP_NAMES;

typedef struct {
     unsigned char  pad1[3];
     unsigned char  colour;
     unsigned char  pad2[3];
} PACKED_REC;

typedef struct {
     unsigned char  opcodes[3];
     unsigned short grip;
} AI_CAR_GRIP;

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
     unsigned short           grip;
} GPE_REC_AI_CAR_GRIP;

typedef struct {
     unsigned short           bhp;
} GPE_REC_PLAYERS_BHP;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

BOOL FileOpenGameExe(char *filename);
BOOL IsGameExe(FILE *fp);
BOOL IsGameExeColoursPresent(void);
BOOL ReadPerformance(FILE *fp);
BOOL ReadHelmetColours(FILE *fp);
BOOL ReadDrivingAids(FILE *fp);
BOOL ReadCarNumbers(FILE *fp);
BOOL ReadAiCarGrip(FILE *fp);
BOOL ReadPlayersCarPerf(FILE *fp);
BOOL FileUpdateGameExe(char *filename);

static void UnpackPackedColours(void);
static void SavePackedRecord(void);

BOOL ImportGpe(char *filename);
BOOL GpeReadHeader(FILE *fp, char *filename);
BOOL GpeRead(FILE *fp);

static unsigned short CalcCrc(void const *ptr, unsigned short len);

BOOL FileSaveNames(char *filename);
static void CalculateNamesCrc(GP_NAMES *gp_names);
static void rotate_left3(unsigned short *ptr_us);

/*---------------------------------------------------------------------------
** Data
*/

static char    title_msg[] = "@(#)"
                             "ImpGPE " VERSION " - Import GPE file.\n"
                             "Copyright (c) Trevor Kellaway (CIS:100331,2330) 1994 - All Rights Reserved.\n\n";

GP_EXE_CAR_COLOURS            GP_EXE_car_colours[GP_EXE_NUM_TEAMS];
GP_EXE_CAR_POWER              GP_EXE_car_power[GP_EXE_NUM_TEAMS];
GP_EXE_DRIVER_QUALIFYING      GP_EXE_driver_qualifying[GP_EXE_NUM_DRIVERS];
GP_EXE_DRIVER_RACE            GP_EXE_driver_race[GP_EXE_NUM_DRIVERS];
GP_EXE_HELMET_COLOURS         GP_EXE_helmet_colours;
unsigned short                GP_EXE_AiCarGrip;
unsigned short                GP_EXE_PlayersCarPerf;
GP_NAMES                      GP_names;
GP_EXE_CAR_NUMBER             GP_EXE_car_numbers[GP_EXE_NUM_DRIVERS];
unsigned char                 GP_EXE_driving_aids[5];

PACKED_REC                    PackedRecord;
AI_CAR_GRIP                   CarGrip;

long                          lGameExeColourFileOffset;
long                          lGameExeDrivingAidsFileOffset;
long                          lGameExeAiCarGripFileOffset;
long                          lGameExePlayersCarPerfFileOffset;

char                          game_exe_version[32];

GPE_HEADER                    hdr;
GPE_RECORD                    rec;

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     char *gpe_filename;
     char *nam_filename = "GPE.NAM";
     char *exe_filename = "GP.EXE";

     if (argc == 1) {
          (void) fprintf(stderr, &title_msg[4]);
          (void) fprintf(stderr, "Usage: ImpGPE (GPE filename) [Output Names filename] [GP.EXE]\n");
          return 1;
     }

     gpe_filename = argv[1];
     if (argc >= 3) {
          nam_filename = argv[2];
     }
     if (argc >= 4) {
          exe_filename = argv[3];
     }

     if (FileOpenGameExe(exe_filename)) {
          /*
          ** Allow all driving aids.
          */
          (void) memset(GP_EXE_driving_aids, 0x3f, sizeof(GP_EXE_driving_aids));

          if (ImportGpe(gpe_filename)) {
               if (FileSaveNames(nam_filename)) {
                    (void) printf("Names file written to '%s', ensure the game uses this file.\n", nam_filename);
                    if (FileUpdateGameExe(exe_filename)) {

                         (void) printf("Successfully updated game EXE (%s).\n", exe_filename);
                    }
               }
          }
     }

     return 0;
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
               (void) printf("ImpGPE: '%s' is not a valid or known version of the game EXE.\n", filename);
          }
          (void) fclose(fp);
     }
     else {
          ok = FALSE;
          (void) printf("ImpGPE: Unable to open '%s'\n", filename);
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
          if (ok) {
               SavePackedRecord();
               UnpackPackedColours();
          }
     }

     if (ok) {
          if ( IsGameExeColoursPresent()     && ReadPerformance(fp) &&
               ReadHelmetColours(fp)         && ReadCarNumbers(fp)
          ) {
               lGameExeDrivingAidsFileOffset      = GP_EXE_V103_EUR_DRIVING_AIDS;
               lGameExeAiCarGripFileOffset        = GP_EXE_V103_EUR_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V103_EUR_PLAYERS_PERF;
               (void) ReadDrivingAids(fp);
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               (void) strcpy(game_exe_version, "European V1.03");
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
          if (ok) {
               SavePackedRecord();
               UnpackPackedColours();
          }
     }

     if (ok) {
          if ( IsGameExeColoursPresent()     && ReadPerformance(fp) &&
               ReadHelmetColours(fp)         && ReadCarNumbers(fp)
          ) {
               lGameExeDrivingAidsFileOffset      = GP_EXE_V105_EUR_DRIVING_AIDS;
               lGameExeAiCarGripFileOffset        = GP_EXE_V105_EUR_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V105_EUR_PLAYERS_PERF;
               (void) ReadDrivingAids(fp);
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               (void) strcpy(game_exe_version, "European V1.05");
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
          if (ok) {
               SavePackedRecord();
               UnpackPackedColours();
          }
     }

     if (ok) {
          if ( IsGameExeColoursPresent()     && ReadPerformance(fp) &&
               ReadHelmetColours(fp)         && ReadCarNumbers(fp)
          ) {
               lGameExeDrivingAidsFileOffset      = GP_EXE_V104_USA_DRIVING_AIDS;
               lGameExeAiCarGripFileOffset        = GP_EXE_V104_USA_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V104_USA_PLAYERS_PERF;
               (void) ReadDrivingAids(fp);
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               (void) strcpy(game_exe_version, "US V1.04/V1.05");
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
          if (ok) {
               SavePackedRecord();
               UnpackPackedColours();
          }
     }

     if (ok) {
          if ( IsGameExeColoursPresent()     && ReadPerformance(fp) &&
               ReadHelmetColours(fp)         && ReadCarNumbers(fp)
          ) {
               lGameExeDrivingAidsFileOffset      = GP_EXE_V105_ITA_DRIVING_AIDS;
               lGameExeAiCarGripFileOffset        = GP_EXE_V105_ITA_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V105_ITA_PLAYERS_PERF;
               (void) ReadDrivingAids(fp);
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               (void) strcpy(game_exe_version, "Italian V1.05");
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
** Purpose:    To read in the car and driver performance.
**
** Returns:    True if read OK, false if read or seek failed.
**
** Notes  :    
*/
BOOL
ReadPerformance(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_CAR_POWER_OFFSET, SEEK_SET) == 0);
     if (ok) {
          ok = fread(GP_EXE_car_power, sizeof(GP_EXE_car_power), 1, fp);
     }
     if (ok) {
          ok = fread(GP_EXE_driver_qualifying, sizeof(GP_EXE_driver_qualifying), 1, fp);
     }
     if (ok) {
          ok = fread(GP_EXE_driver_race, sizeof(GP_EXE_driver_race), 1, fp);
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    To read in the helmet colours.
**
** Returns:    True if read OK, false if read or seek failed.
**
** Notes  :    
*/
BOOL
ReadHelmetColours(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_HELMET_COLOUR_OFFSET, SEEK_SET) == 0);
     if (ok) {
          ok = fread(&GP_EXE_helmet_colours, sizeof(GP_EXE_helmet_colours), 1, fp);
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Read in current driving aids settings.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
ReadDrivingAids(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     if (lGameExeDrivingAidsFileOffset) {
          ok = (fseek(fp, lGameExeDrivingAidsFileOffset, SEEK_SET) == 0);
          if (ok) {
               ok = fread(GP_EXE_driving_aids, sizeof(GP_EXE_driving_aids), 1, fp);
          }
     }
     else {
          ok = FALSE;
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Read in current driving aids settings.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
ReadCarNumbers(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_CAR_NUMBERS, SEEK_SET) == 0);
     if (ok) {
          ok = fread(GP_EXE_car_numbers, sizeof(GP_EXE_car_numbers), 1, fp);
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
     BOOL ok;
     FILE *fp;
     
     if ((fp = fopen(filename, "rb+")) != NULL) {
          ok = (fseek(fp, lGameExeColourFileOffset, SEEK_SET) == 0);
          if (ok) {
               /*
               ** Note: not sizeof(GP_EXE_car_colours) !
               ** Update packed car colours, use copy so we don't
               ** trash displayed colours.
               */
               static GP_EXE_CAR_COLOURS Colours[GP_EXE_NUM_TEAMS];
               PACKED_REC *pr = (PACKED_REC *) (void *) &Colours[18];

               (void) memcpy(Colours, GP_EXE_car_colours, sizeof(Colours));
               (void) memcpy(pr, &PackedRecord, sizeof(*pr));
               pr->colour = GP_EXE_car_colours[18].colours[0];
               ok = fwrite(Colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
          }

          if (ok) {
               ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_CAR_POWER_OFFSET, SEEK_SET) == 0);
               if (ok) {
                    ok = fwrite(GP_EXE_car_power, sizeof(GP_EXE_car_power), 1, fp);
               }
          }

          if (ok) {
               ok = fwrite(GP_EXE_driver_qualifying, sizeof(GP_EXE_driver_qualifying), 1, fp);
          }

          if (ok) {
               ok = fwrite(GP_EXE_driver_race, sizeof(GP_EXE_driver_race), 1, fp);
          }

          if (ok) {
               ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_HELMET_COLOUR_OFFSET, SEEK_SET) == 0);
               if (ok) {
                    ok = fwrite(&GP_EXE_helmet_colours, sizeof(GP_EXE_helmet_colours), 1, fp);
               }
          }

          if (ok && lGameExeDrivingAidsFileOffset) {
               ok = (fseek(fp, lGameExeDrivingAidsFileOffset, SEEK_SET) == 0);
               if (ok) {
                    ok = fwrite(GP_EXE_driving_aids, sizeof(GP_EXE_driving_aids), 1, fp);
               }
          }

          if (ok) {
               ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_CAR_NUMBERS, SEEK_SET) == 0);
               if (ok) {
                    ok = fwrite(GP_EXE_car_numbers, sizeof(GP_EXE_car_numbers), 1, fp);
               }
          }

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
               (void) printf("ImpGPE: Failed to write to '%s'\n", filename);
          }
          (void) fclose(fp);
     }
     else {
          ok = FALSE;
          (void) printf("ImpGPE: Unable to open '%s'\n", filename);
     }

     return ok;
}

static void
UnpackPackedColours(
     void
) {
     /*
     ** Unpack packed records.
     */
     PACKED_REC *pr = (PACKED_REC *) (void *) &GP_EXE_car_colours[18];

     (void) memset(&GP_EXE_car_colours[18], pr->colour, sizeof(GP_EXE_car_colours[18]));
     (void) memset(&GP_EXE_car_colours[19], pr->colour, sizeof(GP_EXE_car_colours[19]));
}

static void
SavePackedRecord(
     void
) {
     PACKED_REC *pr = (PACKED_REC *) (void *) &GP_EXE_car_colours[18];
     
     (void) memcpy(&PackedRecord, pr, sizeof(PackedRecord));
}

BOOL
ImportGpe(
     char *filename
) {
     FILE *fp;
     BOOL ok;

     if ((fp = fopen(filename, "rb")) != NULL) {
          ok = GpeReadHeader(fp, filename);
          if (ok) {
               ok = GpeRead(fp);
               if (!ok) {
                    (void) printf("ImpGPE: Read from '%s' failed.", filename);
               }
          }
          else {
               ok = FALSE;
               (void) printf("ImpGPE: '%s' is not a GPE file.", filename);
          }
     }
     else {
          ok = FALSE;
          (void) printf("ImpGPE: Unable to open '%s'", filename);
     }

     return ok;
}

BOOL
GpeReadHeader(
     FILE *fp,                          /* In  File to read from           */
     char *filename
) {
     BOOL ok = TRUE;

     ok = fread(&hdr, sizeof(hdr), 1, fp);
     if (ok) {
          ok = (hdr.magic == GPE_HEADER_MAGIC) ? TRUE : FALSE;
     }

     if (ok) {
          (void) printf("%s - (GPeditor V%u.%u) %.24s [%s]\n",  filename,
                                   hdr.gpeditor_version_major,
                                   hdr.gpeditor_version_minor,
                                   ctime((long *) &hdr.time),
                                   game_exe_version
                              );
     }

     return ok;
}

BOOL
GpeRead(
     FILE *fp                           /* In  File to read from           */
) {
     static GP_NAMES                    tmp_names;
     static GP_EXE_CAR_COLOURS          tmp_car_colours[GP_EXE_NUM_TEAMS];
     static GP_EXE_CAR_POWER            tmp_car_power[GP_EXE_NUM_TEAMS];
     static GP_EXE_CAR_NUMBER           tmp_car_numbers[GP_EXE_NUM_DRIVERS];
     static GP_EXE_DRIVER_QUALIFYING    tmp_driver_qualifying[GP_EXE_NUM_DRIVERS];
     static GP_EXE_DRIVER_RACE          tmp_driver_race[GP_EXE_NUM_DRIVERS];
     static GP_EXE_HELMET_COLOURS       tmp_helmet_colours;
     static GPE_REC_AI_CAR_GRIP         tmp_ai_car_grip;
     static GPE_REC_PLAYERS_BHP         tmp_players_bhp;
     BOOL                               f_tmp_names              = FALSE;
     BOOL                               f_tmp_car_colours        = FALSE;
     BOOL                               f_tmp_car_power          = FALSE;
     BOOL                               f_tmp_car_numbers        = FALSE;
     BOOL                               f_tmp_driver_qualifying  = FALSE;
     BOOL                               f_tmp_driver_race        = FALSE;
     BOOL                               f_tmp_helmet_colours     = FALSE;
     BOOL                               f_tmp_ai_car_grip        = FALSE;
     BOOL                               f_tmp_players_bhp        = FALSE;
     BOOL                               ok   = TRUE;
     BOOL                               done = FALSE;

     while (ok && !done) {
          ok = fread(&rec, sizeof(rec), 1, fp);
          if (ok) {
               ok = (rec.magic == GPE_RECORD_MAGIC) ? TRUE : FALSE;
          }

          if (ok) {
               switch (rec.type) {
               case GPE_RT_EOF:
                    done = TRUE;
                    break;

               case GPE_RT_EXE_CAR_COLOURS:
                    ok = (rec.size == sizeof(tmp_car_colours)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(tmp_car_colours, sizeof(tmp_car_colours), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(tmp_car_colours, sizeof(tmp_car_colours))) ? TRUE : FALSE;
                    }
                    f_tmp_car_colours = TRUE;
                    break;

               case GPE_RT_EXE_HELMET_COLOURS:
                    ok = (rec.size == sizeof(tmp_helmet_colours)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(&tmp_helmet_colours, sizeof(tmp_helmet_colours), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(&tmp_helmet_colours, sizeof(tmp_helmet_colours))) ? TRUE : FALSE;
                    }
                    f_tmp_helmet_colours = TRUE;
                    break;

               case GPE_RT_EXE_CAR_POWER:
                    ok = (rec.size == sizeof(tmp_car_power)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(tmp_car_power, sizeof(tmp_car_power), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(tmp_car_power, sizeof(tmp_car_power))) ? TRUE : FALSE;
                    }
                    f_tmp_car_power = TRUE;
                    break;

               case GPE_RT_EXE_DRIVER_QUALIFYING:
                    ok = (rec.size == sizeof(tmp_driver_qualifying)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(tmp_driver_qualifying, sizeof(tmp_driver_qualifying), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(tmp_driver_qualifying, sizeof(tmp_driver_qualifying))) ? TRUE : FALSE;
                    }
                    f_tmp_driver_qualifying = TRUE;
                    break;

               case GPE_RT_EXE_DRIVER_RACE:
                    ok = (rec.size == sizeof(tmp_driver_race)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(tmp_driver_race, sizeof(tmp_driver_race), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(tmp_driver_race, sizeof(tmp_driver_race))) ? TRUE : FALSE;
                    }
                    f_tmp_driver_race = TRUE;
                    break;

               case GPE_RT_EXE_CAR_NUMBERS:
                    ok = (rec.size == sizeof(tmp_car_numbers)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(tmp_car_numbers, sizeof(tmp_car_numbers), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(tmp_car_numbers, sizeof(tmp_car_numbers))) ? TRUE : FALSE;
                    }
                    f_tmp_car_numbers = TRUE;
                    break;

               case GPE_RT_GAME_NAMES:
                    ok = (rec.size == sizeof(tmp_names)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(&tmp_names, sizeof(tmp_names), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(&tmp_names, sizeof(tmp_names))) ? TRUE : FALSE;
                    }
                    f_tmp_names = ok;
                    break;

               case GPE_RT_COMMENT:
                    if (ok) {
                         char *pszLocal = (char *) malloc(rec.size + 1);

                         ok = (pszLocal != NULL) ? TRUE : FALSE;
                         if (ok) {
                              ok = fread(pszLocal, rec.size, 1, fp);
                              pszLocal[rec.size] = '\0';

                              if (ok) {
                                   ok = (rec.crc == CalcCrc(pszLocal, rec.size)) ? TRUE : FALSE;
                              }

                              if (ok) {
                                   char *p = pszLocal;

                                   (void) printf("Comment:\n");
                                   while (*p != '\0') {
                                        if (*p != '\r') {
                                             (void) putchar(*p);
                                        }
                                        ++p;
                                   }
                                   (void) printf("\n\n");
                              }
                         }
                         else {
                              (void) printf("ImpGPE: Out of memory, unable to display comment.\n");
                         }

                         if (pszLocal) {
                              free(pszLocal);
                              pszLocal = NULL;
                         }
                    }
                    break;

               case GPE_RT_EXE_AI_CAR_GRIP:
                    ok = (rec.size == sizeof(tmp_ai_car_grip)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(&tmp_ai_car_grip, sizeof(tmp_ai_car_grip), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(&tmp_ai_car_grip, sizeof(tmp_ai_car_grip))) ? TRUE : FALSE;
                    }
                    f_tmp_ai_car_grip = ok;
                    break;

               case GPE_RT_EXE_PLAYERS_BHP:
                    ok = (rec.size == sizeof(tmp_players_bhp)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(&tmp_players_bhp, sizeof(tmp_players_bhp), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(&tmp_players_bhp, sizeof(tmp_players_bhp))) ? TRUE : FALSE;
                    }
                    f_tmp_players_bhp = ok;
                    break;

               default:
                    /*
                    ** Skip unknown record type.
                    */
                    (void) fseek(fp, (long) rec.size, SEEK_CUR);
                    break;
               }
          }
     }

     /*
     ** Only update all fields if we read the whole file OK.
     */
     if (ok) {
          if (f_tmp_names) {
               (void) memcpy(&GP_names, &tmp_names, sizeof(GP_names));
          }
          if (f_tmp_car_colours) {
               (void) memcpy(GP_EXE_car_colours, tmp_car_colours, sizeof(GP_EXE_car_colours));
          }
          if (f_tmp_car_power) {
               (void) memcpy(GP_EXE_car_power, tmp_car_power, sizeof(GP_EXE_car_power));
          }
          if (f_tmp_car_numbers) {
               (void) memcpy(GP_EXE_car_numbers, tmp_car_numbers, sizeof(GP_EXE_car_numbers));
          }
          if (f_tmp_driver_qualifying) {
               (void) memcpy(GP_EXE_driver_qualifying, tmp_driver_qualifying, sizeof(GP_EXE_driver_qualifying));
          }
          if (f_tmp_driver_race) {
               (void) memcpy(GP_EXE_driver_race, tmp_driver_race, sizeof(GP_EXE_driver_race));
          }
          if (f_tmp_helmet_colours) {
               (void) memcpy(&GP_EXE_helmet_colours, &tmp_helmet_colours, sizeof(GP_EXE_helmet_colours));
          }
          if (f_tmp_ai_car_grip && lGameExeAiCarGripFileOffset) {
               GP_EXE_AiCarGrip = tmp_ai_car_grip.grip;
          }
          if (f_tmp_players_bhp && lGameExePlayersCarPerfFileOffset) {
               GP_EXE_PlayersCarPerf = tmp_players_bhp.bhp;
          }
     }

     return ok;
}

static unsigned short
CalcCrc(
     void const     *ptr,               /* In  Data area                   */
     unsigned short len                 /* In  Length                      */
) {
     unsigned char const *p = (unsigned char const *) ptr;
     unsigned char       check_a = 0xff;
     unsigned char       check_b = 0xff;

     while (len--) {
          check_a += *p++;
          check_b += check_a;
     }

     return ((check_b << 8) | check_a);
}

BOOL
FileSaveNames(
     char *filename
) {
     FILE *fp;
     BOOL ok = FALSE;

     if ((fp = fopen(filename, "wb")) != NULL) {
          CalculateNamesCrc(&GP_names);
          ok = fwrite(&GP_names, sizeof(GP_names), 1, fp);

          if (!ok) {
               (void) printf("ImpGPE: Failed to write to '%s'\n", filename);
          }

          (void) fclose(fp);
     }
     else {
          ok = FALSE;
          (void) printf("ImpGPE: Unable to open '%s'\n", filename);
     }

     return ok;
}

static void
CalculateNamesCrc(
     GP_NAMES       *gp_names           /* I/O  Names to calculate CRC for */
) {
     unsigned short i;
     unsigned short crc1 = 0;
     unsigned short crc2 = 0;
     unsigned char  *p = (unsigned char *) gp_names;

     for (i = 0; i < sizeof(*gp_names) - (sizeof(gp_names->crc1) + sizeof(gp_names->crc2)); i++) {
          crc1 += *p;
          rotate_left3(&crc2);
          crc2 += *p;
          ++p;
     }

     gp_names->crc1 = crc1;
     gp_names->crc2 = crc2;
}

static void
rotate_left3(
     unsigned short *ptr_us
) {
     unsigned long us = *ptr_us;

     us <<= 3;

     us |= (us >> 16) & 0x00000007U;
     *ptr_us = (unsigned short) us;
}

/*---------------------------------------------------------------------------
** End of File
*/

