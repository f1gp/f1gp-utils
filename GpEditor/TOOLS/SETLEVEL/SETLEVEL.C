/*
** File   : setlevel.c
** Author : TK
** Date   : 30/07/94
**
** $Header:   L:/GPEDITOR/TOOLS/SETLEVEL/VCS/SETLEVEL.C__   1.0   18 Jun 1995 21:59:12   tk  $
**
** A program to modify the level in a saved game file.
**
** V1.0 14-Jun-95    First bash
*/

#define VERSION     "V1.0 14-Jun-95"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define WHAT_OFFSET                4

#define LEVEL_OFFSET               0x0006L
#define SAVED_GAME_SIZE            26736

#define CRC_SIZE                   (sizeof(short) * 2)

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct {
     unsigned char  data[SAVED_GAME_SIZE - CRC_SIZE];
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
                             "SetLevel " VERSION " - Set Grand Prix level for a saved game.\n"
                             "Copyright (c) T.Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.\n\n";

static SAVED_GAME   game;

static const char *level_txt[] = {
     "Rookie",
     "Amateur",
     "Semi-Pro",
     "Pro",
     "Ace"
};

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE *fp;
     char *game_file;
     int  level     = 0;
     int  old_level = 0;

     (void) printf(&title_msg[WHAT_OFFSET]);

     /*lint -e506 Ignore constat boolean */
     assert(sizeof(SAVED_GAME) == SAVED_GAME_SIZE);
     /*lint +e506 Ignore constat boolean */

     if (argc != 3) {
          (void) printf("Usage: SetLevel (saved game) (level)\n");
          (void) printf("          1 - Rookie\n");
          (void) printf("          2 - Amateur\n");
          (void) printf("          3 - Semi-Pro\n");
          (void) printf("          4 - Pro\n");
          (void) printf("          5 - Ace\n");
          return -1;
     }

     game_file = argv[1];
     level = atoi(argv[2]);

     if (level < 1 || level > 5) {
          (void) printf("SetLevel: The level value should be between 1 and 5.\n");
          return -1;
     }
     --level;

     if ((fp = fopen(game_file, "rb+")) == NULL) {
          (void) printf("SetLevel: Unable to open saved game file called '%s'\n", game_file);
          return -1;
     }

     if (fread(&game, sizeof(game), 1, fp) != 1) {
          (void) printf("SetLevel: Unable to read from saved game file called '%s'\n", game_file);
          return -1;
     }

     if (!CheckCrc(&game, sizeof(game), game.crc1, game.crc2)) {
          (void) printf("SetLevel: '%s' doesn't appear to be a valid saved game file.\n", game_file);
          return -1;
     }

     /*
     ** Update level and recalculate CRC.
     */
     old_level = game.data[LEVEL_OFFSET];
     game.data[LEVEL_OFFSET] = (unsigned char) level;
     CalcCrc(&game, sizeof(game), &game.crc1, &game.crc2);

     (void) fseek(fp, 0L, SEEK_SET);
     if (fwrite(&game, sizeof(game), 1, fp) != 1) {
          (void) printf("SetLevel: Unable to write to saved game file called '%s'\n", game_file);
          return -1;
     }

     (void) printf("Updated saved game '%s' from '%s' to '%s'\n", game_file, level_txt[old_level], level_txt[level]);

     (void) fclose(fp);

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