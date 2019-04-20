/*
** File   : setnames.c
** Author : TK
** Date   : 30/07/94
**
** $Header:   L:/GPEDITOR/SETNAMES/VCS/SETNAMES.C__   1.0   30 Jul 1994 20:39:50   tk  $
**
** A program to a import MPS names file into a saved game file.
**
** V1.0 30/07/94    First bash
*/

#define VERSION     "V1.0 30/Jul/94"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <string.h>
#include <assert.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define WHAT_OFFSET                4

#define GP_EXE_NUM_TEAMS           20
#define GP_EXE_NUM_DRIVERS         (GP_EXE_NUM_TEAMS * 2)

#define SAVED_GAME_NAMES_OFFSET    0x082eL
#define SAVED_GAME_SIZE            26736

#define CRC_SIZE                   (sizeof(short) * 2)

/*---------------------------------------------------------------------------
** Typedefs
*/

/*lint -e754 Ignore structure members not accessed */

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
     char           dummy1[SAVED_GAME_NAMES_OFFSET];
     GP_NAMES       names;
     char           dummy2[SAVED_GAME_SIZE - (SAVED_GAME_NAMES_OFFSET + sizeof(GP_NAMES) + CRC_SIZE)];
     unsigned short crc1;
     unsigned short crc2;
} SAVED_GAME;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static int CheckCrc(void const *data, unsigned short len, unsigned short c1,
                    unsigned short c2);
static void CalcCrc(void const *data, unsigned short len, unsigned short *c1,
                    unsigned short *c2);
static void rotate_left(unsigned short *ptr_us);

/*---------------------------------------------------------------------------
** Data
*/

static char    title_msg[] = "@(#)"
                             "SetNames " VERSION " - Set Grand Prix names for a saved game.\n"
                             "Copyright (c) T.Kellaway (CIS:100331,2330) 1994 - All Rights Reserved.\n\n";

static GP_NAMES     names;
static SAVED_GAME   game;

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE *ifp;
     FILE *ofp;
     char *name_file;
     char *game_file;

     (void) printf(&title_msg[WHAT_OFFSET]);

     /*lint -e506 Ignore constat boolean */
     assert(sizeof(SAVED_GAME) == SAVED_GAME_SIZE);
     /*lint +e506 Ignore constat boolean */

     if (argc != 3) {
          (void) printf("Usage: SetNames (name file) (saved game)\n");
          return -1;
     }

     name_file = argv[1];
     game_file = argv[2];

     if ((ifp = fopen(name_file, "rb")) == NULL) {
          (void) printf("SetNames: Unable to open names file called '%s'\n", name_file);
          return -1;
     }

     if ((ofp = fopen(game_file, "rb+")) == NULL) {
          (void) printf("SetNames: Unable to open saved game file called '%s'\n", game_file);
          return -1;
     }

     if (fread(&names, sizeof(names), 1, ifp) != 1) {
          (void) printf("SetNames: Unable to read from names file called '%s'\n", name_file);
          return -1;
     }

     if (!CheckCrc(&names, sizeof(names), names.crc1, names.crc2)) {
          (void) printf("SetNames: '%s' doesn't appear to be a valid names file.\n", name_file);
          return -1;
     }

     if (fread(&game, sizeof(game), 1, ofp) != 1) {
          (void) printf("SetNames: Unable to read from saved game file called '%s'\n", game_file);
          return -1;
     }

     if (!CheckCrc(&game, sizeof(game), game.crc1, game.crc2)) {
          (void) printf("SetNames: '%s' doesn't appear to be a valid saved game file.\n", name_file);
          return -1;
     }

     /*
     ** Copy names and recalculate CRC.
     */
     (void) memcpy(&game.names, &names, sizeof(game.names) - CRC_SIZE);
     CalcCrc(&game, sizeof(game), &game.crc1, &game.crc2);

     (void) fseek(ofp, 0L, SEEK_SET);
     if (fwrite(&game, sizeof(game), 1, ofp) != 1) {
          (void) printf("SetNames: Unable to write to saved game file called '%s'\n", name_file);
          return -1;
     }

     (void) printf("Updated saved game '%s' with names from '%s'\n", game_file, name_file);

     (void) fclose(ifp);
     (void) fclose(ofp);

     return 0;
}

static int
CheckCrc(
     void const     *data,
     unsigned short len,
     unsigned short c1,
     unsigned short c2
) {
     unsigned short      crc1 = 0;
     unsigned short      crc2 = 0;

     CalcCrc(data, len, &crc1, &crc2);
     return ((c1 == crc1) && (c2 == crc2));
}

static void
CalcCrc(
     void const     *data,
     unsigned short len,
     unsigned short *c1,
     unsigned short *c2
) {
     unsigned char const *p   = data;
     unsigned short      crc1 = 0;
     unsigned short      crc2 = 0;
     unsigned short      i;

     for (i = 0; i < len - CRC_SIZE; i++) {
          crc1 += p[i];
          rotate_left(&crc2);
          crc2 += p[i];
     }

     *c1 = crc1;
     *c2 = crc2;
}

static void
rotate_left(
     unsigned short *ptr_us             /* In   Pointer to value to shift  */
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