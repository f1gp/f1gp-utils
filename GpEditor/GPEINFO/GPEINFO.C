/*
** File   : gpeinfo.c
** Author : TK 
** Date   : 30/05/94
**
** Display GPE information.
**
** V1.0 30/05/94    First version.
** V2.0 --------    Skipped.
** V3.0 04/06/94    Added player's BHP.
** V4.0 25/09/94    Incorporated as part of the standard release of GPed.
** V4.1 23/06/95    Updated version number to keep in step with GpEditor.
*/

#define VERSION     "V4.1 23-Jun-95"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE                            1
#define FALSE                           0

#define GPE_HEADER_MAGIC                0x2a455047L    /* 'GPE*' */
#define GPE_RECORD_MAGIC                0x4b54

#define GPE_RT_EOF                      1
#define GPE_RT_COMMENT                  2
#define GPE_RT_EXE_CAR_COLOURS          3
#define GPE_RT_EXE_HELMET_COLOURS       4
#define GPE_RT_EXE_CAR_POWER            5
#define GPE_RT_EXE_CAR_NUMBERS          6
#define GPE_RT_EXE_DRIVER_QUALIFYING    7
#define GPE_RT_EXE_DRIVER_RACE          8
#define GPE_RT_GAME_NAMES               9
#define GPE_RT_EXE_AI_CAR_GRIP         10
#define GPE_RT_EXE_PLAYERS_BHP         11

#define GP_EXE_NUM_TEAMS                20
#define GP_EXE_NUM_DRIVERS              (GP_EXE_NUM_TEAMS * 2)

#define NORMAL_OFFSET         0x4000
#define OFFSET_TO_GRIP(v)     ((((v) - NORMAL_OFFSET) / 100) + 1)

#define MEM_VAL_TO_BHP(mval)  (((mval) - 632U) / 22U)

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef int bool;

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

/*
** GPE file format.
*/

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

typedef struct {
     unsigned char  number;
     char           driver[24];
     unsigned char  qualifying;
     unsigned char  race;
     char           team[13];
     char           engine[13];
     unsigned short power;
} ENTRY;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void Usage(void);
static int compare(const void *p1, const void *p2);
static const char * surname(const char *name);

/*---------------------------------------------------------------------------
** Data
*/

static char    title_msg[] = "@(#)"
                             "GpeInfo " VERSION " - GPeditor Export File Info.\n"
                             "Copyright (c) Trevor Kellaway (CIS:100331,2330) 1994 - All Rights Reserved.\n\n";

static GP_NAMES                    names;
static GP_EXE_CAR_POWER            car_power[GP_EXE_NUM_TEAMS];
static GP_EXE_CAR_NUMBER           car_numbers[GP_EXE_NUM_DRIVERS];
static GP_EXE_DRIVER_QUALIFYING    driver_qualifying[GP_EXE_NUM_DRIVERS];
static GP_EXE_DRIVER_RACE          driver_race[GP_EXE_NUM_DRIVERS];
static GPE_REC_AI_CAR_GRIP         grip;
static GPE_REC_PLAYERS_BHP         players_bhp;

static ENTRY                       entries[GP_EXE_NUM_DRIVERS];

static bool    fOptionN;
static bool    fOptionB;
static bool    fOptionD;
static bool    fOptionQ;
static bool    fOptionR;
static bool    fOptionT;

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE           *fp;
     char           *filename = NULL;
     GPE_HEADER     hdr;
     GPE_RECORD     rec;
     bool           done;
     void           *mem;
     char           *p;
     unsigned short i;
     unsigned char  cn;
     ENTRY          *ep;
     char           option;

     for (--argc, ++argv; argc; argc--, argv++) {
          if (**argv == '-' || **argv == '/') {
               while (*++*argv) {
                    option = (isupper(**argv)) ? tolower(**argv) : **argv;
                    switch (option) {
                    case 'b': fOptionB = TRUE;    break;
                    case 'n': fOptionN = TRUE;    break;
                    case 'd': fOptionD = TRUE;    break;
                    case 'q': fOptionQ = TRUE;    break;
                    case 'r': fOptionR = TRUE;    break;
                    case 't': fOptionT = TRUE;    break;
                    
                    case 'h':
                    case '?':
                         Usage();
                         break;

                    default:
                         (void) printf("gpinfo: unknown option '-%c'.\n", **argv);
                         Usage();
                         break;
                    }
               }
          }
          else {
               filename = *argv;
          }
     }

     if (filename == NULL) {
          Usage();
     }

     if ((fp = fopen(filename, "rb")) == NULL) {
          (void) fprintf(stderr, "gpeinfo: Unable to open file '%s'\n", filename);
          return 1;
     }

     if (fread(&hdr, sizeof(hdr), 1 ,fp) != 1 || hdr.magic != GPE_HEADER_MAGIC) {
          (void) fprintf(stderr, "gpeinfo: '%s' isn't a GPE file.\n", filename);
          return 1;
     }

     (void) printf("%s - (GPeditor V%u.%u) %s\n",  filename,
                                   hdr.gpeditor_version_major,
                                   hdr.gpeditor_version_minor,
                                   ctime((long *) &hdr.time)
                              );

     /*
     ** Accumulate all data first.
     */
     (void) memset(&names, 0x00, sizeof(names));
     (void) memset(car_power, 0x00, sizeof(car_power));
     (void) memset(car_numbers, 0x00, sizeof(car_numbers));
     (void) memset(driver_qualifying, 0x00, sizeof(driver_qualifying));
     (void) memset(driver_race, 0x00, sizeof(driver_race));
     (void) memset(&grip, 0x00, sizeof(grip));
     (void) memset(entries, 0x00, sizeof(entries));

     for (done = FALSE; !done;) {
          if (fread(&rec, sizeof(rec), 1 ,fp) == 1) {
               if (rec.magic != GPE_RECORD_MAGIC) {
                    (void) fprintf(stderr, "gpeinfo: '%s' is a corrupt GPE file.\n", filename);
                    return 1;
               }

               if (rec.size) {
                    mem = malloc(rec.size + 1);
                    if (mem == NULL) {
                         (void) fprintf(stderr, "gpeinfo: out of memory.\n");
                         return 1;
                    }

                    if (fread(mem, rec.size, 1, fp) != 1) {
                         (void) fprintf(stderr, "gpeinfo: Failed to read from '%s'\n", filename);
                         return 1;
                    }
               }
               else {
                    mem = NULL;
               }

               switch (rec.type) {
               case GPE_RT_EOF:
                    break;

               case GPE_RT_COMMENT:
                    (void) printf("Comment:\n");
                    p = (char *) mem;
                    p[rec.size] = '\0';
                    while (*p != '\0') {
                         if (*p != '\r') {
                              (void) putchar(*p);
                         }
                         ++p;
                    }
                    (void) printf("\n\n");
                    break;

               case GPE_RT_EXE_CAR_COLOURS:
                    break;

               case GPE_RT_EXE_HELMET_COLOURS:
                    break;

               case GPE_RT_EXE_CAR_POWER:
                    (void) memcpy(car_power, mem, sizeof(car_power));
                    break;

               case GPE_RT_EXE_DRIVER_QUALIFYING:
                    (void) memcpy(driver_qualifying, mem, sizeof(driver_qualifying));
                    break;

               case GPE_RT_EXE_DRIVER_RACE:
                    (void) memcpy(driver_race, mem, sizeof(driver_race));
                    break;

               case GPE_RT_EXE_CAR_NUMBERS:
                    (void) memcpy(car_numbers, mem, sizeof(car_numbers));
                    break;

               case GPE_RT_GAME_NAMES:
                    (void) memcpy(&names, mem, sizeof(names));
                    break;

               case GPE_RT_EXE_AI_CAR_GRIP:
                    (void) memcpy(&grip, mem, sizeof(grip));
                    (void) printf("AI Grip:%u\n\n", OFFSET_TO_GRIP(grip.grip));
                    break;

               case GPE_RT_EXE_PLAYERS_BHP:
                    (void) memcpy(&players_bhp, mem, sizeof(players_bhp));
                    (void) printf("Player's BHP:%u\n\n", MEM_VAL_TO_BHP(players_bhp.bhp));
                    break;

               default:
                    break;
               }

               if (mem) {
                    free(mem);
                    mem = NULL;
               }

          }
          else {
               done = TRUE;
          }

     }


     /*
     ** Copy data into consistantly numbered structure.
     */
     ep = entries;
     for (i = 0; i < GP_EXE_NUM_DRIVERS; i++, ep++) {
          cn = car_numbers[i].number;
          if (cn) {
               --cn;
               ep->number      = cn + 1;
               ep->qualifying  = driver_qualifying[cn].qualifying;
               ep->race        = driver_race[cn].race;
               ep->power       = car_power[i / 2].power;
               (void) memcpy(ep->driver, names.drivers[cn].driver, sizeof(ep->driver));
               (void) memcpy(ep->team, names.teams[i / 2].team, sizeof(ep->team));
               (void) memcpy(ep->engine, names.engines[i / 2].engine, sizeof(ep->engine));
          }
     }

     /*
     ** Sort it if requested.
     */
     if (fOptionN || fOptionB || fOptionD || fOptionQ || fOptionR || fOptionT) {
          qsort(entries, GP_EXE_NUM_DRIVERS, sizeof(entries[0]), compare);
     }

     /*
     ** Display it.
     */
     (void) printf("  Car  Driver                    Q   R  Team          Engine        BHP\n");
     (void) printf(" -----------------------------------------------------------------------\n");

     ep = entries;
     for (i = 0; i < GP_EXE_NUM_DRIVERS; i++, ep++) {
          if (ep->number) {
               (void) printf("  #%-2u  %-24s %2u  %2u  %-13s %-13s %3u\n",
                         ep->number,
                         ep->driver,
                         ep->qualifying,
                         ep->race,
                         ep->team,
                         ep->engine,
                         ep->power
                    );
          }
     }

     (void) fclose(fp);
     return 0;
}

static void
Usage(
     void
) {
     (void) printf(&title_msg[4]);
     (void) printf("Usage: gpeinfo [-h?bdnqrt] (filename)\n");
     (void) printf("       -h | -?   This usage message.\n");
     (void) printf("       -b        Sort by car power (BHP).\n");
     (void) printf("       -d        Sort by driver name.\n");
     (void) printf("       -n        Sort by car number.\n");
     (void) printf("       -q        Sort by qualifying probability.\n");
     (void) printf("       -r        Sort by race probability.\n");
     (void) printf("       -t        Sort by team name.\n");
     exit(1);
}

static int
compare(
     const void     *p1,
     const void     *p2
) {
     ENTRY const    *ep1 = p1;
     ENTRY const    *ep2 = p2;
     int            r;

     /*
     ** Car Number (unique).
     */
     if (fOptionN) {
          return ((ep1->number > ep2->number) ? 1 : -1);
     }

     /*
     ** Car Power, driver number.
     */
     if (fOptionB) {
          if (ep1->power == ep2->power) {
               return ((ep1->number > ep2->number) ? 1 : -1);
          }
          return ((ep1->power < ep2->power) ? 1 : -1);
     }

     /*
     ** Driver surname, given name.
     */
     if (fOptionD) {
          r = strcmp(surname(ep1->driver), surname(ep2->driver));
          if (r == 0) {
               return strcmp(ep1->driver, ep2->driver);
          }
          return r;
     }

     /*
     ** Team name, car number.
     */
     if (fOptionT) {
          r = strcmp(ep1->team, ep2->team);
          if (r == 0) {
               return ((ep1->number > ep2->number) ? 1 : -1);
          }
          return r;
     }

     /*
     ** Qualifying, power, car number.
     */
     if (fOptionQ) {
          if (ep1->qualifying == ep2->qualifying) {
               if (ep1->power == ep2->power) {
                    return ((ep1->number > ep2->number) ? 1 : -1);
               }
               return ((ep1->power < ep2->power) ? 1 : -1);
          }
          return ((ep1->qualifying > ep2->qualifying) ? 1 : -1);
     }

     /*
     ** Race, power, car number.
     */
     if (fOptionR) {
          if (ep1->race == ep2->race) {
               if (ep1->power == ep2->power) {
                    return ((ep1->number > ep2->number) ? 1 : -1);
               }
               return ((ep1->power < ep2->power) ? 1 : -1);
          }
          return ((ep1->race > ep2->race) ? 1 : -1);
     }

     return 0;
}

static const char *
surname(
     const char *name
) {
     const char *p = strrchr(name, ' ');

     if (p == NULL) {
          p = name;
     }

     return p;
}

/*---------------------------------------------------------------------------
** End of File
*/

