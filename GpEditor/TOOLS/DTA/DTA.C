/*
** File   : dta.c
** Author : TK 
** Date   : 28/01/94
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

#define VERSION     "V1.0 28/01/94"

static const char sccsid[] = "@(#)" __FILE__ "\t" VERSION;

/*---------------------------------------------------------------------------
** Includes
*/

#include <stdio.h>

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define F1ED_DTA_V12_FILE_SIZE          1494
#define F1ED_DTA_V13_FILE_SIZE          2360

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct {
     char           team_name[13];
     unsigned short car_power;
     char           driver1_name[24];
     char           driver2_name[24];
     char           driver1_qualifying;
     char           driver2_qualifying;
     char           driver1_race;
     char           driver2_race;
     char           car_colours[16];
} F1ED_DTA_V12_ENTRY;

typedef struct {
     char           team_name[13];
     char           unknown1[1];
     unsigned short car_power;
     char           car_colours[16];
     char           driver1_name[24];
     char           driver2_name[24];
     char           car1_number;
     char           car2_number;
     char           driver1_qualifying;
     char           driver2_qualifying;
     char           driver1_race;
     char           driver2_race;
     char           unknown2[32];
} F1ED_DTA_V13_ENTRY;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

void dta_v12(void);
void dta_v13(void);

/*---------------------------------------------------------------------------
** Data
*/

static F1ED_DTA_V12_ENTRY entries_v12[20];
static F1ED_DTA_V13_ENTRY entries_v13[20];

/*---------------------------------------------------------------------------
** Functions
*/

void
main(
     void
) {
     dta_v12();
     dta_v13();
}

void
dta_v12(
     void
) {
     FILE      *fp;
     short     r;
     char      c;

     fp = fopen("v12.dta", "rb");
     if (fp == NULL) {
          printf("Can't open V12.DTA file.\n");
          return;
     }

     r = fread(entries_v12, 1, sizeof(entries_v12), fp);
     if (r != F1ED_DTA_V12_FILE_SIZE) {
          printf("Failed to read enough data from V12.DTA\n");
          return;
     }

     if (fread(&c, 1, 1, fp) != 0) {
          printf("Too much data in V12.DTA\n");
          return;
     }

     fclose(fp);
}

void
dta_v13(
     void
) {
     FILE      *fp;
     short     r;
     char      c;

     fp = fopen("v13.dta", "rb");
     if (fp == NULL) {
          printf("Can't open V13.DTA file.\n");
          return;
     }

     r = fread(entries_v13, 1, sizeof(entries_v13), fp);
     if (r != F1ED_DTA_V13_FILE_SIZE) {
          printf("Failed to read enough data from V13.DTA\n");
          return;
     }

     if (fread(&c, 1, 1, fp) != 0) {
          printf("Too much data in V13.DTA\n");
          return;
     }

     fclose(fp);
}

/*---------------------------------------------------------------------------
** End of File
*/

