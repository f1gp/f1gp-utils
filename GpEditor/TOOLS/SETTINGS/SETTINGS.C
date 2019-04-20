/*
** File   : settings.c
** Author : TK 
** Date   : 11/05/94
**
** $Header$
**
** 
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

#define VERSION     "V1.0 11/05/94"

/*lint -esym(528,sccsid) */
static const char sccsid[] = "@(#)" __FILE__ "\t" VERSION;

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct {
     unsigned char  front_wing;
     unsigned char  rear_wing;
     unsigned char  gear1;
     unsigned char  gear2;
     unsigned char  gear3;
     unsigned char  gear4;
     unsigned char  gear5;
     unsigned char  gear6;
     unsigned char  race_tyres;
     unsigned char  brake_bias;
} CAR_SETUP;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/

int
main(
     int  argc,
     char **argv
) {
     FILE           *fp;
     CAR_SETUP      cs;
     unsigned short i;
     unsigned short addr;

     if (argc != 2) {
          (void) printf("Usage: settings (saved game filename).\n");
          return 1;
     }

     if ((fp = fopen(argv[1], "rb")) == NULL) {
          (void) printf("settings: Unable to open file '%s'\n", argv[1]);
          return 1;
     }

     addr = 0x006a;
     fseek(fp, addr, 0);
     for (i = 0; i < 40; i++) {
          fread(&cs, sizeof(cs), 1, fp);
          (void) printf("%2u(%04xH): %02u/%02u  %c  %2u,%2u,%2u,%2u,%2u,%2u  %u\n",
                    i + 1,
                    addr,
                    cs.front_wing,
                    cs.rear_wing,
                    "ABCD"[cs.race_tyres],
                    cs.gear1,
                    cs.gear2,
                    cs.gear3,
                    cs.gear4,
                    cs.gear5,
                    cs.gear6,
                    cs.brake_bias
               );
          addr += sizeof(cs);
     }

     addr = 0x01fc;
     fseek(fp, addr, 0);
     for (i = 0; i < 40; i++) {
          fread(&cs, sizeof(cs), 1, fp);
          (void) printf("%2u(%04xH): %02u/%02u  %c  %2u,%2u,%2u,%2u,%2u,%2u  %u\n",
                    i + 1,
                    addr,
                    cs.front_wing,
                    cs.rear_wing,
                    "ABCD"[cs.race_tyres],
                    cs.gear1,
                    cs.gear2,
                    cs.gear3,
                    cs.gear4,
                    cs.gear5,
                    cs.gear6,
                    cs.brake_bias
               );
          addr += sizeof(cs);
     }

     (void) fclose(fp);

     return 0;
}

/*---------------------------------------------------------------------------
** End of File
*/

