/*
** File   : gplap.c
** Author : TK 
** Date   : 27/03/94
**
** $Header:   F:/TK/GPLAPTIM/GPLAP/VCS/GPLAP.C__   1.13   07 Apr 1996 12:40:56   tk  $
**
** Lap Time Logger for MPS Grand Prix.
**
** V1.0 27/03/94    Initial version.
** V2.0 23/04/94    Added CR/LF before printing title.
** V2.1 14/05/94    Command line options may now be uppercase.
**                  '/' may be used instead of '-' for options.
** V3.0 25/07/94    Added car setup entry.
**                  Removed Best/Fastest from defaults.
**                  Added elapsed race time.
**                  FP & PRP now display position.
**                  Fixed -w watch bug (would always perform a rewind).
** V3.1 13/08/94    Rebuilt with more garbage defensive version of LAPUTL.LIB
** V4.0 30/09/94    Text for setup record modified slightly.
** V5.0 05/03/95    Added support for Split Time.
** V5.1 02/07/95    Rebuilt with V4.02 Borland C++.
** V6.0 07/10/95    Added support for race authentication records.
** V6.1 04/04/96    Updated version number to keep in step with GPLAPW.
*/

#define VERSION     "V6.1 4th Apr 1996"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <share.h>
#include <io.h>
#include <conio.h>
#include <time.h>
/*lint +elib(???) */
#include <laputl.h>
#include "vdm.h"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE                  1
#define FALSE                 0

#define DEFAULT_LOG_FILENAME  "GPLAPTIM.LOG"

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void Usage(void);
static void ShowLog(char *filename, bool watch_mode);
static void WriteAuthEntries(char *filename);
static bool IsFilter(LB_ENTRY *le);

/*---------------------------------------------------------------------------
** Data
*/

/*lint -esym(528,sccsid) */
static const char sccsid[] = "@(#)GpLap.exe" "\t" VERSION;

static bool fShowTotalLaps    = FALSE;       // -n
static bool x_flag            = FALSE;
static bool w_flag            = FALSE;
static bool fShowPractice     = FALSE;       // -t
static bool fShowQualifying   = FALSE;       // -q
static bool fShowRace         = FALSE;       // -r
static bool fShowComputer     = FALSE;       // -c
static bool fShowLeader       = FALSE;       // -l
static bool fShowPlayer       = FALSE;       // -p
static bool fShowAuth         = FALSE;       // -a
static bool fShowFastest      = FALSE;       // -f
static bool fShowBest         = FALSE;       // -b
static bool fShowAvgSpeed     = FALSE;       // -s
static bool fAvgSpeedMph      = FALSE;       // -m / -k
static bool fElapsed          = FALSE;       // -e
static bool fShowSetup        = FALSE;       // -g
static bool fShowPit          = FALSE;       // -i

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     char *filename = DEFAULT_LOG_FILENAME;
     char option;

     for (--argc, ++argv; argc; argc--, argv++) {
          if (**argv == '-' || **argv == '/') {
               while (*++*argv) {
                    option = (isupper(**argv)) ? tolower(**argv) : **argv;
                    switch (option) {
                    case 'n': fShowTotalLaps      = TRUE;   break;
                    case 'x': x_flag              = TRUE;   break;
                    case 'w': w_flag              = TRUE;   break;
                    case 't': fShowPractice       = TRUE;   break;
                    case 'q': fShowQualifying     = TRUE;   break;
                    case 'r': fShowRace           = TRUE;   break;
                    case 'c': fShowComputer       = TRUE;   break;
                    case 'l': fShowLeader         = TRUE;   break;
                    case 'p': fShowPlayer         = TRUE;   break;
                    case 'a': fShowAuth           = TRUE;   break;
                    case 'f': fShowFastest        = TRUE;   break;
                    case 'b': fShowBest           = TRUE;   break;
                    case 's': fShowAvgSpeed       = TRUE;   break;
                    case 'm': fAvgSpeedMph        = TRUE;   break;
                    case 'k': fAvgSpeedMph        = FALSE;  break;
                    case 'e': fElapsed            = TRUE;   break;
                    case 'g': fShowSetup          = TRUE;   break;
                    case 'i': fShowPit            = TRUE;   break;

                    case 'h':
                    case '?':
                         Usage();
                         break;

                    default:
                         (void) printf("gplap: unknown option '-%c'.\n", **argv);
                         Usage();
                         break;
                    }
               }
          }
          else {
               filename = *argv;
          }
     }

     if ( !fShowPractice && !fShowQualifying && !fShowRace   &&
          !fShowComputer && !fShowLeader     && !fShowPlayer &&
          !fShowAuth     && !fShowFastest    && !fShowBest   &&
          !fShowAvgSpeed && !fShowTotalLaps  && !fShowSetup  &&
          !fShowPit
     ) {
          fShowPractice       = TRUE;
          fShowQualifying     = TRUE;
          fShowRace           = TRUE;
          fShowComputer       = TRUE;
          fShowLeader         = TRUE;
          fShowPlayer         = TRUE;
          fShowAuth           = TRUE;
          fShowSetup          = TRUE;
          fShowPit            = TRUE;
     }

     if (x_flag) {
          WriteAuthEntries(filename);
     }
     else {
          ShowLog(filename, w_flag);
     }

     return 0;
}

static void
Usage(
     void
) {
     (void) printf("GpLap " VERSION " - Grand Prix/World Circuit Lap Time Logger.\n");
     (void) printf("Copyright (c) Trevor Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.\n\n");

     (void) printf( "Usage: gplap [-tqrcplfbasmkeginxw] [log filename]\n"
                    "       -?/-h  This usage message.\n"
                    "       -t     Show Practice (Training).\n"
                    "       -q     Show Qualifying.\n"
                    "       -r     Show Race.\n"
                    "       -c     Show Computer.\n"
                    "       -p     Show Player.\n"
                    "       -l     Show Leader.\n"
                    "       -f     Show Fastest Laps.\n"
                    "       -b     Show Best Laps.\n"
                    "       -a     Show Authentication.\n"
                    "       -s     Show Average Speed.\n"
                    "       -m     Average Speed in MPH.\n"
                    "       -k     Average Speed in KPH.\n"
                    "       -e     Elapsed Race Time.\n"
                    "       -g     Show Car Setup (Garage).\n"
                    "       -i     Show Pit Stops (In/out).\n"
                    "       -n     Number of laps.\n"
                    "       -x     Write authentication file (AUTHLAP.TXT).\n"
                    "       -w     Watch log file grow (<Esc> to quit).\n"
                    "       [...]  Specify another name for GPLAPTIM.LOG.\n"
               );
     exit(1);
}

static void
ShowLog(
     char *filename,                    /* In  Log filename                */
     bool watch_mode                    /* In  Watch mode                  */
) {
     LAP_RECORD     lr;
     LB_ENTRY       le;
     ulong          fastest_race_lap         = 0U;
     ulong          leaders_race_time        = 0U;
     uchar          leaders_lap              = 0;
     ulong          fastest_qual_lap         = 0U;
     ushort         practice_laps            = 0;
     ushort         free_practice_laps       = 0;
     ushort         qualifying_laps          = 0;
     ushort         pre_race_practice_laps   = 0;
     ushort         race_laps                = 0;
     char           circuit                  = -1;
     bool           once                     = TRUE;
     int            fd;
     char           buff[132];

     if ((fd = sopen(filename, (int) (O_RDONLY | O_BINARY), SH_DENYNO)) >= 0) {
          LapSetAvgSpeed(fShowAvgSpeed, fAvgSpeedMph);
          LapSetElapsedMode(fElapsed);
          LapRewind(fd);

          (void) printf("\n %s\n", LapTitleText());
          (void) printf(" %s\n", LapTitleUnderlineText());

          fastest_race_lap = 0;
          while (watch_mode || once) {
               once = FALSE;

               if (kbhit()) {
                    if (getch() == '\033') {
                         watch_mode = FALSE;
                         break;
                    }
               }

               while (LapReadNextEntry(&lr, fd)) {
                    if (LapEntryToLbEntry(&le, &lr, &fastest_race_lap, &leaders_race_time, &leaders_lap, &fastest_qual_lap)) {
                         if (IsFilter(&le)) {
                              if ( le.u.lr.lr_record_type == REC_TYPE_LAP       ||
                                   le.u.lr.lr_record_type == REC_TYPE_AUTH      ||
                                   le.u.lr.lr_record_type == REC_TYPE_AUTH_RACE ||
                                   le.u.lr.lr_record_type == REC_TYPE_PIT_IN    ||
                                   le.u.lr.lr_record_type == REC_TYPE_PIT_OUT
                              ) {
                                   if (circuit != CIRCUIT(le.u.lr.lr_game_mode)) {
                                        circuit = CIRCUIT(le.u.lr.lr_game_mode);
                                        (void) printf(" %s\n", LapCircuitText(circuit));
                                   }
                              }
                              LbEntryToText(buff, &le);
                              (void) printf(" %s\n", buff);
                         }

                         if ( le.u.lr.lr_record_type == REC_TYPE_LAP &&
                              IS_PLAYER(le.u.lr.lr_car_number)
                         ) {
                              switch (GAME_MODE(le.u.lr.lr_game_mode)) {
                              case GM_PRACTICE:
                                   ++practice_laps;
                                   break;

                              case GM_FREE_PRACTICE:
                                   ++free_practice_laps;
                                   break;

                              case GM_QUALIFYING:
                                   ++qualifying_laps;
                                   break;

                              case GM_PRE_RACE_PRACTICE:
                                   ++pre_race_practice_laps;
                                   break;

                              case GM_RACE:
                                   ++race_laps;
                                   break;

                              default:
                                   break;
                              }
                         }
                    }
               }

               if (watch_mode) {
                    long t, et;

                    (void) time(&et);
                    et += 5;

                    for ((void) time(&t); t < et; ) {
                         (void) VDM_release_time_slice();
                         (void) time(&t);
                    }
               }
          }

          (void) close(fd);

          if (fShowTotalLaps) {
               (void) printf( "\n Player's Laps\n"
                              "\t Practice:\t\t%u\n"
                              "\t Free Practice:\t\t%u\n"
                              "\t Qualifying:\t\t%u\n"
                              "\t Pre-race Practice:\t%u\n"
                              "\t Race:\t\t\t%u",
                                   practice_laps,
                                   free_practice_laps,
                                   qualifying_laps,
                                   pre_race_practice_laps,
                                   race_laps
                              );
          }
     }
     else {
          (void) printf("gplap: Unable to open '%s'\n", filename);
          Usage();
     }
}

static void
WriteAuthEntries(
     char *filename                     /* In  Log filename                */
) {
     LAP_RECORD     lr;
     LB_ENTRY       le;
     int            fd;
     FILE           *fp;
     ulong          fastest_race_lap    = 0U;
     ulong          leaders_race_time   = 0U;
     uchar          leaders_lap         = 0;
     ulong          fastest_qual_lap    = 0U;

     if ((fd = sopen(filename, (int) (O_RDONLY | O_BINARY), SH_DENYNO)) >= 0) {
          if ((fp = fopen("AUTHLAP.TXT", "a+")) == NULL) {
               (void) printf("gplap: unable to open/create AUTHLAP.TXT\n");
               exit(1);
          }

          (void) fseek(fp, 0L, SEEK_END);

          LapRewind(fd);
          while (LapReadNextEntry(&lr, fd)) {
               if (lr.lr_record_type == REC_TYPE_AUTH || lr.lr_record_type == REC_TYPE_AUTH_RACE) {
                    if (LapEntryToLbEntry(&le, &lr, &fastest_race_lap, &leaders_race_time, &leaders_lap, &fastest_qual_lap)) {
                         LapWriteAuthRecord(fp, &le);
                    }
               }
          }

          (void) close(fd);
          (void) fclose(fp);
     }
     else {
          (void) printf("gplap: Unable to open '%s'\n", filename);
          exit(1);
     }
}

static bool
IsFilter(
     LB_ENTRY  *le                      /* In  List box entry              */
) {
     LAP_RECORD const    *lr            = &le->u.lr;
     bool                fAdd           = FALSE;
     bool                fDoComputer    = FALSE;
     bool                fDoPlayer      = FALSE;

     if (lr->lr_record_type == REC_TYPE_LAP || lr->lr_record_type == REC_TYPE_SPLIT) {
          fDoComputer    = fShowComputer && !IS_PLAYER(lr->lr_car_number);
          fDoPlayer      = fShowPlayer   && IS_PLAYER(lr->lr_car_number);

          if (IS_RACE(lr->lr_game_mode) && fShowRace) {
               if (fShowLeader && lr->lr_car_position == 1) {
                    fAdd = TRUE;
               }

               if ( (fShowFastest && le->fastest_race_lap) ||
                    (fShowBest    && lr->lr_lap_time == lr->lr_best_time)
               ) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }

               if (!fShowFastest && !fShowBest && (fDoComputer || fDoPlayer)) {
                    fAdd = TRUE;
               }
          }

          if (IS_QUALIFYING(lr->lr_game_mode) && fShowQualifying) {
               if (fShowLeader && lr->lr_qual_position == 1) {
                    fAdd = TRUE;
               }

               if ( (fShowFastest && le->fastest_qual_lap) ||
                    (fShowBest && lr->lr_lap_time == lr->lr_best_time)                    
               ) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }

               if (!fShowFastest && !fShowBest && (fDoComputer || fDoPlayer)) {
                    fAdd = TRUE;
               }
          }

          if ( (IS_PRACTICE(lr->lr_game_mode)          && fShowPractice)   ||
               (IS_FREE_PRACTICE(lr->lr_game_mode)     && fShowPractice)   ||
               (IS_PRE_RACE_PRACTICE(lr->lr_game_mode) && fShowPractice)
          ) {
               if ( (fShowFastest && le->fastest_qual_lap) ||
                    (fShowBest && lr->lr_lap_time == lr->lr_best_time)                    
               ) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }

               if (!fShowFastest && !fShowBest && (fDoComputer || fDoPlayer)) {
                    fAdd = TRUE;
               }
          }
     }
     else if (lr->lr_record_type == REC_TYPE_AUTH || lr->lr_record_type == REC_TYPE_AUTH_RACE) {
          if ( (IS_RACE(lr->lr_game_mode)              && fShowRace)       ||
               (IS_QUALIFYING(lr->lr_game_mode)        && fShowQualifying) ||
               (IS_PRACTICE(lr->lr_game_mode)          && fShowPractice)   ||
               (IS_FREE_PRACTICE(lr->lr_game_mode)     && fShowPractice)   ||
               (IS_PRE_RACE_PRACTICE(lr->lr_game_mode) && fShowPractice)
          ) {
               fAdd = fShowAuth;
          }
     }
     else if (lr->lr_record_type == REC_TYPE_SETUP) {
          if (fShowSetup && fShowPlayer) {
               if ( (IS_RACE(lr->lr_game_mode)              && fShowRace)       ||
                    (IS_QUALIFYING(lr->lr_game_mode)        && fShowQualifying) ||
                    (IS_PRACTICE(lr->lr_game_mode)          && fShowPractice)   ||
                    (IS_FREE_PRACTICE(lr->lr_game_mode)     && fShowPractice)   ||
                    (IS_PRE_RACE_PRACTICE(lr->lr_game_mode) && fShowPractice)
               ) {
                    fAdd = TRUE;
               }
          }
     }
     else if (lr->lr_record_type == REC_TYPE_PIT_IN || lr->lr_record_type == REC_TYPE_PIT_OUT) {
          if (fShowPit && !fShowFastest && !fShowBest) {
               fDoComputer    = fShowComputer && !IS_PLAYER(lr->lr_car_number);
               fDoPlayer      = fShowPlayer   && IS_PLAYER(lr->lr_car_number);

               if (IS_RACE(lr->lr_game_mode) && fShowRace) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }

               if (IS_QUALIFYING(lr->lr_game_mode) && fShowQualifying) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }

               if ( (IS_PRACTICE(lr->lr_game_mode)          && fShowPractice)   ||
                    (IS_FREE_PRACTICE(lr->lr_game_mode)     && fShowPractice)   ||
                    (IS_PRE_RACE_PRACTICE(lr->lr_game_mode) && fShowPractice)
               ) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }
          }
     }
     else {
          fAdd = TRUE;
     }

     return fAdd;
}

/*---------------------------------------------------------------------------
** End of File
*/

