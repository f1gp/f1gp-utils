/*
** File   : gpfps.c
** Author : TK
** Date   : 30/07/94
**
** $Header:   L:/GPEDITOR/TOOLS/GPFPS/VCS/GPFPS.C__   1.0   09 Nov 1994 23:08:24   tk  $
**
** A program to a modify the FPS in a MPS Grand Prix saved game file.
**
** V1.0 09/11/94    First bash
*/

#define VERSION     "V1.0 09/Nov/94"

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

#define SAVED_GAME_FPS_OFFSET      0x0014L
#define SAVED_GAME_SIZE            26736

#define CRC_SIZE                   (sizeof(short) * 2)

#define NUM_ELEMENTS(x)            (sizeof(x) / sizeof(x[0]))

/*---------------------------------------------------------------------------
** Typedefs
*/

/*lint -e754 Ignore structure members not accessed */

typedef struct {
     char           dummy1[SAVED_GAME_FPS_OFFSET];
     unsigned char  fps;
     char           dummy2[SAVED_GAME_SIZE - (SAVED_GAME_FPS_OFFSET + sizeof(unsigned char) + CRC_SIZE)];
     unsigned short crc1;
     unsigned short crc2;
} SAVED_GAME;

typedef struct {
     float          fps;
     unsigned char  game_value;
} FPS;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void Usage(void);
static int  CheckCrc(void const *data, unsigned short len, unsigned short c1,
                    unsigned short c2);
static void CalcCrc(void const *data, unsigned short len, unsigned short *c1,
                    unsigned short *c2);
static void rotate_left(unsigned short *ptr_us);

/*---------------------------------------------------------------------------
** Data
*/

static char    title_msg[] = "@(#)"
                             "GpFPS " VERSION " - Set Grand Prix FPS for a saved game.\n"
                             "Copyright (c) T.Kellaway (CIS:100331,2330) 1994 - All Rights Reserved.\n\n";

static const FPS fps_values[] = {
     {    25.0,     0x0c      },
     {    23.0,     0x0d      },
     {    21.4,     0x0e      },
     {    20.0,     0x0f      },
     {    18.7,     0x10      },
     {    17.6,     0x11      },
     {    16.6,     0x12      },
     {    15.7,     0x13      },
     {    15.0,     0x14      },
     {    14.2,     0x15      },
     {    13.6,     0x16      },
     {    13.0,     0x17      },
     {    12.5,     0x18      },
     {    12.0,     0x19      },
     {    11.5,     0x1a      },
     {    11.1,     0x1b      },
     {    10.7,     0x1c      },
     {    10.3,     0x1d      },
     {    10.0,     0x1e      },
     {     9.6,     0x1f      },
     {     9.3,     0x20      },
     {     9.0,     0x21      },
     {     8.8,     0x22      },
     {     8.5,     0x23      },
     {     8.3,     0x24      },
     {     8.1,     0x25      },
};

static SAVED_GAME   game;

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE           *fp;
     char           *game_file;
     float          new_fps        = 0.0;
     unsigned char  game_value;
     unsigned int   i;

     (void) printf(&title_msg[WHAT_OFFSET]);

     /*lint -e506 Ignore constat boolean */
     assert(sizeof(SAVED_GAME) == SAVED_GAME_SIZE);
     /*lint +e506 Ignore constat boolean */

     if (argc < 2 || argc > 3) {
          Usage();
          return 1;
     }

     game_file = argv[1];
     if (argc == 3) {
          new_fps = (float) atof(argv[2]);
     }

     if ((fp = fopen(game_file, "rb+")) == NULL) {
          (void) printf("GpFPS: Unable to open saved game file called '%s'\n", game_file);
          return 1;
     }

     if (fread(&game, sizeof(game), 1, fp) != 1) {
          (void) printf("GpFPS: Unable to read from saved game file called '%s'\n", game_file);
          return 1;
     }

     if (!CheckCrc(&game, sizeof(game), game.crc1, game.crc2)) {
          (void) printf("GpFPS: '%s' doesn't appear to be a valid saved game file.\n", game_file);
          return 1;
     }

     /*
     ** Display current value.
     */
     (void) printf("'%s' is currently set to %2.1f FPS.\n", game_file, fps_values[game.fps - fps_values[0].game_value].fps);

     if (argc == 3) {
          /*
          ** Try and find user specified FPS.
          */
          game_value = 0;
          for (i = 0; i < NUM_ELEMENTS(fps_values); i++) {
               if (new_fps == fps_values[i].fps) {
                    game_value = fps_values[i].game_value;
                    break;
               }
               else if (new_fps > fps_values[i].fps) {
                    (void) printf("\nUsing nearest lower value to %2.1f of %2.1f FPS.\n", new_fps, fps_values[i].fps);
                    game_value = fps_values[i].game_value;
                    break;
               }
          }

          if (game_value == 0) {
               (void) printf("\nUsing nearest lower value to %2.1f of %2.1f FPS.\n", new_fps, fps_values[NUM_ELEMENTS(fps_values) - 1].fps);
               game_value = fps_values[NUM_ELEMENTS(fps_values) - 1].game_value;
          }

          /*
          ** Update FPS and recalculate CRC.
          */
          game.fps = game_value;
          CalcCrc(&game, sizeof(game), &game.crc1, &game.crc2);

          (void) fseek(fp, 0L, SEEK_SET);
          if (fwrite(&game, sizeof(game), 1, fp) != 1) {
               (void) printf("GpFPS: Unable to write to saved game file called '%s'\n", game_file);
               return 1;
          }

          (void) printf("\nUpdated saved game '%s' with new FPS of %2.1f\n", game_file, fps_values[game.fps - fps_values[0].game_value].fps);
     }

     (void) fclose(fp);

     return 0;
}

static void
Usage(
     void
) {
     (void) printf("Usage: GpFPS (saved game) [FPS]\n");
     (void) printf("       If no FPS is specified the current value is displayed.\n");
     (void) printf("\nAvailable FPS:\n");
     (void) printf("25.0, 23.0, 21.4, 20.0, 18.7, 17.6, 16.6, 15.7, 15.0, 14.2, 13.6, 13.0, 12.5\n");
     (void) printf("12.0, 11.5, 11.1, 10.7, 10.3, 10.0,  9.6,  9.3,  9.0,  8.8,  8.5,  8.3,  8.1\n");
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