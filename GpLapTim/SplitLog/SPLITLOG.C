/*
** File   : splitlog.c
** Author : TK
** Date   : 28/07/94
**
** $Header:   L:/GPLAPTIM/SPLITLOG/VCS/SPLITLOG.C__   1.0   28 Jul 1994 23:32:04   tk  $
**
** Split a GPLAPTIM log into different circuits.
**
** V1.0   28/07/94  First version.
*/

#define VERSION     "V1.0 28-Jul-94"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <string.h>
/*lint +elib(???) */
#include "..\gplaptim.h"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define WHAT_OFFSET      4

#define MAX_CIRCUITS     16

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef union {
     LAP_RECORD     lr;
     AUTH_RECORD    ar;
     SETUP_RECORD   sr;
     PIT_IN_RECORD  pi;
     PIT_OUT_RECORD po;
} RECORDS;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void Usage(void);

/*---------------------------------------------------------------------------
** Data
*/

static char    title_msg[] = "@(#)"
                             "SplitLog " VERSION " - Split GPLAPTIM.LOG into different circuit logs.\n"
                             "Copyright (c) T.Kellaway (CIS:100331,2330) 1994 - All Rights Reserved.\n\n";

static char *circuits[MAX_CIRCUITS] = {
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
};

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     RECORDS   rec;
     FILE      *ifp;
     FILE      *ofp;
     char      *ip_name  = "GPLAPTIM.LOG";
     char      buff[80];
     short     circuit   = -1;
     short     last      = -1;
     short     count     = 0;

     (void) printf(&title_msg[WHAT_OFFSET]);     

     if (argc > 1) {
          if (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "-h") == 0) {
               Usage();
               return -1;
          }
          ip_name = argv[1];
     }

     if ((ifp = fopen(ip_name, "rb")) == NULL) {
          (void) fprintf(stderr, "SplitLog: unable to open input file '%s'\n", ip_name);
          return -1;
     }

     while (fread(&rec, sizeof(rec), 1, ifp) == 1) {
          switch (rec.lr.lr_record_type) {
          case REC_TYPE_LAP:
               circuit = CIRCUIT(rec.lr.lr_game_mode);
               break;

          case REC_TYPE_AUTH:
               circuit = CIRCUIT(rec.ar.ar_game_mode);
               break;

          case REC_TYPE_SETUP:
               circuit = CIRCUIT(rec.sr.sr_game_mode);
               break;

          case REC_TYPE_PIT_IN:
               circuit = CIRCUIT(rec.pi.pi_game_mode);
               break;

          case REC_TYPE_PIT_OUT:
               circuit = CIRCUIT(rec.po.po_game_mode);
               break;

          default:
               break;
          }

          if (circuit >= 0 && circuit < MAX_CIRCUITS) {
               (void) sprintf(buff, "TRACK%02u.LOG", circuit);

               if (circuit != last) {
                    last = circuit;
                    (void) printf("\n%s - %s\n", buff, circuits[circuit]);
               }

               if ((ofp = fopen(buff, "ab")) == NULL) {
                    (void) fseek(ofp, 0L, SEEK_END);
                    (void) fprintf(stderr, "\nSplitLog: Unable to open/create file '%s'\n", buff);
                    return -1;
               }
               (void) fwrite(&rec, sizeof(rec), 1, ofp);
               (void) fclose(ofp);

               if (++count > 10) {
                    (void) printf(".");
                    count = 0;
               }
          }
     }
     (void) printf("\n");

     (void) fclose(ifp);

     return 0;
}

static void
Usage(
     void
) {
     short     i;

     (void) printf("Usage: SplitLog [-h|-?] [GPLAPTIM.LOG]\n");
     (void) printf("\n");
     (void) printf("Filename/Tracks\n");
     for (i = 0; i < MAX_CIRCUITS; i++) {
          (void) printf("TRACK%02u.LOG - %s\n", i, circuits[i]);
     }
}
/*---------------------------------------------------------------------------
** End of File
*/

