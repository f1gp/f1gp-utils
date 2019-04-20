/*
** File   : laputl.h
** Author : TK
** Date   : 15/04/94
**
** $Header:   L:/GPLAPTIM/LAPUTL/VCS/LAPUTL.H__   1.4   05 Mar 1995 15:18:22   tk  $
**
** Grand Prix Lap Time Logger.
*/

#ifndef   laputl_h
#define   laputl_h

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
** Includes
*/

#include <gplaptim.h>

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct {
     union {
          LAP_RECORD          lr;
          AUTH_RECORD         ar;
          DATE_RECORD         dr;
          SETUP_RECORD        sr;
          PIT_IN_RECORD       pi;
          PIT_OUT_RECORD      po;
          SPLIT_TIME_RECORD   st;
     } u;
     char                fastest_race_lap;
     char                fastest_qual_lap;
     unsigned long       leaders_race_time;
     unsigned char       leaders_lap;
     unsigned long       pitstop_time;
     void                *next;
} LB_ENTRY;

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/

void
LapRewind(
     int            fd                  /* In   File to read from          */
);

int
LapReadNextEntry(
     LAP_RECORD     *entry,             /* Out  Entry read                 */
     int            fd                  /* In   File to read from          */
);

int
LapEntryToLbEntry(
     LB_ENTRY       *lbe,               /* Out Output buffer               */
     LAP_RECORD     *entry,             /* In  Entry to convert            */
     unsigned long  *fastest_race_lap,  /* In  Fastest race lap            */
     unsigned long  *leaders_race_time, /* In  Leader's race time          */
     unsigned char  *leaders_lap,       /* In  Leader's lap                */
     unsigned long  *fastest_qual_lap   /* In  Fastest qualifying lap      */
);

void
LbEntryToText(
     char      *buff,                   /* Out Output buffer               */
     LB_ENTRY  *lbe                     /* In  Listbox entry               */
);

char *
LapCircuitText(
     int       circuit_number           /* In  Cicruit number              */
);

void
LapAuthDateToAscii(
     char           *op_buff,           /* Out  Output buffer              */
     AUTH_RECORD    *ar                 /* In   Authenication record       */
);

char *
LapSkillToAscii(
     uchar     skill_level              /* In  Skill level                 */
);

void
LapLapTimeToAscii(
     char   *s,                         /* Out  Output buffer              */
     ulong  tim                         /* In   Lap time                   */
);

void
LapBehindTimeToAscii(
     char   *s,                         /* Out  Output buffer              */
     ulong  tim                         /* In   Race time                  */
);

void
LapRaceTimeToAscii(
     char   *s,                         /* Out  Output buffer              */
     ulong  tim                         /* In   Race time                  */
);

void
LapPitTimeToAscii(
     char   *s,                         /* Out  Output buffer              */
     ulong  tim                         /* In   Lap time                   */
);

void
LapAvgSpeedToAscii(
     char           *s,                 /* Out Output buffer               */
     LAP_RECORD     *lr,                /* In  Lap entry                   */
     int            use_mph             /* In  MPH or KPH                  */
);

void
LapSetAvgSpeed(
     int       enable,                  /* In  Enable average speed display*/
     int       in_mph                   /* In  Show in MPH or KPH          */
);

unsigned char
LapCircuitLaps(
     int       circuit_number           /* In  Cicruit number              */
);

char *
LapTitleText(
     void
);

char *
LapTitleUnderlineText(
     void
);

void
LapWriteAuthRecord(
     FILE      *fp,                     /* In  File to write to            */
     LB_ENTRY  *lbe                     /* In  Listbox entry               */
);

void
LapCopyAuthRecord(
     char      *op_str,                 /* Out String to write to          */
     LB_ENTRY  *lbe                     /* In  Listbox entry               */
);

void
LapSetElapsedMode(
     int       enable                   /* In  Enable elapsed race time    */
);

#ifdef __cplusplus
}
#endif

#endif    /* laputl_h */
/*---------------------------------------------------------------------------
** End of File
*/

