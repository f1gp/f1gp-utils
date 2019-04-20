/*
** File   : gpefxdta.c
** Author : TK 
** Date   : 12/02/94
**
** $Header:   L:/GPEDITOR/TOOLS/GPEFXDTA/VCS/GPEFXDTA.C__   1.0   12 Feb 1994 00:56:40   tk  $
**
** Program to patch '!' back into F1ED.DTA file, for GPEDITOR V1.0
*/

#define VERSION     "V1.0 11/02/94"

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
/*lint +elib(???) */

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TRUE                            1
#define FALSE                           0

#define DTA_FILENAME                    "F1ED.DTA"

#define F1ED_DTA_V12_FILE_SIZE          1494
#define F1ED_DTA_V13_FILE_SIZE          2360

#define NUM_TEAM_ENTRIES_V12            18
#define NUM_TEAM_ENTRIES_V13            20

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef int    bool;

/*lint -e754 Ignore unused structure members  */

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

static void dta_v12(FILE *fp);
static void dta_v13(FILE *fp);

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
     void
) {
     FILE           *fp;
     struct stat    sb;

     (void) puts("GPEDITOR Patch Utility for F1ED.DTA files (" VERSION ").\n");

     if ((fp = fopen(DTA_FILENAME, "rb+")) == NULL) {
          (void) puts("gpefxdta: Unable to open 'F1ED.DTA'.\n");
          return 1;
     }

     if (stat(DTA_FILENAME, &sb) != 0) {
          (void) puts("gpefxdta: Unable to get file info for 'F1ED.DTA'.");
          return 1;
     }

     if (sb.st_size == F1ED_DTA_V12_FILE_SIZE) {
          dta_v12(fp);
     }
     else if (sb.st_size == F1ED_DTA_V13_FILE_SIZE) {
          dta_v13(fp);
     }
     else {
          (void) puts("gpefxdta: 'F1ED.DTA' isn't a V1.2 or V1.3 DTA file.");
          return 1;
     }

     (void) fclose(fp);
     return 0;
}

static void
dta_v12(
     FILE *fp
) {
     F1ED_DTA_V12_ENTRY  entries[NUM_TEAM_ENTRIES_V12];
     unsigned int        i;
     bool                different = FALSE;

     if (fread(entries, sizeof(entries), 1, fp) != 1) {
          (void) puts("gpefxdta: Failed to read enough data from V1.2 'F1ED.DTA'.");
          return;
     }

     for (i = 0; i < NUM_TEAM_ENTRIES_V12; i++) {
          if (entries[i].car_colours[0] != '!') {
               different = TRUE;
          }
     }

     if (different) {
          (void) puts("This V1.2 'F1ED.DTA' file won't coexist with GPEDITOR V1.0.");
          for (i = 0; i < NUM_TEAM_ENTRIES_V12; i++) {
               entries[i].car_colours[0] = '!';
          }

          if (fseek(fp, 0L, SEEK_SET) == 0 && fwrite(entries, sizeof(entries), 1, fp) == 1) {
               (void) puts("'F1ED.DTA' has been modified to coexist with GPEDITOR V1.0.");
          }
          else {
               (void) puts("gpefxdta: Write to 'F1ED.DTA' failed.");
          }
     }
     else {
          (void) puts("This V1.2 'F1ED.DTA' file will work with GPEDITOR V1.0.");
     }
}

static void
dta_v13(
     FILE *fp
) {
     F1ED_DTA_V13_ENTRY  entries[NUM_TEAM_ENTRIES_V13];
     unsigned int        i;
     bool                different = FALSE;

     if (fread(entries, sizeof(entries), 1, fp) != 1) {
          (void) puts("gpefxdta: Failed to read enough data from V1.3 'F1ED.DTA'.");
          return;
     }

     for (i = 0; i < NUM_TEAM_ENTRIES_V13; i++) {
          if (entries[i].car_colours[0] != '!') {
               different = TRUE;
          }
     }

     if (different) {
          (void) puts("This V1.3 'F1ED.DTA' file won't coexist with GPEDITOR V1.0.");
          for (i = 0; i < NUM_TEAM_ENTRIES_V13; i++) {
               entries[i].car_colours[0] = '!';
          }

          if (fseek(fp, 0L, SEEK_SET) == 0 && fwrite(entries, sizeof(entries), 1, fp) == 1) {
               (void) puts("'F1ED.DTA' has been modified to coexist with GPEDITOR V1.0.");
          }
          else {
               (void) puts("gpefxdta: Write to 'F1ED.DTA' failed.");
          }
     }
     else {
          (void) puts("This V1.3 'F1ED.DTA' file will work with GPEDITOR V1.0.");
     }
}

/*---------------------------------------------------------------------------
** End of File
*/

