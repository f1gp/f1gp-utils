/*
** File   : flags.c
** Author : TK
** Date   : 30/10/94
**
** $Header:   F:/TK/GPEDITOR/TOOLS/GPFLAGS/VCS/GPFLAGS.C__   1.5   31 Dec 1995 13:57:32   tk  $
**
** Patch V1.05 of GP.EXE for yellow flags.
**
** V1.0   18-Jun-95  First bash.
** V1.1   23-Jun-95  Added filename argument on command line.
** V2.0   11-Nov-95  Added variable delay arguments.
** V2.1   31-Dec-95  Increased default first delay to 5s.
*/

#define VERSION     "V2.1 31-Dec-95"

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

#define TRUE             1
#define FALSE            0

#define NUM_ELEMENTS(x)       (sizeof(x) / sizeof(x[0]))

#define DEFAULT_DELAY_FLAGS   5
#define DEFAULT_DELAY_CAR     30

/*
** Note we are limited to a max of 65s (65000ms) due to word value limit.
*/
#define MIN_DELAY_FLAGS       1
#define MAX_DELAY_FLAGS       60
#define MIN_DELAY_CAR         10
#define MAX_DELAY_CAR         60

#define PATCH_INDEX_FLAGS     0
#define PATCH_INDEX_CAR       1

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef int bool;

typedef struct {
     long           offset;
     unsigned short value;
} PATCH_RECORD;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void Usage(void);

/*---------------------------------------------------------------------------
** Data
*/

static char    title_msg[] = "@(#)"
                             "GpFlags " VERSION " - Grand Prix/World Circuit Yellow Flags Patch.\n"
                             "Copyright (c) T.Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.\n\n";

static PATCH_RECORD const patches_old[] = {
     {    0x005bddL,     0x4e20    },        // 4e20h (20s)
     {    0x005c4fL,     0x3a98    },        // 3a98h (15s)
};

static PATCH_RECORD patches_new[] = {
     {    0x005bddL,     DEFAULT_DELAY_FLAGS * 1000    },
     {    0x005c4fL,     DEFAULT_DELAY_CAR   * 1000    },
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
     unsigned short i;
     unsigned short w;
     PATCH_RECORD   const *patches = NULL;
     PATCH_RECORD   const *pr;
     char           *filename      = "GP.EXE";
     short          delay_flags    = DEFAULT_DELAY_FLAGS;
     short          delay_car      = DEFAULT_DELAY_CAR;
     bool           o_flag         = FALSE;
     bool           p_flag         = FALSE;
     bool           f_flag         = FALSE;
     bool           c_flag         = FALSE;

     (void) printf(&title_msg[4]);

     for (--argc, ++argv; argc; argc--, argv++) {
          if (argv[0][0] == '-' || argv[0][0] == '/') {
               switch (argv[0][1]) {
               case 'o':
               case 'O':
                    o_flag = TRUE;
                    break;

               case 'p':
               case 'P':
                    p_flag = TRUE;
                    break;

               case 'f':
               case 'F':
                    f_flag = TRUE;
                    delay_flags = atoi(&argv[0][2]);
                    if (delay_flags < MIN_DELAY_FLAGS || delay_flags >  MAX_DELAY_FLAGS) {
                         (void) printf( "GpFlags: Flags delay should be between %u and %u.\n",
                                        MIN_DELAY_FLAGS,
                                        MAX_DELAY_FLAGS);
                         Usage();
                    }
                    patches_new[PATCH_INDEX_FLAGS].value = (unsigned short) delay_flags * 1000U;
                    break;

               case 'c':
               case 'C':
                    c_flag = TRUE;
                    delay_car = atoi(&argv[0][2]);
                    if (delay_car < MIN_DELAY_CAR || delay_car >  MAX_DELAY_CAR) {
                         (void) printf( "GpFlags: Car removal delay should be between %u and %u.\n",
                                        MIN_DELAY_CAR,
                                        MAX_DELAY_CAR);
                         Usage();
                    }
                    patches_new[PATCH_INDEX_CAR].value = (unsigned short) delay_car * 1000U;
                    break;

               case '?':
               case 'h':
               default:
                    Usage();
                    break;
               }
          }
          else {
               filename = argv[0];
          }
     }

     if (o_flag && p_flag) {
          (void) printf("GpFlags: Specify only one of -o and -p.\n");
          Usage();
     }

     if (o_flag) {
          patches = patches_old;

          if (f_flag || c_flag) {
               (void) printf("GpFlags: Only use -fN and -cN with the -p option.\n");
               Usage();
          }
     }
     else if (p_flag || f_flag || c_flag) {
          p_flag  = TRUE;
          patches = patches_new;
     }
     else {
          Usage();
     }

     fp = fopen(filename, "rb+");
     if (fp == NULL) {
          (void) printf("GpFlags: Unable to open %s (ensure it exists & is writeable).\n", filename);
          return 1;
     }

     /*
     ** Perform all patches.
     */
     /*lint -e506 Ignore constant boolean */
     assert(NUM_ELEMENTS(patches_old) == NUM_ELEMENTS(patches_new));
     /*lint +e506 Ignore constant boolean */

     for (i = 0, pr = patches; i < NUM_ELEMENTS(patches_new); i++, pr++) {
          if (fseek(fp, pr->offset, SEEK_SET) != 0) {
               (void) printf("GpFlags: Seek failed\n");
               return 1;
          }

          w = pr->value;
          if (fwrite(&w, sizeof(w), 1, fp) != 1) {
               (void) printf("GpFlags: Write failed\n");
               return 1;
          }
     }

     (void) fclose(fp);

     if (p_flag) {
          (void) printf("Patch Completed OK. Yellow flags and car removal modified.\n");
     }
     else {
          (void) printf("Patch Completed OK. Original code restored.\n");
     }
     return 0;
}

static void
Usage(
     void
) {
     (void) printf( "\n"
                    "Usage: GpFlags {-o | {-p [-fN -cN]}} [-?h] [GP.EXE]\n"
                    "       -o     Restore original code.\n"
                    "       -p     Patch new code (default -f%u -c%u).\n"
                    "         -fN  Yellow flags delay (%u to %u).\n"
                    "         -cN  Car removal delay (%u to %u).\n",
                    DEFAULT_DELAY_FLAGS,
                    DEFAULT_DELAY_CAR,
                    MIN_DELAY_FLAGS,    MAX_DELAY_FLAGS,
                    MIN_DELAY_CAR,      MAX_DELAY_CAR
               );
     exit(1);
}

/*---------------------------------------------------------------------------
** End of File
*/

