/*
** File   : f1grid.c
** Author : TK
** Date   : 18/01/95
**
** $Header$
**
** 
*/

#define VERSION     "V1.0 18/01/95"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
/*lint +elib(???) */

/*lint -e793 */
#include "savinf.h"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE                  1
#define FALSE                 0

#define NUM_ELEMENTS(x)       ((int) (sizeof(x) / sizeof(x[0])))

#define CRC_SIZE              (sizeof(short) * 2)
#define WHAT_OFFSET           4

#define DEFAULT_SAV_FILENAME  "F1GRID.SAV"
#define DEFAULT_TXT_FILENAME  "F1GRID.TXT"

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef int    bool;

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
                             "F1Grid " VERSION " - Modify F1GP grid positions.\n"
                             "Copyright (c) T.Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.\n\n";

static SAVED_GAME   sg;
static char         buff[512];

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE                *fp;
     unsigned short      i;
     unsigned short      j;
     unsigned short      n;
     char *sav_filename  = DEFAULT_SAV_FILENAME;
     char *txt_filename  = DEFAULT_TXT_FILENAME;
     char option;
     bool expect_sav     = TRUE;
     bool expect_txt     = FALSE;
     bool u_flag         = FALSE;
     bool                duplicate;
     short               pos;
     short               car;
     short               nums[40];
     short               line = 0;

     (void) printf(&title_msg[WHAT_OFFSET]);

     for (--argc, ++argv; argc; argc--, argv++) {
          if (**argv == '-' || **argv == '/') {
               while (*++*argv) {
                    option = (char) ((isupper(**argv)) ? tolower(**argv) : **argv);
                    switch (option) {
                    case 'h':
                    case '?':
                         Usage();
                         break;

                    case 'u':
                         u_flag = TRUE;
                         break;

                    default:
                         (void) printf("f1grid: unknown option '-%c'.\n", **argv);
                         Usage();
                         break;
                    }
               }
          }
          else {
               if (expect_sav) {
                    sav_filename   = *argv;
                    expect_sav     = FALSE;
                    expect_txt     = TRUE;
               }
               else if (expect_txt) {
                    txt_filename   = *argv;
                    expect_txt     = FALSE;
               }
               else {
                    Usage();
               }
          }
     }

     /*lint -e506 -e792 */
     assert(sizeof(PER_CAR) == 0xc0);
     assert(sizeof(SAVED_GAME) == SAVED_GAME_SIZE);
     /*lint +e506 +e792 */

     if ((fp = fopen(sav_filename, "rb")) == NULL) {
          (void) printf("f1grid: Unable to open file '%s'\n", sav_filename);
          return 1;
     }

     if (fread(&sg, sizeof(sg), 1, fp) != 1) {
          (void) printf("f1grid: Unable to read enough data from file '%s'\n", sav_filename);
          return 1;
     }
     (void) fclose(fp);
       
     if (!CheckCrc(&sg, sizeof(sg), sg.crc1, sg.crc2)) {
          (void) printf("f1grid: '%s' doesn't appear to be a valid saved game file.\n", sav_filename);
          return 1;
     }

     if (sg.session_type != ST_QUALIFYING) {
          (void) printf("f1grid: You can only edit the grid for saved qualifying sessions.\n.");
          return 1;
     }

     if (!u_flag) {
          if ((fp = fopen(txt_filename, "w")) == NULL) {
               (void) printf("f1grid: Unable to open file '%s'\n", txt_filename);
               return 1;
          }

          for (i = 0, j = 1; i < NUM_ELEMENTS(sg.fastest_lap_position); i++) {
               n = (sg.fastest_lap_position[i] & 0x3f);
               if (n != 0) {
                    (void) fprintf(fp, "%2u)  #%02u  %s\n",
                                        j++,
                                        n,
                                        sg.driver_names[n - 1]
                                   );
               }
          }
          (void) fclose(fp);

          (void) printf("Extracted car numbers & drivers names from '%s' and\n", sav_filename);
          (void) printf("written them to '%s'.\n", txt_filename);
          (void) printf("Now edit the first number in this file and rerun F1GRID with the '-u' option.\n");
     }
     else {
          for (i = 0; i < NUM_ELEMENTS(nums); i++) {
               nums[i] = -1;
          }

          if ((fp = fopen(txt_filename, "r")) == NULL) {
               (void) printf("f1grid: Unable to open file '%s'\n", txt_filename);
               return 1;
          }

          while (fgets(buff, sizeof(buff), fp) != NULL) {
               ++line;
               if (sscanf(buff, "%2d)  #%2d  ", &pos, &car) == 2) {
                    if (pos >= 1 && pos <= NUM_ELEMENTS(nums)) {
                         --pos;
                         if (nums[pos] == -1) {
                              if (car >= 1 && car <= 40) {
                                   duplicate = FALSE;
                                   for (j = 0; j < NUM_ELEMENTS(nums); j++) {
                                        if (nums[pos] == car) {
                                             duplicate = TRUE;
                                             (void) printf("f1grid:L%03u: Car number of '%d' is already present.\n", line, car);
                                             return 1;
                                        }
                                   }

                                   if (!duplicate) {
                                        nums[pos] = car;
                                   }
                              }
                              else {
                                   (void) printf("f1grid:L%03u: Car number of '%d' is invalid.\n", line, car);
                                   return 1;
                              }
                         }
                         else {
                              (void) printf("f1grid:L%03u: Grid position of '%d' is already filled by car #%d.\n", line, pos + 1, nums[pos]);
                              return 1;
                         }
                    }
                    else {
                         (void) printf("f1grid:L%03u: Grid position of '%d' is invalid.\n", line, pos);
                         return 1;
                    }
               }
          }

          /*
          ** Setup qualifying session so it expires immediately.
          */
          sg.current_session_time_ms = (sg.qualifying_period_conf * 60U);
          sg.current_session_time_ms *= 1000U;

          for (i = 0; i < NUM_ELEMENTS(sg.fastest_lap_time); i++) {
               sg.fastest_lap_time[i]        = 599999U;
               sg.qualifying_grid_time[i]    = 599999U;
          }

          for (i = 0; i < NUM_ELEMENTS(sg.fastest_lap_time); i++) {
               if (sg.car_numbers[i] != 0) {
                    for (j = 0; j < NUM_ELEMENTS(nums); j++) {
                         n = (sg.car_numbers[i] & 0x3f);
                         if (n == nums[j]) {
                              sg.fastest_lap_time[n - 1]     = j + 1;
                              sg.qualifying_grid_time[n - 1] = j + 1;
                              break;
                         }
                    }
               }
          }

          for (i = 0, j = 1; i < NUM_ELEMENTS(nums); i++) {
               if (nums[i] > 0) {
                    n = nums[i];
                    (void) printf("%2u)  #%02u  %-24s\n",
                                        j++,
                                        n,
                                        sg.driver_names[n - 1]
                                   );
               }
          }

          /*
          ** Recalculate CRC and write new saved game.
          */
          CalcCrc(&sg, sizeof(sg), &sg.crc1, &sg.crc2);
          if ((fp = fopen(sav_filename, "wb")) == NULL) {
               (void) printf("f1grid: Unable to write to saved game file called '%s'\n", sav_filename);
               return 1;
          }

          if (fwrite(&sg, sizeof(sg), 1, fp) != 1) {
               (void) printf("f1grid: Unable to write to saved game file called '%s'\n", sav_filename);
               return 1;
          }

          (void) fclose(fp);
     }

     return 0;
}

static void
Usage(
     void
) {
     (void) printf( "Usage: f1grid [-h?] [-u] [Saved Game] [Text file]\n"
                    "       -?/-h            This usage message.\n"
                    "       -u               Update saved game from text file.\n"
                    "       [Saved Game]     Specify another name for F1GRID.SAV.\n"
                    "       [Text File]      Specify another name for F1GRID.TXT.\n"
                    "\n"
                    "To modify a grid first save an F1GP qualifying session to a file. Then to\n"
                    "obtain the list of car numbers & drivers in .TXT file format use:\n"
                    "    F1GRID DEMO.SAV DEMO.TXT\n"
                    "(you can type just 'F1GRID' on its own and it will use the default names).\n"
                    "\n"
                    "Now using any ASCII editor (e.g., Windows NOTEPAD) edit the .TXT file and\n"
                    "change the first number to indicate the drivers starting position. Now just\n"
                    "update the .SAV saved game file with the -u option:\n"
                    "    F1GRID -u DEMO.SAV DEMO.TXT\n"
                    "\n"
                    "(you can type just 'F1GRID -u' on its own and it will use the default names).\n"
                    "\n"
                    "Now in F1GP reload the game, return to qualifying and unpause the game. The\n"
                    "qualifying session will terminate and you can race with your preset grid.\n"
               );
     exit(1);
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