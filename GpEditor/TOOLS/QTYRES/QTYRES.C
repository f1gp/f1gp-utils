/*
** File   : qtyres.c
** Author : TK
** Date   :  2/06/94
**
** $Header:   L:/GPEDITOR/TOOLS/QTYRES/VCS/QTYRES.C__   1.1   23 Jun 1995 18:49:38   tk  $
**
** Patch V1.05 of GP.EXE for Q tyres. Q tyres are now permitted in all
** types of session except for a race. Also the 4 Q tyre limit in qualifying
** is no longer present.
**
** V1.0    2/06/94       First bash.
** V1.1    23-Jun-95     Added command line support for filename.
*/

#define VERSION     "V1.1 23-Jun-95"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE             1
#define FALSE            0

#define NUM_ELEMENTS(x)       (sizeof(x) / sizeof(x[0]))

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef int bool;

typedef struct {
     long           offset;
     unsigned char  original;
     unsigned char  new;
} PATCH_RECORD;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void Usage(void);

/*---------------------------------------------------------------------------
** Data
*/

static char    title_msg[] = "@(#)"
                             "Qtyres " VERSION " - Grand Prix/World Circuit Qualifying Tyres Patch.\n"
                             "Copyright (c) T.Kellaway (CIS:100331,2330) 1995 - All Rights Reserved.\n";

static PATCH_RECORD const patches_old[] = {
     {    0x00ac18L,     0x80,     0x40      },
     {    0x00ac19L,     0x75,     0x74      },
     {    0x00ac33L,     0x40,     0x04      },
};

static PATCH_RECORD const patches_new[] = {
     {    0x00ac18L,     0x40,     0x80      },
     {    0x00ac19L,     0x74,     0x75      },
     {    0x00ac33L,     0x04,     0x40      },
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
     unsigned char  c;
     PATCH_RECORD   const *patches = NULL;
     PATCH_RECORD   const *pr;
     bool           new_code       = FALSE;
     char           *filename      = "GP.EXE";

     (void) puts(&title_msg[4]);

     if (argc < 2 || argc > 3) {
          Usage();
     }

     if (strcmp(argv[1], "-o") == 0) {
          patches = patches_old;
     }
     else if (strcmp(argv[1], "-p") == 0) {
          patches = patches_new;
          new_code = TRUE;
     }
     else {
          Usage();
     }

     if (argc == 3) {
          filename = argv[2];
     }

     fp = fopen(filename, "rb+");
     if (fp == NULL) {
          (void) printf("Qtyres: unable to open %s (ensure it exists & is writeable).\n", filename);
          return 1;
     }

     /*
     ** Check all original values exist first.
     */
     for (i = 0, pr = patches; i < NUM_ELEMENTS(patches_new); i++, pr++) {
          if (fseek(fp, pr->offset, SEEK_SET) != 0) {
               (void) puts("Qtyres: Seek failed");
               return 1;
          }

          if (fread(&c, sizeof(c), 1, fp) != 1) {
               (void) puts("Qtyres: Read failed");
               return 1;
          }

          if (c != pr->original) {
               if (c == pr->new) {
                    (void) puts("Qtyres: Patched already!");
               }
               else {
                    (void) puts("Qtyres: Expected value not present (not V1.05?).");
               }
               return 1;
          }

     }

     /*
     ** Now perform all patches.
     */
     for (i = 0, pr = patches; i < NUM_ELEMENTS(patches_new); i++, pr++) {
          if (fseek(fp, pr->offset, SEEK_SET) != 0) {
               (void) puts("Qtyres: Seek failed");
               return 1;
          }

          c = pr->new;
          if (fwrite(&c, sizeof(c), 1, fp) != 1) {
               (void) puts("Qtyres: Write failed");
               return 1;
          }
     }

     (void) fclose(fp);

     if (new_code) {
          (void) puts("Patch Completed OK. Q-tyres now enabled.");
     }
     else {
          (void) puts("Patch Completed OK. Original code restored.");
     }
     return 0;
}

static void
Usage(
     void
) {
     (void) puts("Usage: Qtyres {-o | -p} [GP.EXE]\n"
                    "       -o   Restore Original code.\n"
                    "       -p   Patch new qualifying code."
               );
     exit(1);
}

/*---------------------------------------------------------------------------
** End of File
*/

