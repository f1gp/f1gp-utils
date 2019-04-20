/*
** File   : gpwet.c
** Author : TK
** Date   :  9/12/95
**
** $Header:   F:/TK/GPEDITOR/TOOLS/GPWET/VCS/GPWET.C__   1.1   17 Dec 1995 15:16:36   tk  $
**
** Wet Race for MPS Grand Prix V1.05.
**
** V1.0 09/12/95    First bash.
** V1.1 17/12/95    Overrides "no wet race" flag in game track files (Phoenix).
*/

#define VERSION     "V1.1 17-Dec-95"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <stdlib.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE        1
#define FALSE       0

/*
** European, US and Italian GP.EXE (V1.05)
**
** 00E412  0E             push   cs
** 00E413  E8C122         call   0106D7
** 00E416  25FF00         and    ax,0FF
** 00E419  3DF000         cmp    ax,0F0
** 00E41C  7D05           jge    00E423
** 00E41E  E91100         jmp    00E432
** 00E421  90             nop
** 00E422  90             nop
** 00E423  EB14           jmps   00E439
** 00E425  81BE2E120000   cmp    word ptr [bp+122E],0
** 00E42B  740C           jz     00E439
** 00E42D  E90200         jmp    00E432
** 00E430  90             nop
** 00E431  90             nop
** 00E432  C7862E120000   mov    word ptr [bp+122E],0
** 00E438  CB             retf
** 00E439  C7862E126000   mov    word ptr [bp+122E],60
** 00E43F  CB             retf
*/
#define GP_PROBABILITY_LOCATION         0x00e41aL

#define COMPARE_DEFAULT                 0x00f0
#define COMPARE_MAX                     0x0100


/*
** 019F4A  F70674010040  test    word ptr [0174],4000
*/
#define GP_TRACK_TEST_LOCATION          0x019f4fL

#define GP_TRACK_TEST_DISABLED          0x00
#define GP_TRACK_TEST_DEFAULT           0x40


#define MIN_PERCENT                     0
#define MAX_PERCENT                     100

#define VALUE_TO_PERCENT(x)             ((COMPARE_MAX - (x)) * 100 / COMPARE_MAX)
#define PERCENT_TO_VALUE(x)             (COMPARE_MAX - ((x) * COMPARE_MAX / 100))

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef int    bool;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void Usage(void);

/*---------------------------------------------------------------------------
** Data
*/

static char    title_msg[] = "@(#)"
                             "GpWet " VERSION " - Modify Wet Race Probability for Grand Prix/World Circuit.\n"
                             "Copyright (c) Trevor Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.\n\n";

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE           *fp;
     char           *filename      = "GP.EXE";
     bool           o_flag         = FALSE;
     bool           p_flag         = FALSE;
     short          v;
     unsigned short probability    = 0;
     unsigned short compare_value  = COMPARE_DEFAULT;
     unsigned char  track_test     = GP_TRACK_TEST_DEFAULT;

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
                    v = atoi(&argv[0][2]);
                    if (v < MIN_PERCENT || v >  MAX_PERCENT) {
                         (void) printf( "GpWet: Probability value should be between %u and %u.\n",
                                        MIN_PERCENT,
                                        MAX_PERCENT);
                         Usage();
                    }
                    probability = (unsigned char) v;
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
          (void) printf("GpWet: Specify only one of -o and -p.\n");
          Usage();
     }

     fp = fopen(filename, "rb+");
     if (fp == NULL) {
          (void) printf("GpWet: Unable to open %s (ensure it exists & is writeable).\n", filename);
          return 1;
     }

     /*
     ** Perform all patches.
     */
     if (fseek(fp, GP_PROBABILITY_LOCATION, SEEK_SET) != 0) {
          (void) printf("GpWet: Seek failed\n");
          return 1;
     }

     if (!o_flag && !p_flag) {
          if (fread(&compare_value, sizeof(compare_value), 1, fp) != 1) {
               (void) printf("GpWet: Read failed\n");
               return 1;
          }
     }
     else {
          if (p_flag) {
               compare_value = PERCENT_TO_VALUE(probability);
          }
          else {
               compare_value  = COMPARE_DEFAULT;
               probability    = VALUE_TO_PERCENT(compare_value);
          }
          if (fwrite(&compare_value, sizeof(compare_value), 1, fp) != 1) {
               (void) printf("GpWet: Write failed\n");
               return 1;
          }

          if (fseek(fp, GP_TRACK_TEST_LOCATION, SEEK_SET) != 0) {
               (void) printf("GpWet: Seek failed\n");
               return 1;
          }

          if (p_flag) {
               track_test = GP_TRACK_TEST_DISABLED;
          }
          else {
               track_test = GP_TRACK_TEST_DEFAULT;
          }
          if (fwrite(&track_test, sizeof(track_test), 1, fp) != 1) {
               (void) printf("GpWet: Write failed\n");
               return 1;
          }
     }

     (void) fclose(fp);

     if (p_flag) {
          (void) printf("New wet race probabilty of %u%% set (%04xh).\n",
                         probability,
                         compare_value
               );
     }
     else if (o_flag) {
          (void) printf("Original wet race probability of %u%% restored (%04xh).\n",
                         probability,
                         compare_value
               );
     }
     else {
          (void) printf("Current wet race probability value is %u%% (%04xh).\n",
                         VALUE_TO_PERCENT(compare_value),
                         compare_value
                    );
     }
     return 0;
}

static void
Usage(
     void
) {
     (void) printf( "\n"
                    "Usage: GpWet {-o | -p} [-?h] [GP.EXE]\n"
                    "       -o     Restore original probability (6.25%%).\n"
                    "       -p     Patch new probability (%u - %u).\n"
                    "       -h?    This usage message.\n",
                    MIN_PERCENT, MAX_PERCENT
               );
     exit(1);
}


/*---------------------------------------------------------------------------
** End of File
*/

