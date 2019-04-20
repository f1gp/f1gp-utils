/*
** File   : gamexe.cpp
** Author : TK 
** Date   :  1/01/94
**
** $Header:   L:/GPEDITOR/VCS/GAMEXE.CP_   1.7   24 Jun 1995 11:36:28   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Loading/Updating of game EXE.
*/

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <cppwin.hpp>
/*lint +elib(???) */
#include "GPeditor.hpp"
#include "GPeditor.h"
#pragma hdrstop
/*lint -elib(???) */
#include <stdfopen.hpp>
/*lint +elib(???) */
#include "cntrlbar.hpp"
#include "drawarea.hpp"
#include "drawhelm.hpp"
#include "palette.hpp"
#include "teams.hpp"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*
** Location of car design data within game EXE.
*/
#define GP_EXE_V103_EUR_CAR_DESIGNS          0x024dc3L
#define GP_EXE_V104_USA_CAR_DESIGNS          0x026af8L
#define GP_EXE_V105_EUR_CAR_DESIGNS          0x026b24L
#define GP_EXE_V105_ITA_CAR_DESIGNS          0x026af0L

#define GP_EXE_CAR_POWER_OFFSET              (-0x000078L)
#define GP_EXE_HELMET_COLOUR_OFFSET          (+0x000127L)
#define GP_EXE_CAR_NUMBERS                   (-0x000decL)

#define GP_EXE_V103_EUR_DRIVING_AIDS         0x01c7d6L
#define GP_EXE_V104_USA_DRIVING_AIDS         0x01e4d3L
#define GP_EXE_V105_EUR_DRIVING_AIDS         0x01e4ffL
#define GP_EXE_V105_ITA_DRIVING_AIDS         0x01e4cbL

#define GP_EXE_V103_EUR_AI_CAR_GRIP          0x004e83L
#define GP_EXE_V104_USA_AI_CAR_GRIP          0x004ed7L
#define GP_EXE_V105_EUR_AI_CAR_GRIP          0x004ed7L
#define GP_EXE_V105_ITA_AI_CAR_GRIP          0x004ed7L

#define GP_EXE_V103_EUR_PLAYERS_PERF         0x004d44L
#define GP_EXE_V104_USA_PLAYERS_PERF         0x004d88L
#define GP_EXE_V105_EUR_PLAYERS_PERF         0x004d88L
#define GP_EXE_V105_ITA_PLAYERS_PERF         0x004d88L

#define ASM_NOP                              0x90
#define ASM_ADD_AX_BYTE1                     0x81
#define ASM_ADD_AX_BYTE2                     0xc0

/*
** Car colours actual file size after takjng into account that the last
** two cars are in one packed record.
*/

#define COLOURS_FILE_FRAGMENT_SIZE           ((sizeof(GP_EXE_car_colours[0]) * 18) + sizeof(PACKED_REC))

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct {
     unsigned char  pad1[3];
     unsigned char  colour;
     unsigned char  pad2[3];
} PACKED_REC;

typedef struct {
     unsigned char  opcodes[3];
     unsigned short grip;
} AI_CAR_GRIP;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void UnpackPackedColours(void);
static void SavePackedRecord(void);

/*---------------------------------------------------------------------------
** Data
*/

GP_EXE_CAR_COLOURS                 GP_EXE_car_colours[GP_EXE_NUM_TEAMS];
GP_EXE_CAR_POWER                   GP_EXE_car_power[GP_EXE_NUM_TEAMS];
GP_EXE_DRIVER_QUALIFYING           GP_EXE_driver_qualifying[GP_EXE_NUM_DRIVERS];
GP_EXE_DRIVER_RACE                 GP_EXE_driver_race[GP_EXE_NUM_DRIVERS];
GP_EXE_HELMET_COLOURS              GP_EXE_helmet_colours;
unsigned short                     GP_EXE_AiCarGrip;
unsigned short                     GP_EXE_PlayersCarPerf;

static PACKED_REC                  PackedRecord;
static AI_CAR_GRIP                 CarGrip;

/*---------------------------------------------------------------------------
** Functions
*/

/*---------------------------------------------------------------------------
** Purpose:    Attempts to open the user specified game EXE.
**
** Returns:    True if game EXE opened OK, false if not.
**
** Notes  :    Tries various intelligent checks to work out which version of
**             the game EXE it is.
*/
BOOL
AppWindow::FileOpenGameExe(
     char *filename                     /* In  Filename to open            */
) {
     char      szFilterSpec[128];
     BOOL      ok   = TRUE;
     FILE      *fp;

     if (filename) {
          strcpy(tmp_buff, filename);
     }
     else {
          strcpy(tmp_buff, szGpExeFilename);
          strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_EXE));
          ConvertFilterSpec(szFilterSpec);
          StdFileOpenDialog dialog(     Instance(), Handle(),
                                        GetIdt(IDT_OPEN_GAME_EXE_DLG_TITLE),
                                        tmp_buff, sizeof(tmp_buff),
                                        "EXE", szFilterSpec
                                   );
          ok = dialog.ProcessMessages();
     }

     if (ok) {
          Cursor cursor(IDC_WAIT);
          cursor.Select();
          {
               if ((fp = fopen(tmp_buff, "rb")) != NULL) {
                    ok = IsGameExe(fp);

                    if (!ok) {
                         sprintf(msg_buff, GetIdt(IDT_INVALID_GAME_EXE), tmp_buff);
                         MsgBoxWarning mb(GetAppName(), msg_buff);
                         (void) mb.ProcessMessages();
                    }
                    (void) fclose(fp);
               }
               else {
                    ok = FALSE;
                    sprintf(msg_buff, GetIdt(IDT_UNABLE_TO_OPEN), tmp_buff);
                    MsgBoxWarning mb(GetAppName(), msg_buff);
                    (void) mb.ProcessMessages();
               }
          }
          cursor.Deselect();
     }

     if (ok) {
          /*
          ** Reset default driver names relative to EXE car numbers.
          */
          if (!fNamesLoaded) {
               (void) SendMessage(pControlBar->Handle(), IDE_CBAR_LOAD_DRIVER_NAMES, 0, 0L);
          }

          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_SHOW_TEAMS, 0, 0L);
          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_SHOW_DRIVERS, !fCarEditorEnabled, 0L);
          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_TEAM_SELECT, 0, 0L);
          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_DRIVER_SELECT, 0, 0L);
          (void) SendMessage( pDrawingWindow->Handle(),
                              IDE_DESIGN_LOAD_EXE_COLOURS,
                              0,
                              (LONG) (void *) GP_EXE_car_colours
                         );

          (void) SendMessage( pHelmetWindow->Handle(),
                              IDE_HELMET_LOAD_EXE_COLOURS,
                              0,
                              (LONG) (void *) &GP_EXE_helmet_colours
                         );

          (void) SendMessage(pDrawingWindow->Handle(), IDE_DESIGN_TEAM_SELECT, 0, 0L);
          (void) SendMessage(pHelmetWindow->Handle(), IDE_HELMET_DRIVER_SELECT, 0, 0L);
          (void) SendMessage(pTeamsWindow->Handle(), IDE_TEAMS_TEAM_SELECT, 0, 0L);

          strcpy(szGpExeFilename, tmp_buff);
          fGpExeIsFound = TRUE;
          UpdateCaption(szGpExeFilename);
          UpdateMenu();

          (void) SendMessage(pTeamsWindow->Handle(), IDE_TEAMS_UPDATE_ALL_CARS, 0, 0L);

          frame_width  = GetWindowWidth() - GetClientWidth();
          frame_width += pF1CarBitmap->Width();

          frame_width += TEAMS_WINDOW_WIDTH(pF1CarBitmap->Width());
          SetWindowWidth(frame_width);

          (void) pProfile->Write(APP_PROF_STR_EXE_FILENAME, szGpExeFilename);
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Reads the game EXE car colour table and checks its validity.
**
** Returns:    True if car colour table read OK, false otherwise.
**
** Notes  :    Performs intelligent search for game EXE car colour table.
*/
BOOL
AppWindow::IsGameExe(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok;

     /*
     ** Check known locations first (smaller files first as well).
     */
     lGameExeColourFileOffset = GP_EXE_V103_EUR_CAR_DESIGNS;
     ok = (fseek(fp, lGameExeColourFileOffset, SEEK_SET) == 0);
     if (ok) {
          /*
          ** Note: not sizeof(GP_EXE_car_colours) !
          */
          ok = fread(GP_EXE_car_colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
          if (ok) {
               SavePackedRecord();
               UnpackPackedColours();
          }
     }

     if (ok) {
          if ( IsGameExeColoursPresent()     && ReadPerformance(fp) &&
               ReadHelmetColours(fp)         && ReadCarNumbers(fp)
          ) {
               lGameExeDrivingAidsFileOffset      = GP_EXE_V103_EUR_DRIVING_AIDS;
               lGameExeAiCarGripFileOffset        = GP_EXE_V103_EUR_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V103_EUR_PLAYERS_PERF;
               (void) ReadDrivingAids(fp);
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               sprintf(game_exe_version, "%s V1.03", GetIdt(IDT_EUROPEAN));
               return TRUE;
          }
     }

     lGameExeColourFileOffset = GP_EXE_V105_EUR_CAR_DESIGNS;
     ok = (fseek(fp, lGameExeColourFileOffset, SEEK_SET) == 0);
     if (ok) {
          /*
          ** Note: not sizeof(GP_EXE_car_colours) !
          */
          ok = fread(GP_EXE_car_colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
          if (ok) {
               SavePackedRecord();
               UnpackPackedColours();
          }
     }

     if (ok) {
          if ( IsGameExeColoursPresent()     && ReadPerformance(fp) &&
               ReadHelmetColours(fp)         && ReadCarNumbers(fp)
          ) {
               lGameExeDrivingAidsFileOffset      = GP_EXE_V105_EUR_DRIVING_AIDS;
               lGameExeAiCarGripFileOffset        = GP_EXE_V105_EUR_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V105_EUR_PLAYERS_PERF;
               (void) ReadDrivingAids(fp);
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               sprintf(game_exe_version, "%s V1.05", GetIdt(IDT_EUROPEAN));
               return TRUE;
          }
     }

     lGameExeColourFileOffset = GP_EXE_V104_USA_CAR_DESIGNS;
     ok = (fseek(fp, lGameExeColourFileOffset, SEEK_SET) == 0);
     if (ok) {
          /*
          ** Note: not sizeof(GP_EXE_car_colours) !
          */
          ok = fread(GP_EXE_car_colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
          if (ok) {
               SavePackedRecord();
               UnpackPackedColours();
          }
     }

     if (ok) {
          if ( IsGameExeColoursPresent()     && ReadPerformance(fp) &&
               ReadHelmetColours(fp)         && ReadCarNumbers(fp)
          ) {
               lGameExeDrivingAidsFileOffset      = GP_EXE_V104_USA_DRIVING_AIDS;
               lGameExeAiCarGripFileOffset        = GP_EXE_V104_USA_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V104_USA_PLAYERS_PERF;
               (void) ReadDrivingAids(fp);
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               strcpy(game_exe_version, "US V1.04/V1.05");
               return TRUE;
          }
     }

     lGameExeColourFileOffset = GP_EXE_V105_ITA_CAR_DESIGNS;
     ok = (fseek(fp, lGameExeColourFileOffset, SEEK_SET) == 0);
     if (ok) {
          /*
          ** Note: not sizeof(GP_EXE_car_colours) !
          */
          ok = fread(GP_EXE_car_colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
          if (ok) {
               SavePackedRecord();
               UnpackPackedColours();
          }
     }

     if (ok) {
          if ( IsGameExeColoursPresent()     && ReadPerformance(fp) &&
               ReadHelmetColours(fp)         && ReadCarNumbers(fp)
          ) {
               lGameExeDrivingAidsFileOffset      = GP_EXE_V105_ITA_DRIVING_AIDS;
               lGameExeAiCarGripFileOffset        = GP_EXE_V105_ITA_AI_CAR_GRIP;
               lGameExePlayersCarPerfFileOffset   = GP_EXE_V105_ITA_PLAYERS_PERF;
               (void) ReadDrivingAids(fp);
               (void) ReadAiCarGrip(fp);
               (void) ReadPlayersCarPerf(fp);

               strcpy(game_exe_version, "Italian V1.05");
               return TRUE;
          }
     }

     /*
     ** Search through the file looking for a match.
     */
     strcpy(msg_buff, GetAppName());
     MsgBoxYesNo mb(msg_buff, GetIdt(IDT_UNKNOWN_GAME_VERSION));
     (void) mb.ProcessMessages();

     if (mb.No()) {
          return FALSE;
     }

     /*
     ** This will be *slow*. It could be speeded up!
     */
     (void) fseek(fp, 0L, SEEK_SET);
     for (ok = TRUE; ok;) {
          unsigned char  c;

          ok = fread(&c, sizeof(c), 1, fp);

          if (ok && c == '!') {
               long pos = ftell(fp) - 1L;

               (void) fseek(fp, pos, SEEK_SET);

               /*
               ** Note: not sizeof(GP_EXE_car_colours) !
               */
               ok = fread(GP_EXE_car_colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
               if (ok) {
                    /*
                    ** Must set global here as the Read functions use it!
                    */
                    SavePackedRecord();
                    UnpackPackedColours();
                    lGameExeColourFileOffset = pos;
                    if (IsGameExeColoursPresent() && ReadPerformance(fp) && ReadHelmetColours(fp)) {
                         lGameExeColourFileOffset           = pos;
                         lGameExeDrivingAidsFileOffset      = 0L;
                         lGameExeAiCarGripFileOffset        = 0L;
                         lGameExePlayersCarPerfFileOffset   = 0L;
                         strcpy(game_exe_version, "V?.??");
                         return TRUE;
                    }
               }
               (void) fseek(fp, pos + 1L, SEEK_SET);
          }
     }

     return FALSE;
}

/*---------------------------------------------------------------------------
** Purpose:    Checks colour table data structure for known good values.
**
** Returns:    True if colour table contains valid data, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::IsGameExeColoursPresent(
     void
) {
     BOOL           ok = TRUE;
     unsigned short i;
     unsigned short matches = 0;
     
     /*
     ** Note: We check the a percentage number of teams, not the actual
     ** number of teams as F1ED seems to splat the '!' for some of them.
     ** The last two cars aren't there as there in a packed record.
     */
     for (i = 0; ok && i < NUM_ELEMENTS(GP_EXE_car_colours) - 2; i++) {
          if (GP_EXE_car_colours[i].pling == '!') {
               ++matches;
          }
     }

     /*
     ** Assume >= 75% matches is OK.
     */
     if (matches < NUM_ELEMENTS(GP_EXE_car_colours) - (NUM_ELEMENTS(GP_EXE_car_colours) / 4)) {
          ok = FALSE;
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    To read in the car and driver performance.
**
** Returns:    True if read OK, false if read or seek failed.
**
** Notes  :    
*/
BOOL
AppWindow::ReadPerformance(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_CAR_POWER_OFFSET, SEEK_SET) == 0);
     if (ok) {
          ok = fread(GP_EXE_car_power, sizeof(GP_EXE_car_power), 1, fp);
     }
     if (ok) {
          ok = fread(GP_EXE_driver_qualifying, sizeof(GP_EXE_driver_qualifying), 1, fp);
     }
     if (ok) {
          ok = fread(GP_EXE_driver_race, sizeof(GP_EXE_driver_race), 1, fp);
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    To read in the helmet colours.
**
** Returns:    True if read OK, false if read or seek failed.
**
** Notes  :    
*/
BOOL
AppWindow::ReadHelmetColours(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_HELMET_COLOUR_OFFSET, SEEK_SET) == 0);
     if (ok) {
          ok = fread(&GP_EXE_helmet_colours, sizeof(GP_EXE_helmet_colours), 1, fp);
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Read in current driving aids settings.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::ReadDrivingAids(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     if (lGameExeDrivingAidsFileOffset) {
          ok = (fseek(fp, lGameExeDrivingAidsFileOffset, SEEK_SET) == 0);
          if (ok) {
               ok = fread(GP_EXE_driving_aids, sizeof(GP_EXE_driving_aids), 1, fp);
          }
     }
     else {
          ok = FALSE;
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Read in current driving aids settings.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::ReadCarNumbers(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_CAR_NUMBERS, SEEK_SET) == 0);
     if (ok) {
          ok = fread(GP_EXE_car_numbers, sizeof(GP_EXE_car_numbers), 1, fp);
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Read in AI Car Grip setting.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::ReadAiCarGrip(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     if (lGameExeAiCarGripFileOffset) {
          ok = (fseek(fp, lGameExeAiCarGripFileOffset, SEEK_SET) == 0);
          if (ok) {
               ok = fread(&CarGrip, sizeof(CarGrip), 1, fp);
               GP_EXE_AiCarGrip = CarGrip.grip;
               if (CarGrip.opcodes[1] == ASM_ADD_AX_BYTE1) {
                    /*
                    ** Change value from an offset from 0x4000 due to ADD.
                    */
                    GP_EXE_AiCarGrip += 0x4000;
               }
          }
     }
     else {
          ok = FALSE;
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Read in AI Car Grip setting.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::ReadPlayersCarPerf(
     FILE *fp                           /* In  File pointer for game EXE   */
) {
     BOOL ok = TRUE;

     if (lGameExePlayersCarPerfFileOffset) {
          ok = (fseek(fp, lGameExePlayersCarPerfFileOffset, SEEK_SET) == 0);
          if (ok) {
               ok = fread(&GP_EXE_PlayersCarPerf, sizeof(GP_EXE_PlayersCarPerf), 1, fp);
          }
     }
     else {
          ok = FALSE;
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Updates the game EXE file with the current colour table.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::FileUpdateGameExe(
     void
) {
     static BOOL    first = TRUE;
     BOOL           ok;
     FILE           *fp;
     
     ASSERT(fGpExeIsFound);

     if (first) {
          first = FALSE;
          strcpy(msg_buff, GetAppName());
          MsgBoxYesNo mb(msg_buff, GetIdt(IDT_ENSURE_GAME_EXE_BACKED_UP), MB_DEFBUTTON2);
          (void) mb.ProcessMessages();
          if (mb.No()) {
               return FALSE;
          }
     }

     Cursor cursor(IDC_WAIT);
     cursor.Select();
     {
          (void) SendMessage( pDrawingWindow->Handle(),
                              IDE_DESIGN_SAVE_EXE_COLOURS,
                              0,
                              (LONG) (void *) GP_EXE_car_colours
                         );

          (void) SendMessage( pHelmetWindow->Handle(),
                              IDE_HELMET_SAVE_EXE_COLOURS,
                              0,
                              (LONG) (void *) &GP_EXE_helmet_colours
                         );

          if ((fp = fopen(szGpExeFilename, "rb+")) != NULL) {
               ok = (fseek(fp, lGameExeColourFileOffset, SEEK_SET) == 0);
               if (ok) {
                    /*
                    ** Note: not sizeof(GP_EXE_car_colours) !
                    ** Update packed car colours, use copy so we don't
                    ** trash displayed colours.
                    */
                    static GP_EXE_CAR_COLOURS Colours[GP_EXE_NUM_TEAMS];
                    PACKED_REC *pr = (PACKED_REC *) &Colours[18];

                    memcpy(Colours, GP_EXE_car_colours, sizeof(Colours));
                    memcpy(pr, &PackedRecord, sizeof(*pr));
                    pr->colour = GP_EXE_car_colours[18].colours[0];
                    ok = fwrite(Colours, COLOURS_FILE_FRAGMENT_SIZE, 1, fp);
               }

               if (ok) {
                    ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_CAR_POWER_OFFSET, SEEK_SET) == 0);
                    if (ok) {
                         ok = fwrite(GP_EXE_car_power, sizeof(GP_EXE_car_power), 1, fp);
                    }
               }

               if (ok) {
                    ok = fwrite(GP_EXE_driver_qualifying, sizeof(GP_EXE_driver_qualifying), 1, fp);
               }

               if (ok) {
                    ok = fwrite(GP_EXE_driver_race, sizeof(GP_EXE_driver_race), 1, fp);
               }

               if (ok) {
                    ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_HELMET_COLOUR_OFFSET, SEEK_SET) == 0);
                    if (ok) {
                         ok = fwrite(&GP_EXE_helmet_colours, sizeof(GP_EXE_helmet_colours), 1, fp);
                    }
               }

               if (ok && lGameExeDrivingAidsFileOffset) {
                    ok = (fseek(fp, lGameExeDrivingAidsFileOffset, SEEK_SET) == 0);
                    if (ok) {
                         ok = fwrite(GP_EXE_driving_aids, sizeof(GP_EXE_driving_aids), 1, fp);
                    }
               }

               if (ok) {
                    ok = (fseek(fp, lGameExeColourFileOffset + GP_EXE_CAR_NUMBERS, SEEK_SET) == 0);
                    if (ok) {
                         ok = fwrite(GP_EXE_car_numbers, sizeof(GP_EXE_car_numbers), 1, fp);
                    }
               }

               if (ok && lGameExeAiCarGripFileOffset) {
                    ok = (fseek(fp, lGameExeAiCarGripFileOffset, SEEK_SET) == 0);
                    if (ok) {
                         if (GP_EXE_AiCarGrip == 0x4000) {
                              /*
                              ** Restore original.
                              **
                              ** JNS ....
                              ** MOV AX, 4000H
                              */
                              CarGrip.opcodes[0] = 0x79;
                              CarGrip.opcodes[1] = 0x03;
                              CarGrip.opcodes[2] = 0xb8;
                         }
                         else {
                              /*
                              ** Change
                              **
                              ** NOP
                              ** ADD AX, offset
                              */
                              CarGrip.opcodes[0] = ASM_NOP;
                              CarGrip.opcodes[1] = ASM_ADD_AX_BYTE1;
                              CarGrip.opcodes[2] = ASM_ADD_AX_BYTE2;
                         }

                         CarGrip.grip = GP_EXE_AiCarGrip;
                         if (CarGrip.opcodes[1] == ASM_ADD_AX_BYTE1) {
                              /*
                              ** Change value to an offset from 0x4000 due to ADD.
                              */
                              CarGrip.grip -= 0x4000;
                         }
                         ok = fwrite(&CarGrip, sizeof(CarGrip), 1, fp);
                    }
               }

               if (ok && lGameExePlayersCarPerfFileOffset) {
                    ok = (fseek(fp, lGameExePlayersCarPerfFileOffset, SEEK_SET) == 0);
                    if (ok) {
                         ok = fwrite(&GP_EXE_PlayersCarPerf, sizeof(GP_EXE_PlayersCarPerf), 1, fp);
                    }
               }

               if (!ok) {
                    sprintf(msg_buff, GetIdt(IDT_WRITE_FAILED), szGpExeFilename);
                    MsgBoxWarning mb(GetAppName(), msg_buff);
                    (void) mb.ProcessMessages();
               }
               (void) fclose(fp);
          }
          else {
               ok = FALSE;
               sprintf(msg_buff, GetIdt(IDT_UNABLE_TO_OPEN), szGpExeFilename);
               MsgBoxWarning mb(GetAppName(), msg_buff);
               (void) mb.ProcessMessages();
          }
     }
     cursor.Deselect();

     if (ok) {
          strcpy(msg_buff, GetAppName());
          MsgBoxInfo mb(msg_buff, GetIdt(IDT_SUCCESSFULLY_UPDATED));
          (void) mb.ProcessMessages();
     }
     return ok;
}

static void
UnpackPackedColours(
     void
) {
     /*
     ** Unpack packed records.
     */
     PACKED_REC *pr = (PACKED_REC *) &GP_EXE_car_colours[18];

     memset(&GP_EXE_car_colours[18], pr->colour, sizeof(GP_EXE_car_colours[18]));
     memset(&GP_EXE_car_colours[19], pr->colour, sizeof(GP_EXE_car_colours[19]));
}

static void
SavePackedRecord(
     void
) {
     PACKED_REC *pr = (PACKED_REC *) &GP_EXE_car_colours[18];
     
     memcpy(&PackedRecord, pr, sizeof(PackedRecord));
}

/*---------------------------------------------------------------------------
** End of File
*/

