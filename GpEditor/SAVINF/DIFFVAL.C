/*
** File   : diffval.c
** Author : TK 
** Date   : 29/10/94
**
** $Header$
**
** 
*/

#define VERSION     "V1.0 29/10/94"

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

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

/*lint -esym(528,sccsid) */
static const char sccsid[] = "@(#)" __FILE__ "\t" VERSION;

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE *fp1;
     FILE *fp2;
     int  v1;
     int  v2;
     long addr = 0L;
     char c1;
     char c2;

     if (argc != 5) {
          (void) printf("Usage: diffval (filename1) (filename2) (value1) (value2)\n");
          return 1;
     }

     if ((fp1 = fopen(argv[1], "rb")) == NULL) {
          (void) printf("diffval: can' open file called '%s'\n", argv[1]);
          return 1;
     }

     if ((fp2 = fopen(argv[2], "rb")) == NULL) {
          (void) printf("diffval: can' open file called '%s'\n", argv[2]);
          return 1;
     }

     v1 = atoi(argv[3]);
     v2 = atoi(argv[4]);

     while (fread(&c1, 1, 1, fp1) == 1 && fread(&c2, 1, 1, fp2)) {
          if (c1 == v1 && c2 == v2) {
               (void) printf("%08lxH: %3d %3d\n", addr, c1, c2);
          }
          ++addr;
     }

     (void) fclose(fp1);
     (void) fclose(fp2);

     return 0;
}

/*---------------------------------------------------------------------------
** End of File
*/

