/*
** File   : rol3crc.c
** Author : TK 
** Date   : 13/03/94
**
** $Header$
**
** Calculate (and update) CRC values for MPS GP.EXE name and circuit files. 
*/

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

#define VERSION     "V1.2 20/04/95"

#define TRUE             1
#define FALSE            0

#define MAX_FILE_SIZE    30000

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef int    bool;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void rotate_left(unsigned short *ptr_us);

/*---------------------------------------------------------------------------
** Data
*/

/*lint -esym(528,sccsid) */
static const char sccsid[] = "@(#)" __FILE__ "\t" VERSION;

static unsigned char     buffer[MAX_FILE_SIZE];

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
     unsigned short crc1;
     unsigned short crc2;
     unsigned short cnt;
     bool           ok;
     bool           update = TRUE;

     if (argc != 2) {
          if (!(argc == 3 && strcmp(argv[1], "-n") == 0)) {
               (void) printf("Usage: rol3crc [-n] (filename)\n");
               exit(1);
          }
          update = FALSE;
     }

     if ((fp = fopen(update ? argv[1] : argv[2], update ? "rb+" : "rb")) == NULL) {
          (void) printf("rol3crc: unable to open file '%s'\n", update ? argv[1] : argv[2]);
          exit(1);
     }

     cnt = fread(buffer, 1, sizeof(buffer), fp);

     if (cnt <= (sizeof(crc1) + sizeof(crc2))) {
          (void) printf("rol3crc: File is too small!\n");
          exit(1);
     }

     if (cnt >= sizeof(buffer) - (sizeof(crc1) + sizeof(crc2))) {
          (void) printf("rol3crc: File is too large for buffer!\n");
          exit(1);
     }

     crc1 = 0;
     crc2 = 0;
     for (i = 0; i < cnt - (sizeof(crc1) + sizeof(crc2)); i++) {
          crc1 += buffer[i];
          rotate_left(&crc2);
          crc2 += buffer[i];
     }

     (void) printf("CRC1: %04xH\n", crc1);
     (void) printf("CRC2: %04xH\n", crc2);

     if (update) {
          ok = (fseek(fp, (long) cnt - (sizeof(crc1) + sizeof(crc2)), SEEK_SET) == 0);
          if (ok) {
               ok = (fwrite(&crc1, sizeof(crc1), 1, fp) == 1);
          }
     
          if (ok) {
               ok = (fwrite(&crc2, sizeof(crc2), 1, fp) == 1);
          }

          if (ok) {
               ok = (fclose(fp) == 0);
          }

          if (ok) {
               (void) printf("CRCs updated OK.\n");
          }
          else {
               (void) printf("rol3crc: Failed to update CRCs.\n");
          }
     }

     return 0;
}

static void
rotate_left(
     unsigned short *ptr_us             /* In   Pointer to value to shift  */
) {
     unsigned long us = *ptr_us;

     us <<= 3;

     us |= (us >> 16) & 0x00000007U;
     *ptr_us = (unsigned short) us;
}

/*---------------------------------------------------------------------------
** End of File
*/

