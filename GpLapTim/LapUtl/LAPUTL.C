/*
** File   : laputl.cpp
** Author : TK
** Date   : 15/04/94
**
** $Header:   F:/TK/GPLAPTIM/LAPUTL/VCS/LAPUTL.C__   1.11   09 Oct 1995 22:27:54   tk  $
**
** Grand Prix Lap Time Logger.
*/

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <string.h>
#include <io.h>
/*lint +elib(???) */
#include "laputl.h"
#include "version.i"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#ifndef TRUE
#define TRUE                  1
#endif
#ifndef FALSE
#define FALSE                 0
#endif

#define AVG_SPEED_COLUMN      75

#define NUM_ELEMENTS(s)       (int)(sizeof(s) / sizeof(s[0]))

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static char lapCarNumberCharacter(uchar car_number);

/*---------------------------------------------------------------------------
** Data
*/

/*lint -esym(528,sccsid) */
static const char sccsid[] = "@(#)LapUtl.lib" "\t" VERSION;

static char *months[] = { "?", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", };

static char *weekdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "?" };

static char *skill[] = { "Rookie", "Amateur", "Semi-Pro", "Pro", "Ace", "?" };

static char *circuits[] = {
     "Phoenix, United States",
     "Interlagos, Brazil",
     "Imola, San Marino",
     "Monte-Carlo, Monaco",
     "Montreal, Canada",
     "Mexico City, Mexico",
     "Magny-Cours, France",
     "Silverstone, Great Britain",
     "Hockenheim, Germany",
     "Hungaroring, Hungary",
     "Spa-Francorchamps, Belgium",
     "Monza, Italy",
     "Estoril, Portugal",
     "Barcelona, Spain",
     "Suzuka, Japan",
     "Adelaide, Australia",
     "?"
};

static unsigned long circuits_length_miles[] = {
     2280,                    /* Phoenix             - United States  */
     2688,                    /* Interlagos          - Brazil         */
     3132,                    /* Imola               - San Marino     */
     2068,                    /* Monte-Carlo         - Monaco         */
     2753,                    /* Montreal            - Canada         */
     2747,                    /* Mexico City         - Mexico         */
     2654,                    /* Magny Cours         - France         */
     3202,                    /* Silverstone         - Great Britain  */
     4227,                    /* Hockenheim          - Germany        */
     2466,                    /* Hungaroring         - Hungary        */
     4313,                    /* Spa-Francorchamps   - Belgium        */
     3604,                    /* Monza               - Italy          */
     2703,                    /* Estoril             - Portugal       */
     2950,                    /* Barcelona           - Spain          */
     3641,                    /* Suzuka              - Japan          */
     2349,                    /* Adelaide            - Australia      */
};

static unsigned long circuits_length_kilometers[] = {
     3668,                    /* Phoenix             - United States  */
     4325,                    /* Interlagos          - Brazil         */
     5040,                    /* Imola               - San Marino     */
     3328,                    /* Monte-Carlo         - Monaco         */
     4430,                    /* Montreal            - Canada         */
     4421,                    /* Mexico City         - Mexico         */
     4271,                    /* Magny Cours         - France         */
     5153,                    /* Silverstone         - Great Britain  */
     6802,                    /* Hockenheim          - Germany        */
     3968,                    /* Hungaroring         - Hungary        */
     6940,                    /* Spa-Francorchamps   - Belgium        */
     5800,                    /* Monza               - Italy          */
     4350,                    /* Estoril             - Portugal       */
     4747,                    /* Barcelona           - Spain          */
     5859,                    /* Suzuka              - Japan          */
     3780                     /* Adelaide            - Australia      */
};

static int     fAvgSpeed = FALSE;
static int     fMph      = TRUE;
static int     fElapsed  = FALSE;

static unsigned char circuits_laps[] = {
     81,                      /* Phoenix             - United States  */
     71,                      /* Interlagos          - Brazil         */
     61,                      /* Imola               - San Marino     */
     78,                      /* Monte-Carlo         - Monaco         */
     69,                      /* Montreal            - Canada         */
     69,                      /* Mexico City         - Mexico         */
     72,                      /* Magny Cours         - France         */
     59,                      /* Silverstone         - Great Britain  */
     45,                      /* Hockenheim          - Germany        */
     77,                      /* Hungaroring         - Hungary        */
     44,                      /* Spa-Francorchamps   - Belgium        */
     53,                      /* Monza               - Italy          */
     71,                      /* Estoril             - Portugal       */
     65,                      /* Barcelona           - Spain          */
     53,                      /* Suzuka              - Japan          */
     81,                      /* Adelaide            - Australia      */
};

static char szTitle1[]          = "Pos Car Lap     Driver               Lap Time  Best Lap   Race Time";
static char szTitleUnderline1[] = "-----------------------------------------------------------------------------";

static char szTitle2[]          = "Pos Car Lap     Driver               Lap Time  Best Lap   Race Time        Avg Speed";
static char szTitleUnderline2[] = "------------------------------------------------------------------------------------";

static unsigned long pit_time[CAR_NUMBER_MASK + 1];

/*---------------------------------------------------------------------------
** Functions
*/

void
LapRewind(
     int            fd                  /* In   File to read from          */
) {
     (void) lseek(fd, 0L, SEEK_SET);
}

int
LapReadNextEntry(
     LAP_RECORD     *entry,             /* Out  Entry read                 */
     int            fd                  /* In   File to read from          */
) {
     return (read(fd, entry, sizeof(*entry)) == (int) sizeof(*entry));
}

int
LapEntryToLbEntry(
     LB_ENTRY       *lbe,               /* Out Output buffer               */
     LAP_RECORD     *entry,             /* In  Entry to convert            */
     unsigned long  *fastest_race_lap,  /* In  Fastest race lap            */
     unsigned long  *leaders_race_time, /* In  Leader's race time          */
     unsigned char  *leaders_lap,       /* In  Leader's lap                */
     unsigned long  *fastest_qual_lap   /* In  Fastest qualifying lap      */
) {
     static int     game_mode = -1;
     bool           ok        = FALSE;

     (void) memset(lbe, 0x00, sizeof(*lbe));
     (void) memcpy(&lbe->u.lr, entry, sizeof(lbe->u.lr));

     if (entry->lr_record_type == REC_TYPE_LAP) {
          LAP_RECORD     *lr = &lbe->u.lr;

          if (game_mode != GAME_MODE(lr->lr_game_mode)) {
               game_mode         = GAME_MODE(lr->lr_game_mode);
               *fastest_qual_lap = 0xffffffffUL;
               *fastest_race_lap = 0xffffffffUL;
          }

          if (game_mode == GM_RACE) {
               if (!(lr->lr_best_time & TIME_MASK) && lr->lr_best_time < *fastest_race_lap) {
                    *fastest_race_lap = lr->lr_best_time;
                    lbe->fastest_race_lap = TRUE;
               }
               else {
                    lbe->fastest_race_lap = FALSE;
               }

               /*
               ** Catch the leader or the next person when the leader pits.
               */
               if (lr->lr_car_position == 1 || lr->lr_lap_number > *leaders_lap) {
                    *leaders_race_time       = lr->lr_race_time;
                    *leaders_lap             = lr->lr_lap_number;
               }
               lbe->leaders_race_time = *leaders_race_time;
               lbe->leaders_lap       = *leaders_lap;
          }
          else {
               /*
               ** Qualifying.
               */
               if (!(lr->lr_best_time & TIME_MASK) && lr->lr_best_time < *fastest_qual_lap) {
                    *fastest_qual_lap = lr->lr_best_time;
                    lbe->fastest_qual_lap  = TRUE;
               }
               else {
                    lbe->fastest_qual_lap  = FALSE;
               }
          }
          ok = TRUE;
     }
     else if (entry->lr_record_type == REC_TYPE_AUTH) {
          ok = TRUE;               
     }
     else if (entry->lr_record_type == REC_TYPE_AUTH_RACE) {
          ok = TRUE;               
     }
     else if (entry->lr_record_type == REC_TYPE_DATE) {
          ok = TRUE;               
     }
     else if (entry->lr_record_type == REC_TYPE_SETUP) {
          ok = TRUE;               
     }
     else if (entry->lr_record_type == REC_TYPE_PIT_IN) {
          PIT_IN_RECORD  *pi  = &lbe->u.pi;

          ok = TRUE;

          if (GAME_MODE(lbe->u.pi.pi_game_mode) == GM_RACE) {
               if (CAR_NUMBER(pi->pi_car_number) != 0) {
                    pit_time[CAR_NUMBER(pi->pi_car_number) - 1] = pi->pi_in_time;
               }
          }
     }
     else if (entry->lr_record_type == REC_TYPE_PIT_OUT) {
          PIT_OUT_RECORD *po  = &lbe->u.po;

          ok = TRUE;

          if (GAME_MODE(lbe->u.po.po_game_mode) == GM_RACE) {
               if (CAR_NUMBER(po->po_car_number) != 0) {
                    if (pit_time[CAR_NUMBER(po->po_car_number) - 1]) {
                         lbe->pitstop_time = po->po_out_time - (pit_time[CAR_NUMBER(po->po_car_number) - 1] + 300L);
                    }
                    pit_time[CAR_NUMBER(po->po_car_number) - 1] = 0L;
               }
          }
     }
     else if (entry->lr_record_type == REC_TYPE_SPLIT) {
          ok = TRUE;               
     }

     return ok;
}

void
LbEntryToText(
     char      *buff,                   /* Out Output buffer               */
     LB_ENTRY  *lbe                     /* In  Listbox entry               */
) {
     char      tmp[132];

     if (lbe->u.lr.lr_record_type == REC_TYPE_LAP) {
          LAP_RECORD     *lr = &lbe->u.lr;

          (void) strcpy(buff, "");

          if (GAME_MODE(lbe->u.lr.lr_game_mode) == GM_RACE) {
               (void) sprintf(tmp, "%2u. %c%-2u %3u %-24s",
                         lr->lr_car_position,
                         lapCarNumberCharacter(lr->lr_car_number),
                         CAR_NUMBER(lr->lr_car_number),
                         lr->lr_lap_number,
                         lr->lr_driver
                    );

               (void) strcat(buff, tmp);
               (void) strcat(buff, " ");
               LapLapTimeToAscii(tmp, lr->lr_lap_time);
               (void) strcat(buff, tmp);
               (void) strcat(buff, " ");

               LapLapTimeToAscii(tmp, lr->lr_best_time);
               (void) strcat(buff, tmp);

               (void) strcat(buff, (lr->lr_lap_time == lr->lr_best_time) ? "!" : " ");
               (void) strcat(buff, (lbe->fastest_race_lap) ? "FL" : "  ");

               /*
               ** Catch the leader or the next person when the leader pits.
               */
               if (lr->lr_car_position == 1 || lr->lr_lap_number > lbe->leaders_lap) {
                    (void) strcat(buff, " ");
                    LapRaceTimeToAscii(tmp, lr->lr_race_time);
                    (void) strcat(buff, tmp);
               }
               else {
                    if (fElapsed) {
                         (void) strcat(buff, " ");
                         LapRaceTimeToAscii(tmp, lr->lr_race_time);
                         (void) strcat(buff, tmp);
                    }
                    else if (lbe->leaders_lap == lr->lr_lap_number) {
                         (void) strcat(buff, "   ");
                         LapBehindTimeToAscii(tmp, lr->lr_race_time - lbe->leaders_race_time);
                         (void) strcat(buff, tmp);
                    }
                    else {
                         ushort laps_behind = lbe->leaders_lap - lr->lr_lap_number;

                         (void) sprintf(tmp, " %u lap%s ", laps_behind, (laps_behind == 1) ? " " : "s");
                         (void) strcat(buff, tmp);
                         LapBehindTimeToAscii(tmp, lr->lr_race_time - lbe->leaders_race_time);
                         (void) strcat(buff, tmp);
                    }
               }

               if (fAvgSpeed) {
                    while (strlen(buff) < AVG_SPEED_COLUMN) {
                         (void) strcat(buff, " ");
                    }
                    LapAvgSpeedToAscii(tmp, lr, fMph);
                    (void) strcat(buff, tmp);
               }
          }
          else {
               /*
               ** Pre-Race Practice/Qualifying/Warm Up.
               */
               if (GAME_MODE(lr->lr_game_mode) == GM_QUALIFYING) {
                    if (lr->lr_best_time & TIME_MASK) {
                         (void) strcat(buff, "Q - ");
                    }
                    else if (lr->lr_qual_position == 0) {
                         (void) strcat(buff, "DNQ ");
                    }
                    else {
                         (void) sprintf(tmp, "Q%2u ", lr->lr_qual_position);
                         (void) strcat(buff, tmp);
                    }
               }
               else if (GAME_MODE(lr->lr_game_mode) == GM_FREE_PRACTICE) {
                    if (lr->lr_best_time & TIME_MASK) {
                         (void) strcat(buff, "F - ");
                    }
                    else {
                         (void) sprintf(tmp, "F%2u ", lr->lr_qual_position);
                         (void) strcat(buff, tmp);
                    }
               }
               else if (GAME_MODE(lr->lr_game_mode) == GM_PRE_RACE_PRACTICE) {
                    if (lr->lr_best_time & TIME_MASK) {
                         (void) strcat(buff, "P - ");
                    }
                    else {
                         (void) sprintf(tmp, "P%2u ", lr->lr_qual_position);
                         (void) strcat(buff, tmp);
                    }
               }
               else {
                    (void) strcat(buff, "T - ");
               }

               (void) sprintf(tmp, "%c%-2u %3u %-24s",
                         lapCarNumberCharacter(lr->lr_car_number),
                         CAR_NUMBER(lr->lr_car_number),
                         lr->lr_lap_number,
                         lr->lr_driver
                    );
               (void) strcat(buff, tmp);

               (void) strcat(buff, " ");
               LapLapTimeToAscii(tmp, lr->lr_lap_time);
               (void) strcat(buff, tmp);
               (void) strcat(buff, " ");

               LapLapTimeToAscii(tmp, lr->lr_best_time);
               (void) strcat(buff, tmp);

               (void) strcat(buff, (lr->lr_lap_time == lr->lr_best_time) ? "!" : " ");
               if (GAME_MODE(lr->lr_game_mode) == GM_QUALIFYING) {
                    (void) strcat(buff, (lbe->fastest_qual_lap) ? "FQ" : "  ");
               }
               else {
                    (void) strcat(buff, (lbe->fastest_qual_lap) ? "FL" : "  ");
               }

               if (fAvgSpeed) {
                    while (strlen(buff) < AVG_SPEED_COLUMN) {
                         (void) strcat(buff, " ");
                    }
                    LapAvgSpeedToAscii(tmp, lr, fMph);
                    (void) strcat(buff, tmp);
               }
          }
     }
     else if ( lbe->u.ar.ar_record_type == REC_TYPE_AUTH    ||
               lbe->u.ar.ar_record_type == REC_TYPE_AUTH_RACE
     ) {
          AUTH_RECORD    *ar   = &lbe->u.ar;
          ushort         hours;
          bool           am;

          if (lbe->u.ar.ar_record_type == REC_TYPE_AUTH) {
               LapLapTimeToAscii(buff, ar->ar_lap_time);
          }
          else {
               LapRaceTimeToAscii(buff, ar->ar_lap_time);
          }
          (void) strcat(buff, " ");
          (void) strcat(buff, (char *) ar->ar_driver);
          (void) strcat(buff, " ");

          (void) sprintf(&buff[strlen(buff)], "BHP:%u AIG:%u %s",
                    ar->ar_bhp,
                    ar->ar_ai_grip,
                    LapSkillToAscii(ar->ar_skill)
               );

          if (ar->ar_drv_aids & DRV_AID_F1) {
               (void) strcat(buff, ",F1");
          }
          if (ar->ar_drv_aids & DRV_AID_F2) {
               (void) strcat(buff, ",F2");
          }
          if (ar->ar_drv_aids & DRV_AID_F3) {
               (void) strcat(buff, ",F3");
          }
          if (ar->ar_drv_aids & DRV_AID_F4) {
               (void) strcat(buff, ",F4");
          }
          if (ar->ar_drv_aids & DRV_AID_F5) {
               (void) strcat(buff, ",F5");
          }
          if (ar->ar_drv_aids & DRV_AID_F6) {
               (void) strcat(buff, ",F6");
          }
          if (ar->ar_drv_aids & DRV_AID_STEERING) {
               (void) strcat(buff, ",S");
          }
          if (ar->ar_drv_aids & DRV_AID_TRACTION) {
               (void) strcat(buff, ",T");
          }

          am = TRUE;
          hours = ar->ar_hours;
          if (hours >= 12) {
               am = FALSE;
               if (hours > 12) {
                    hours -= 12;
               }
          }

          if (ar->ar_month > 12) {
               ar->ar_month = 0;
          }
          (void) sprintf(&buff[strlen(buff)], " %u-%s-%u %u:%02u%s",
                    ar->ar_day,
                    months[ar->ar_month],
                    ar->ar_year - 1900,
                    hours,
                    ar->ar_minutes,
                    (am) ? "am" : "pm"
               );
     }
     else if (lbe->u.dr.dr_record_type == REC_TYPE_DATE) {
          DATE_RECORD    *dr = &lbe->u.dr;
          ushort         hours;
          bool           am;

          am = TRUE;
          hours = dr->dr_hours;
          if (hours >= 12) {
               am = FALSE;
               if (hours > 12) {
                    hours -= 12;
               }
          }

          if (dr->dr_week_day > 7) {
               dr->dr_week_day = 7;
          }
          if (dr->dr_month > 12) {
               dr->dr_month = 0;
          }
          (void) sprintf(buff, "%s %u %s %u %2u:%02u%s",
                    weekdays[dr->dr_week_day],
                    dr->dr_day,
                    months[dr->dr_month],
                    dr->dr_year,
                    hours,
                    dr->dr_minutes,
                    (am) ? "am" : "pm"
               );
     }
     else if (lbe->u.lr.lr_record_type == REC_TYPE_SETUP) {
          SETUP_RECORD   *sr  = &lbe->u.sr;
          short          bias = sr->sr_brake_bias;

          if (sr->sr_using_tyres > 6) {
               sr->sr_using_tyres = 6;
          }
          (void) sprintf(buff, "Wings:F%02u/R%02u  Brake-Bias:%c%u  Gears:%02u,%02u,%02u,%02u,%02u,%02u  Tyres:%c",
                    sr->sr_front_wing,
                    sr->sr_rear_wing,
                    (sr->sr_brake_bias <= 0) ? 'R' : 'F',
                    (sr->sr_brake_bias <= 0) ? -bias : bias,
                    sr->sr_gear1,
                    sr->sr_gear2,
                    sr->sr_gear3,
                    sr->sr_gear4,
                    sr->sr_gear5,
                    sr->sr_gear6,
                    "ABCDQW?"[sr->sr_using_tyres]
               );

     }
     else if (lbe->u.lr.lr_record_type == REC_TYPE_PIT_IN) {
          PIT_IN_RECORD  *pi  = &lbe->u.pi;

          if (GAME_MODE(lbe->u.pi.pi_game_mode) == GM_RACE) {
               (void) sprintf(buff, "%2u. ", pi->pi_car_position);
          }
          else if (GAME_MODE(pi->pi_game_mode) == GM_QUALIFYING) {
               if (pi->pi_car_position == 0) {
                    (void) strcpy(buff, "Q - ");
               }
               else {
                    (void) sprintf(buff, "Q%2u ", pi->pi_car_position);
               }
          }
          else if (GAME_MODE(pi->pi_game_mode) == GM_FREE_PRACTICE) {
               if (pi->pi_car_position == 0) {
                    (void) strcpy(buff, "F - ");
               }
               else {
                    (void) sprintf(buff, "F%2u ", pi->pi_car_position);
               }
          }
          else if (GAME_MODE(pi->pi_game_mode) == GM_PRE_RACE_PRACTICE) {
               if (pi->pi_car_position == 0) {
                    (void) strcpy(buff, "P - ");
               }
               else {
                    (void) sprintf(buff, "P%2u ", pi->pi_car_position);
               }
          }
          else {
               (void) strcpy(buff, "T - ");
          }

          (void) sprintf(tmp, "%c%-2u %3u %-24s Pit In",
                    lapCarNumberCharacter(pi->pi_car_number),
                    CAR_NUMBER(pi->pi_car_number),
                    pi->pi_lap_number,
                    pi->pi_driver
               );
          (void) strcat(buff, tmp);
     }
     else if (lbe->u.lr.lr_record_type == REC_TYPE_PIT_OUT) {
          PIT_OUT_RECORD *po  = &lbe->u.po;

          if (GAME_MODE(lbe->u.po.po_game_mode) == GM_RACE) {
               (void) sprintf(buff, "%2u. ", po->po_car_position);
          }
          else if (GAME_MODE(po->po_game_mode) == GM_QUALIFYING) {
               if (po->po_car_position == 0) {
                    (void) strcpy(buff, "Q - ");
               }
               else {
                    (void) sprintf(buff, "Q%2u ", po->po_car_position);
               }
          }
          else if (GAME_MODE(po->po_game_mode) == GM_FREE_PRACTICE) {
               if (po->po_car_position == 0) {
                    (void) strcpy(buff, "F - ");
               }
               else {
                    (void) sprintf(buff, "F%2u ", po->po_car_position);
               }
          }
          else if (GAME_MODE(po->po_game_mode) == GM_PRE_RACE_PRACTICE) {
               if (po->po_car_position == 0) {
                    (void) strcpy(buff, "P - ");
               }
               else {
                    (void) sprintf(buff, "P%2u ", po->po_car_position);
               }
          }
          else {
               (void) strcpy(buff, "T - ");
          }

          (void) sprintf(tmp, "%c%-2u %3u %-24s Pit Out  ",
                    lapCarNumberCharacter(po->po_car_number),
                    CAR_NUMBER(po->po_car_number),
                    po->po_lap_number,
                    po->po_driver
               );
          (void) strcat(buff, tmp);

          if (GAME_MODE(lbe->u.po.po_game_mode) == GM_RACE) {
               if (lbe->pitstop_time != 0L) {
                    LapPitTimeToAscii(tmp, lbe->pitstop_time);
                    (void) strcat(buff, tmp);
               }
          }
     }
     else if (lbe->u.lr.lr_record_type == REC_TYPE_SPLIT) {
          SPLIT_TIME_RECORD *st = &lbe->u.st;

          (void) strcpy(buff, "");

          if (GAME_MODE(lbe->u.st.st_game_mode) != GM_RACE) {
               /*
               ** Pre-Race Practice/Qualifying/Warm Up.
               */
               if (GAME_MODE(st->st_game_mode) == GM_QUALIFYING) {
                    if (st->st_qual_position == 0) {
                         (void) strcat(buff, "DNQ ");
                    }
                    else {
                         (void) sprintf(tmp, "Q%2u ", st->st_qual_position);
                         (void) strcat(buff, tmp);
                    }
               }
               else if (GAME_MODE(st->st_game_mode) == GM_FREE_PRACTICE) {
                    (void) sprintf(tmp, "F%2u ", st->st_qual_position);
                    (void) strcat(buff, tmp);
               }
               else if (GAME_MODE(st->st_game_mode) == GM_PRE_RACE_PRACTICE) {
                    (void) sprintf(tmp, "P%2u ", st->st_qual_position);
                    (void) strcat(buff, tmp);
               }
               else {
                    (void) strcat(buff, "T - ");
               }

               (void) sprintf(tmp, "%c%-2u %3u %-24s",
                         lapCarNumberCharacter(st->st_car_number),
                         CAR_NUMBER(st->st_car_number),
                         st->st_lap_number,
                         st->st_driver
                    );
               (void) strcat(buff, tmp);

               (void) strcat(buff, " ");
               LapLapTimeToAscii(tmp, st->st_split_time);
               (void) strcat(buff, tmp);
               (void) strcat(buff, " ");

               (void) sprintf(tmp, "Split #%u @ %u%%", st->st_point, st->st_percentage);
               (void) strcat(buff, tmp);
          }
     }
}

char *
LapCircuitText(
     int       circuit_number           /* In  Cicruit number              */
) {
     if (circuit_number >= NUM_ELEMENTS(circuits)) {
          circuit_number = NUM_ELEMENTS(circuits) - 1;
     }
     return circuits[circuit_number];
}

void
LapAuthDateToAscii(
     char           *op_buff,           /* Out  Output buffer              */
     AUTH_RECORD    *ar                 /* In   Authenication record       */
) {
     ushort    hours = ar->ar_hours;
     bool      am    = TRUE;

     if (hours >= 12) {
          am = FALSE;
          if (hours > 12) {
               hours -= 12;
          }
     }

     if (ar->ar_month > 12) {
          ar->ar_month = 0;
     }
     (void) sprintf(op_buff, " %u-%s-%u %u:%02u%s",
               ar->ar_day,
               months[ar->ar_month],
               ar->ar_year - 1900,
               hours,
               ar->ar_minutes,
               (am) ? "am" : "pm"
          );
}

char *
LapSkillToAscii(
     uchar     skill_level              /* In  Skill level                 */
) {
     if (skill_level >= NUM_ELEMENTS(skill)) {
          skill_level = NUM_ELEMENTS(skill) - 1;
     }
     return skill[skill_level];
}

void
LapLapTimeToAscii(
     char   *s,                         /* Out  Output buffer              */
     ulong  tim                         /* In   Lap time                   */
) {
     ulong  thous;
     ulong  secs;
     ulong  mins;

     if (tim & TIME_MASK) {
          (void) sprintf(s, "-:--.---");
          return;
     }
     thous = tim % 1000U;
     tim /= 1000L;

     secs = tim % 60U;
     tim /= 60U;

     mins = tim % 60U;
     tim /= 60U;

     (void) sprintf(s, "%lu:%02lu.%03lu", mins, secs, thous);
}

void
LapBehindTimeToAscii(
     char   *s,                         /* Out  Output buffer              */
     ulong  tim                         /* In   Race time                  */
) {
     ulong  thous;
     ulong  secs;
     ulong  mins;

     tim &= ~TIME_MASK;

     thous = tim % 1000U;
     tim /= 1000L;

     secs = tim % 60U;
     tim /= 60U;

     mins = tim % 60U;
     tim /= 60U;

     if (mins == 0) {
          (void) sprintf(s, "+%lu.%03lu", secs, thous);
     }
     else {
          (void) sprintf(s, "+%lu:%02lu.%03lu", mins, secs, thous);
     }
}

void
LapRaceTimeToAscii(
     char   *s,                         /* Out  Output buffer              */
     ulong  tim                         /* In   Race time                  */
) {
     ulong  thous;
     ulong  secs;
     ulong  mins;
     ulong  hours;

     tim &= ~TIME_MASK;

     thous = tim % 1000U;
     tim /= 1000L;

     secs = tim % 60U;
     tim /= 60U;

     mins = tim % 60U;
     tim /= 60U;

     hours = tim;

     if (hours == 0) {
          (void) sprintf(s, "%lu:%02lu.%03lu", mins, secs, thous);
     }
     else {
          (void) sprintf(s, "%luh%02lu:%02lu.%03lu", hours, mins, secs, thous);
     }
}

void
LapPitTimeToAscii(
     char   *s,                         /* Out  Output buffer              */
     ulong  tim                         /* In   Lap time                   */
) {
     ulong  thous;
     ulong  secs;

     if (tim & TIME_MASK) {
          (void) strcpy(s, "");
          return;
     }

     thous = tim % 1000U;
     tim /= 1000L;

     secs = tim;
     (void) sprintf(s, "%2lu.%1lus", secs, thous / 100L);
}

void
LapAvgSpeedToAscii(
     char           *s,                 /* Out Output buffer               */
     LAP_RECORD     *lr,                /* In  Lap entry                   */
     int            use_mph             /* In  MPH or KPH                  */
) {
     int            circuit;
     unsigned long  t;
     unsigned long  v;
     unsigned long  cl;
     unsigned long  mod;
     unsigned long  rem;

     if (lr->lr_lap_time & TIME_MASK) {
          (void) strcpy(s, "");
     }
     else {
          circuit   = CIRCUIT(lr->lr_game_mode);
          t         = lr->lr_lap_time;

          if (circuit >= NUM_ELEMENTS(circuits_length_miles)) {
               circuit = 0;
          }

          if (use_mph) {
               cl = circuits_length_miles[circuit];
          }
          else {
               cl = circuits_length_kilometers[circuit];
          }

          v   = (3600UL * cl * 100UL) / t;
          mod = v / 100UL;
          rem = v % 100UL;
          (void) sprintf(s, "%3lu.%02lu%s", mod, rem, (use_mph) ? "mph" : "kph");
     }
}

void
LapSetAvgSpeed(
     int       enable,                  /* In  Enable average speed display*/
     int       in_mph                   /* In  Show in MPH or KPH          */
) {
     fAvgSpeed = enable;
     fMph      = in_mph;
}

unsigned char
LapCircuitLaps(
     int       circuit_number           /* In  Cicruit number              */
) {
     if (circuit_number >= NUM_ELEMENTS(circuits_laps)) {
          circuit_number = 0;
     }
     return circuits_laps[circuit_number];
}

char *
LapTitleText(
     void
) {
     return ((fAvgSpeed) ? szTitle2 : szTitle1);
}

char *
LapTitleUnderlineText(
     void
) {
     return ((fAvgSpeed) ? szTitleUnderline2 : szTitleUnderline1);
}

void
LapWriteAuthRecord(
     FILE      *fp,                     /* In  File to write to            */
     LB_ENTRY  *lbe                     /* In  Listbox entry               */
) {
     char           buff[132];
     unsigned char  *p;
     unsigned short i;

     if (lbe->u.ar.ar_record_type == REC_TYPE_AUTH || lbe->u.ar.ar_record_type == REC_TYPE_AUTH_RACE) {
          LbEntryToText(buff, lbe);
          (void) fprintf(fp, " %s\n", LapCircuitText(CIRCUIT(lbe->u.ar.ar_game_mode)));
          (void) fprintf(fp, " %s\n", buff);          
          (void) fprintf(fp, " AUTH1:[");
          for (i = 0, p = (unsigned char *) &lbe->u.ar; i < sizeof(lbe->u.ar); i++) {
               if (i == sizeof(AUTH_RECORD) / 2) {
                    (void) fprintf(fp, "]\n AUTH2:[");
               }
               (void) fprintf(fp, "%02X", *p++);
          }
          (void) fprintf(fp, "]\n\n");
     }
}

void
LapCopyAuthRecord(
     char      *op_str,                 /* Out String to write to          */
     LB_ENTRY  *lbe                     /* In  Listbox entry               */
) {
     char           buff[132];
     unsigned char  *p;
     unsigned short i;

     *op_str = '\0';
     if (lbe->u.ar.ar_record_type == REC_TYPE_AUTH || lbe->u.ar.ar_record_type == REC_TYPE_AUTH_RACE) {
          (void) sprintf(buff, " %s\r\n", LapCircuitText(CIRCUIT(lbe->u.ar.ar_game_mode)));
          (void) strcpy(op_str, buff);

          (void) strcat(op_str, " ");
          LbEntryToText(buff, lbe);
          (void) strcat(op_str, buff);
          (void) strcat(op_str, "\r\n");

          (void) strcat(op_str, " AUTH1:[");
          for (i = 0, p = (unsigned char *) &lbe->u.ar; i < sizeof(lbe->u.ar); i++) {
               if (i == sizeof(AUTH_RECORD) / 2) {
                    (void) strcat(op_str, "]\r\n AUTH2:[");
               }
               (void) sprintf(buff, "%02X", *p++);
               (void) strcat(op_str, buff);
          }
          (void) strcat(op_str, "]\r\n\r\n\0\0");
     }
}

void
LapSetElapsedMode(
     int       enable                   /* In  Enable elapsed race time    */
) {
     fElapsed = enable;
}

static char
lapCarNumberCharacter(
     uchar car_number
) {
     char c = '#';

     if (IS_PLAYER(car_number)) {
          if (IS_PLAYER_CC(car_number)) {
               c = '%';
          }
          else {
               c = '*';
          }
     }

     return c;
}

/*---------------------------------------------------------------------------
** End of File
*/

