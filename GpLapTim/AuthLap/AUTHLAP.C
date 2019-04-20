/*
** File   : authlap.c
** Author : TK
** Date   : 16/04/94
**
** $Header:   F:/TK/GPLAPTIM/AUTHLAP/VCS/AUTHLAP.C__   1.11   07 Apr 1996 12:44:16   tk  $
**
** Authenticator of GPLAPTIM fast laps.
**
** V2.0 16/04/94    First cut.
** V2.1 14/05/94    Changed "SA" to "S".
** V3.0 01/Jan/95   Added detection of illegal wing/tyre settings.
**                  Changed LRC modifier value from 'TK' to 'KT'.
** V4.0 27/Feb/95   Added support for detection of save/reloaded games,
**                  BHP modification and grass detection.
**                  Changed LRC modifier value from 'KT' to 'tk'.
** V5.0 02/Mar/95   Use of V4.1 now highlighted for good authentications.
** V5.1 06/Mar/95   Added display of game mode.
** V6.0 07/Oct/95   Added support for elapsed race time and damage limits.
** V6.1 04/Apr/96   Updated version number to stay in step with GPLAPTIM.
*/

#define VERSION               "V6.1 4th Apr 96"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/*lint +elib(???) */
#include <gplaptim.h>

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE                  1
#define FALSE                 0

#define AUTH1_SIZE            ((sizeof(AUTH_RECORD) * 2) / 2)
#define AUTH2_SIZE            ((sizeof(AUTH_RECORD) * 2) - AUTH1_SIZE)

#define ILLEGAL_WINGS         0x01
#define ILLEGAL_TYRES         0x02
#define ILLEGAL_10S_RELOAD    0x04
#define ILLEGAL_30S_RELOAD    0x08
#define ILLEGAL_BHP           0x10
#define ILLEGAL_GRASS_USED    0x20
#define ILLEGAL_DAMAGE        0x40

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void    ShowIllegalEntry(uchar seconds_field);
static void    ShowAuthEntry(AUTH_RECORD *entry);
static void    sprint_lap_time(char *s, ulong tim);
static void    hex_pack(void *dst, void const *src, ushort unpacked_len);
static ushort  sum_check_old(void const *ptr, ushort length);
static ushort  sum_check_v41(void const *ptr, ushort length);
static ushort  sum_check_v50(void const *ptr, ushort length);
static ushort  sum_check_v60(void const *ptr, ushort length);

/*---------------------------------------------------------------------------
** Data
*/

/*lint -esym(528,sccsid) */
static const char sccsid[] = "@(#)" __FILE__ "\t" VERSION;

static char pattern1[] = "AUTH1:[";
static char pattern2[] = "AUTH2:[";

static char         unpacked_record[sizeof(AUTH_RECORD) * 2];
static AUTH_RECORD  ar;

static char *months[] = { "", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

static char *skill[] = { "Rookie", "Amateur", "Semi-Pro", "Pro", "Ace" };

static char *circuits[] = {
     "Phoenix - United States",
     "Interlagos - Brazil",
     "Imola - San Marino",
     "Monte-Carlo - Monaco",
     "Montreal - Canada",
     "Mexico City - Mexico",
     "Magny Cours - France",
     "Silverstone - Great Britain",
     "Hockenheim - Germany",
     "Hungaroring - Hungary",
     "Spa-Francorchamps - Belgium",
     "Monza - Italy",
     "Estoril - Portugal",
     "Barcelona - Spain",
     "Suzuka - Japan",
     "Adelaide - Australia",
};

static char *game_mode_text[] = {
     "Practice",
     "Free Practice",
     "Qualifying",
     "Pre-Race Practice",
     "Race"
};

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE           *fp;
     int            ok          = 1;
     int            matched1    = FALSE;
     int            matched2    = FALSE;
     int            match_index = 0;
     int            ch;
     unsigned char  c;
     unsigned short i;

     (void) printf("AuthLap " VERSION "\n");

     if (argc == 1) {
          (void) printf("Usage: authlap (filename) [filename] [...]\n");
          return 1;
     }

     for (--argv, ++argv; argc; argc--, argv++) {
          if ((fp = fopen(*argv, "r")) == NULL) {
               (void) printf("authlap: Unable to open file '%s'.\n", *argv);
               return 0;
          }

          while ((ch = fgetc(fp)) != EOF) {
               c = (unsigned char) ch;

               if (!matched1) {
                    if (c == pattern1[0]) {
                         match_index = 1;
                    }
                    else if (c == pattern1[match_index]) {
                         if (++match_index >= (int) sizeof(pattern1) - 1) {
                              for (i = 0; i < AUTH1_SIZE; i++) {
                                   unpacked_record[i] = (char) fgetc(fp);
                              }
                              match_index = 0;
                              matched1 = TRUE;
                         }
                    }
                    else {
                         match_index = 0;
                    }
               }

               if (matched1 && !matched2) {
                    if (c == pattern2[0]) {
                         match_index = 1;
                    }
                    else if (c == pattern2[match_index]) {
                         if (++match_index >= (int) sizeof(pattern2) - 1) {
                              for (i = 0; i < AUTH2_SIZE; i++) {
                                   unpacked_record[i + AUTH1_SIZE] = (char) fgetc(fp);
                              }
                              matched2 = TRUE;
                         }
                    }
                    else {
                         match_index = 0;
                    }
               }

               if (matched1 && matched2) {
                    hex_pack(&ar, unpacked_record, sizeof(unpacked_record));

                    ShowAuthEntry(&ar);
                    if (ar.ar_crc == sum_check_v60(&ar, sizeof(ar) - sizeof(ar.ar_crc))) {
                         if (ar.ar_seconds & 0x80) {
                              (void) printf("\aAuthentication code OK, but:\n");
                              ShowIllegalEntry(ar.ar_seconds);
                         }
                         else
                         {
                              (void) printf("Authentication code OK\n");
                         }
                    }
                    else if (ar.ar_crc == sum_check_v50(&ar, sizeof(ar) - sizeof(ar.ar_crc))) {
                         if (ar.ar_seconds & 0x80) {
                              (void) printf("\aAuthentication code OK (V5.0 record, no checks for illegal damage), but:\n");
                              ShowIllegalEntry(ar.ar_seconds);
                         }
                         else
                         {
                              (void) printf("Authentication code OK (V5.0 record, no checks for illegal damage)\n");
                         }
                    }
                    else if (ar.ar_crc == sum_check_v41(&ar, sizeof(ar) - sizeof(ar.ar_crc))) {
                         if (ar.ar_seconds & 0x80) {
                              (void) printf("\aAuthentication code OK, but illegal wing/tyre settings used!\n");
                         }
                         else {
                              (void) printf("Authentication code OK (V4.1 record, only checks for illegal wings/tyres)\n");
                         }
                    }
                    else if (ar.ar_crc == sum_check_old(&ar, sizeof(ar) - sizeof(ar.ar_crc))) {
                         (void) printf("Authentication code OK (old style record, no checks for illegal wings/tyres)\n");
                    }
                    else {
                         (void) printf("\aAuthentication code FAILED!\n");
                    }
                    (void) printf("\n");

                    matched1 = FALSE;
                    matched2 = FALSE;
               }
          }

          (void) fclose(fp);
     }

     return ok;
}

static void
ShowIllegalEntry(
     uchar     seconds_field            /* In  Seconds field               */
) {
     if (seconds_field & ILLEGAL_WINGS) {
          (void) printf("\tIllegal Wings\n");
     }
     if (seconds_field & ILLEGAL_TYRES) {
          (void) printf("\tIllegal Tyres\n");
     }
     if (seconds_field & ILLEGAL_30S_RELOAD) {
          (void) printf("\tGame Save/Reload or Pause >30s.\n");
     }
     else if (seconds_field & ILLEGAL_10S_RELOAD) {
          (void) printf("\tGame Save/Reload or Pause >10s.\n");
     }
     if (seconds_field & ILLEGAL_BHP) {
          (void) printf("\tIllegal BHP (reloaded save game with illegal BHP).\n");
     }
     if (seconds_field & ILLEGAL_GRASS_USED) {
          (void) printf("\tIllegal use of Grass.\n");
     }
     if (seconds_field & ILLEGAL_DAMAGE) {
          (void) printf("\tIllegal Damage Limits.\n");
     }
}

static void
ShowAuthEntry(
     AUTH_RECORD    *entry              /* In  Entry to display            */
) {
     char text[128];
     int  circuit;

     if (entry->ar_record_type == REC_TYPE_AUTH || entry->ar_record_type == REC_TYPE_AUTH_RACE) {
          circuit = CIRCUIT(entry->ar_game_mode);
          (void) printf("%s ", circuits[circuit]);
          (void) printf(" %u-%s-%u %2u:%02u:%02u - %s%s\n",
                    entry->ar_day,
                    months[entry->ar_month],
                    entry->ar_year - 1900,
                    entry->ar_hours,
                    entry->ar_minutes,
                    entry->ar_seconds & 0x3f,
                    game_mode_text[GAME_MODE(entry->ar_game_mode)],
                    entry->ar_record_type == REC_TYPE_AUTH_RACE ? " (Elapsed Time Authentication)" : ""
               );

          sprint_lap_time(text, entry->ar_lap_time);
          (void) strcat(text, " ");
          (void) strcat(text, (char *) entry->ar_driver);
          (void) strcat(text, " ");

          (void) sprintf(&text[strlen(text)], "BHP:%u AIG:%u %s",
                    entry->ar_bhp,
                    entry->ar_ai_grip,
                    skill[entry->ar_skill]
               );

          if (entry->ar_drv_aids & DRV_AID_F1) {
               (void) strcat(text, ",F1");
          }
          if (entry->ar_drv_aids & DRV_AID_F2) {
               (void) strcat(text, ",F2");
          }
          if (entry->ar_drv_aids & DRV_AID_F3) {
               (void) strcat(text, ",F3");
          }
          if (entry->ar_drv_aids & DRV_AID_F4) {
               (void) strcat(text, ",F4");
          }
          if (entry->ar_drv_aids & DRV_AID_F5) {
               (void) strcat(text, ",F5");
          }
          if (entry->ar_drv_aids & DRV_AID_F6) {
               (void) strcat(text, ",F6");
          }
          if (entry->ar_drv_aids & DRV_AID_STEERING) {
               (void) strcat(text, ",S");
          }
          if (entry->ar_drv_aids & DRV_AID_TRACTION) {
               (void) strcat(text, ",T");
          }

          (void) strcat(text, "\n");
          (void) printf(text);
     }
}

static void
sprint_lap_time(
     char   *s,
     ulong  tim
) {
     ulong  thous;
     ulong  secs;
     ulong  mins;
     ulong  hours;

     if (tim & 0xe0000000U) {
          (void) sprintf(s, "-:--.---");
          return;
     }

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

static void
hex_pack(
     void           *dst,               /* Out Destination packed buffer   */
     void const     *src,               /* In  Source unpacked buffer      */
     ushort         unpacked_len        /* In  Length of unpacked buffer   */
) {
     uchar          *d = dst;
     uchar const    *s = src;
     uchar          c;

     for (; unpacked_len != 0; unpacked_len--) {
          c = *s++;
          if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
               if (isdigit(c)) {
                    c -= '0';
               }
               else {
                    c -= 'A';
                    c += 10;
               }

               if ((unpacked_len & 1) == 0) {
                    *d = (uchar) (c << 4);
               }
               else {
                    *d++ |= c;
               }
          }
          else {
               (void) printf("authlap: Error, non-hex character in auth string.\n");
               exit(1);
          }
     }
}

static ushort
sum_check_old(
     void const     *ptr,               /* In  Data to sum-check           */
     ushort         length              /* In  Number of bytes             */
) {
     uchar const    *p = ptr;
     uchar          a  = 0;
     uchar          b  = 0;
     ushort         r;

     while (length--) {
          a += *p++;
          b += a;
     }

     r = (a << 8) | b;
     r += 0x544b;

     return r;
}

static ushort
sum_check_v41(
     void const     *ptr,               /* In  Data to sum-check           */
     ushort         length              /* In  Number of bytes             */
) {
     uchar const    *p = ptr;
     uchar          a  = 0;
     uchar          b  = 0;
     ushort         r;

     while (length--) {
          a += *p++;
          b += a;
     }

     r = (a << 8) | b;
     r += 0x4b54;

     return r;
}

static ushort
sum_check_v50(
     void const     *ptr,               /* In  Data to sum-check           */
     ushort         length              /* In  Number of bytes             */
) {
     uchar const    *p = ptr;
     uchar          a  = 0;
     uchar          b  = 0;
     ushort         r;

     while (length--) {
          a += *p++;
          b += a;
     }

     r = (a << 8) | b;
     r += 0x746b;

     return r;
}

static ushort
sum_check_v60(
     void const     *ptr,               /* In  Data to sum-check           */
     ushort         length              /* In  Number of bytes             */
) {
     uchar const    *p = ptr;
     uchar          a  = 0;
     uchar          b  = 0;
     ushort         r;

     while (length--) {
          a += *p++;
          b += a;
     }

     r = (a << 8) | b;
     r += 0x6060;

     return r;
}

/*---------------------------------------------------------------------------
** End of File
*/

