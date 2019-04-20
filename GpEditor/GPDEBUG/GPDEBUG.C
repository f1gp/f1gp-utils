/*
** File   : gpdebug.c
** Author : TK 
** Date   : 24/01/94
**
** $Header:   L:/GPEDITOR/GPDEBUG/VCS/GPDEBUG.C__   1.0   26 Jan 1994 21:30:56   tk  $
**
** Change History:
** 
** V1.0 24/01/94    First cut.
*/

#define VERSION     "V1.0 24/01/94"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE                       1
#define FALSE                      0

#define NUM_ELEMENTS(x)            (sizeof(x) / sizeof(x[0]))

#define OUTPUT_FILENAME            "GPDEBUG.OP"

#define VERSION_STRING_LENGTH      12
#define GP_EXE_MAX_TEAMS           18

#define DUMP_START_OFFSET          (-0x00000100L)
#define DUMP_END_OFFSET            (+0x00000600L)

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef short  bool;

typedef struct {
     char           pling;
     unsigned char  colours[15];
} GP_EXE_CAR_COLOURS;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static bool SearchForVersion(FILE *ip, char *game_version);
static bool SearchForColours(FILE *ip, long *colour_offset);
static bool IsGameExeColoursPresent(void);

/*---------------------------------------------------------------------------
** Data
*/

/*lint -esym(528,sccsid) */

static const char sccsid[] = "@(#)" "gpdebug.exe" " " VERSION " "
     "(c) Copyright 1994 T.Kellaway - All rights reserved.";

static GP_EXE_CAR_COLOURS          GP_EXE_car_colours[GP_EXE_MAX_TEAMS];

/*---------------------------------------------------------------------------
** Functions
*/

short
main(
     short     argc,
     char      *argv[]
) {
     FILE           *ip;
     FILE           *op;
     struct stat    sb;
     char           game_version[VERSION_STRING_LENGTH + 1];
     char           *gamename;
     long           t;
     long           colour_offset;
     long           pos;
     short          n;
     bool           found;

     (void) fprintf(stderr, "GP Editor - Debug Tool (%s) T.Kellaway CIS:100331,2330\n\n", VERSION);

     if (argc != 2) {
          (void) fprintf(stderr, "Usage: gpdebug (GP.EXE or equivalent)\n");
          return 1;
     }

     gamename = argv[1];
     if ((ip = fopen(gamename, "rb")) == NULL) {
          (void) fprintf(stderr, "gpdebug: unable to open file '%s'.\n", gamename);
          return 1;
     }

     if (stat(gamename, &sb) != 0) {
          (void) fprintf(stderr, "gpdebug: unable to get file info for '%s'.\n", gamename);
          return 1;
     }

     if ((op = fopen(OUTPUT_FILENAME, "w")) == NULL) {
          (void) fprintf(stderr, "gpdebug: unable to open output file '%s'.\n", OUTPUT_FILENAME);
          return 1;
     }

     (void) printf("Analysing '%s', please wait...\n", gamename);

     (void) time(&t);
     (void) fprintf(op, "GP Debug - %s\n\n", VERSION);
     (void) fprintf(op, "GPDEBUG run at: %s\n", ctime(&t));
     (void) fprintf(op, "Game Filename:  '%s'\n", gamename);
     (void) fprintf(op, "Game Size:      %lu bytes\n", sb.st_size);
     (void) fprintf(op, "Game Date:      %lu - %s", sb.st_mtime, ctime(&sb.st_mtime));

     found = SearchForVersion(ip, game_version);
     (void) fprintf(op, "Game Version:   %s\n", (found) ? game_version : "Unknown!");

     if (SearchForColours(ip, &colour_offset)) {
          (void) fprintf(op, "Colour Offset:  %06lxH\n", colour_offset);
     }
     else {
          (void) fprintf(stderr, "\ngpdebug: Sorry, this doesn't appear to be a game EXE!\n");
          return 1;
     }

     (void) fprintf(op, "Game Data:\n");
     (void) fseek(ip, colour_offset + DUMP_START_OFFSET, SEEK_SET);
     for (n = 1, pos = ftell(ip); pos < colour_offset + DUMP_END_OFFSET; ++pos) {
          unsigned char  c;

          (void) fread(&c, sizeof(c), 1, ip);
          (void) fprintf(op, "%02x", c);
          if (n++ >= 64 / 2) {
               n = 1;
               (void) fprintf(op, "\n");
          }
     }

     (void) fclose(ip);
     (void) fclose(op);

     (void) printf("\nFinished\n\n");
     (void) printf("Please send the '%s' file to Trevor Kellaway at CIS:100331,2330.\n", OUTPUT_FILENAME);
     (void) printf("Note that this file only contains 7-bit ASCII.\n");
     (void) printf("Please ZIP it up if you are performing a Binary send.\n\n");
     (void) printf("Thanks for your assistance.\n\n");
     return 0;
}

static bool
SearchForVersion(
     FILE      *ip,                     /* In   Game EXE                   */
     char      *game_version            /* I/O  Found version string       */
) {
     bool      found = FALSE;
     char      c;
     long      pos;
     short     count = 0;

     if (fseek(ip, 0L, SEEK_SET) == 0) {
          for (found = FALSE; !found;) {
               if (++count > 1024) {
                    count = 0;
                    (void) printf("*");
               }

               if (fread(&c, sizeof(c), 1, ip) == 1) {
                    if (c == 'V') {
                         pos = ftell(ip);
                         game_version[0] = c;
                         if (fread(&game_version[1], VERSION_STRING_LENGTH - 1, 1, ip) == 1) {
                              if (strncmp(game_version, "Version 1", 9) == 0) {
                                   game_version[VERSION_STRING_LENGTH] = '\0';
                                   found = TRUE;
                              }
                              else {
                                   if (fseek(ip, pos, SEEK_SET) != 0) {
                                        break;
                                   }
                              }
                         }
                         else {
                              break;
                         }
                    }
               }
               else {
                    break;
               }
          }
     }

     return found;
}

static bool
SearchForColours(
     FILE           *ip,                /* In   Game EXE                   */
     long           *colour_offset      /* Out  Colour table position      */
) {
     bool           ok;
     unsigned char  c;
     short          count = 0;

     /*
     ** This will be *slow*. It could be speeded up!
     */
     if (fseek(ip, 0L, SEEK_SET) != 0) {
          return FALSE;
     }

     for (ok = TRUE; ok;) {
          if (++count > 1024) {
               count = 0;
               (void) printf("*");
          }

          ok = (bool) fread(&c, sizeof(c), 1, ip);

          if (ok && c == '!') {
               long pos = ftell(ip) - 1L;

               if (fseek(ip, pos, SEEK_SET) != 0) {
                    return FALSE;
               }
               ok = (bool) fread(GP_EXE_car_colours, sizeof(GP_EXE_car_colours), 1, ip);
               if (ok) {
                    if (IsGameExeColoursPresent()) {
                         *colour_offset = pos;
                         return TRUE;
                    }
               }
               if (fseek(ip, pos + 1L, SEEK_SET) != 0) {
                    return FALSE;
               }
          }
     }

     return FALSE;
}

static bool
IsGameExeColoursPresent(
     void
) {
     bool           ok = TRUE;
     unsigned short i;

     for (i = 0; ok && i < NUM_ELEMENTS(GP_EXE_car_colours); i++) {
          if (GP_EXE_car_colours[i].pling != '!') {
               ok = FALSE;
          }
     }

     return ok;
}

/*---------------------------------------------------------------------------
** End of File
*/

