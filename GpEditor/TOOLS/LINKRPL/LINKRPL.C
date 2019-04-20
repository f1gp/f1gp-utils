/*
** File   : linkrpl.c
** Author : TK
** Date   : 12/04/94
**
** $Header$
**
** Modify linked MPS GP replay files so they work on single machine mode.
*/

/*****************************************************************************
**                                                                          **
** This program is the confidential and proprietary product of Airtech      **
** Computer Security Limited. Any unauthorised use, reproduction or         **
** transfer of this program is strictly prohibited. Copyright 1993          **
** Airtech Computer Security Limited. (Subject to limited distribution      **
** and restricted disclosure only.) All rights reserved.                    **
**                                                                          **
*****************************************************************************/

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

#define VERSION     "V1.0 12/04/94"

#define MAX_FILE_SIZE         30000

#define LINKED_REPLAY_SIZE    26738

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

static char    title_msg[] = "@(#)"
                             "LinkRpl " VERSION " - Grand Prix/World Circuit Reply Modifier.\n"
                             "Copyright (c) T.Kellaway (CIS:100331,2330) 1994 - All Rights Reserved.\n\n";

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

     (void) printf(title_msg);

     if (argc != 2) {
          (void) printf("Usage: linkrpl (filename)\n");
          exit(1);
     }

     if ((fp = fopen(argv[1], "rb+")) == NULL) {
          (void) printf("linkrpl: unable to open file '%s'\n", argv[1]);
          exit(1);
     }

     cnt = fread(buffer, 1, sizeof(buffer), fp);

     if (cnt <= (sizeof(crc1) + sizeof(crc2))) {
          (void) printf("linkrpl: File is too small!\n");
          exit(1);
     }

     if (cnt >= sizeof(buffer) - (sizeof(crc1) + sizeof(crc2))) {
          (void) printf("linkrpl: File is too large for buffer!\n");
          exit(1);
     }

     if (cnt != LINKED_REPLAY_SIZE) {
          (void) printf("linkrpl: This isn't a linked replay save file!\n");
          exit(1);
     }

     (void) fclose(fp);

     /*
     ** Reduce file size by 2.
     */
     cnt -= 2;

     crc1 = 0;
     crc2 = 0;
     for (i = 0; i < cnt - (sizeof(crc1) + sizeof(crc2)); i++) {
          crc1 += buffer[i];
          rotate_left(&crc2);
          crc2 += buffer[i];
     }


     if ((fp = fopen(argv[1], "wb+")) == NULL) {
          (void) printf("linkrpl: unable to open file '%s' for writing.\n", argv[1]);
          exit(1);
     }

     if (fwrite(buffer, 1, cnt, fp) != cnt) {
          (void) printf("linkrpl: write to file '%s' failed.\n", argv[1]);
          exit(1);
     }

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
          (void) printf("Linked replay file changed to single replay OK.\n");
     }
     else {
          (void) printf("linkrpl: Failed to update CRCs.\n");
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