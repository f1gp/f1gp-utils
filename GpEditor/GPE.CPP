/*
** File   : gpe.cpp
** Author : TK 
** Date   : 22/02/94
**
** $Header:   L:/GPEDITOR/VCS/GPE.CP_   1.5   24 Jun 1995 11:36:30   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** F1SA.GPE Import/Export facility.
*/

/*lint -elib(???) */
#include <cppwin.hpp>
/*lint +elib(???) */
#include "GPeditor.hpp"
#include "GPeditor.h"
#pragma hdrstop
/*lint -elib(???) */
#include <time.h>
#include <stdfopen.hpp>
#include <io.h>
/*lint +elib(???) */
#include "cntrlbar.hpp"
#include "drawarea.hpp"
#include "drawhelm.hpp"
#include "teams.hpp"
#include "gpe.hpp"
#include "version.ipp"

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define GPE_FILE_EXTENSION              "GPE"
#define COMMENT_FILE_EXTENSION          "TXT"

#define TEXT_BUFF_SIZE                  16000

/*---------------------------------------------------------------------------
** Typedefs
*/

class GpeImpCommentDialog : public DialogWindow {
public:
     GpeImpCommentDialog(HINSTANCE inst, HWND parent, char *text, time_t t);
     ~GpeImpCommentDialog();

private:
     void Initialise(Event *event);
     BOOL UserEventHandler(Event *event);
     void Save(void);

     char      *pText;
     time_t    Time;
     char      msg_buff[128];
};

class GpeExpCommentDialog : public DialogWindow {
public:
     GpeExpCommentDialog(HINSTANCE inst, HWND parent, FILE *fp, BOOL *pOk);
     ~GpeExpCommentDialog();

private:
     BOOL UserEventHandler(Event *event);
     void Load(void);
     void OK();

     FILE      *fp;
     BOOL      *pOk;
     char      msg_buff[128];
};

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static unsigned short CalcCrc(void const *ptr, unsigned short len);

/*---------------------------------------------------------------------------
** Data
*/

static GPE_HEADER   hdr;
static GPE_RECORD   rec;

/*---------------------------------------------------------------------------
** Functions
*/

BOOL
AppWindow::ImportGpe(
     void
) {
     FILE           *fp;
     BOOL           ok;
     char           szFilterSpec[128];

     strcpy(tmp_buff, szGpeFilename);
     strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_GPE));
     ConvertFilterSpec(szFilterSpec);
     StdFileOpenDialog dialog(     Instance(), Handle(),
                                   GetIdt(IDT_IMPORT_GPE_DLG_TITLE),
                                   tmp_buff, sizeof(tmp_buff),
                                   GPE_FILE_EXTENSION,
                                   szFilterSpec
                              );
     ok = dialog.ProcessMessages();

     if (ok) {
          Cursor cursor(IDC_WAIT);
          cursor.Select();
          {
               if ((fp = fopen(tmp_buff, "rb")) != NULL) {
                    ok = GpeReadHeader(fp);
                    if (ok) {
                         ok = GpeRead(fp);
                         if (!ok) {
                              sprintf(msg_buff, GetIdt(IDT_READ_FAILED), tmp_buff);
                              MsgBoxError mb(GetAppName(), msg_buff);
                              (void) mb.ProcessMessages();
                         }
                    }
                    else {
                         ok = FALSE;
                         sprintf(msg_buff, GetIdt(IDT_INVALID_GPE_FILE), tmp_buff);
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
          strcpy(szGpeFilename, tmp_buff);
          (void) pProfile->Write(APP_PROF_STR_GPE_FILENAME, szGpeFilename);

          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_LOAD_NAMES, 0, 0L);
          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_LOAD_DRIVER_NAMES, 0, 0L);

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
          (void) SendMessage(pTeamsWindow->Handle(), IDE_TEAMS_UPDATE_ALL_CARS, 0, 0L);
     }

     return ok;
}

BOOL
AppWindow::ExportGpe(
     void
) {
     FILE           *fp;
     BOOL           ok;
     char           szFilterSpec[128];

     strcpy(tmp_buff, szGpeFilename);
     strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_GPE));
     ConvertFilterSpec(szFilterSpec);
     StdFileOpenDialog dialog(     Instance(), Handle(),
                                   GetIdt(IDT_EXPORT_GPE_DLG_TITLE),
                                   tmp_buff, sizeof(tmp_buff),
                                   GPE_FILE_EXTENSION,
                                   szFilterSpec
                              );
     ok = dialog.ProcessMessages();

     if (ok) {
          if (access(tmp_buff, 0x00) == 0) {
               sprintf(msg_buff, GetIdt(IDT_OVERWRITE_EXISTING_FILE), tmp_buff);

               MsgBoxYesNo    mb(GetAppName(), msg_buff);
               (void) mb.ProcessMessages();
               if (mb.No()) {
                    ok = FALSE;
               }
          }
     }

     if (ok) {
          Cursor cursor(IDC_WAIT);
          cursor.Select();
          {
               /*
               ** Ensure the current windows values are present.
               */
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

               if ((fp = fopen(tmp_buff, "wb")) != NULL) {
                    ok = GpeWrite(fp);

                    if (!ok) {
                         sprintf(msg_buff, GetIdt(IDT_WRITE_FAILED), tmp_buff);
                         MsgBoxError mb(GetAppName(), msg_buff);
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
          strcpy(szGpeFilename, tmp_buff);
          (void) pProfile->Write(APP_PROF_STR_GPE_FILENAME, szGpeFilename);
     }

     return ok;
}

BOOL
AppWindow::GpeReadHeader(
     FILE *fp                           /* In  File to read from           */
) {
     BOOL ok = TRUE;

     ok = fread(&hdr, sizeof(hdr), 1, fp);
     if (ok) {
          ok = (hdr.magic == GPE_HEADER_MAGIC) ? TRUE : FALSE;
     }

     return ok;
}

BOOL
AppWindow::GpeRead(
     FILE *fp                           /* In  File to read from           */
) {
     static GP_NAMES                    tmp_names;
     static GP_EXE_CAR_COLOURS          tmp_car_colours[GP_EXE_NUM_TEAMS];
     static GP_EXE_CAR_POWER            tmp_car_power[GP_EXE_NUM_TEAMS];
     static GP_EXE_CAR_NUMBER           tmp_car_numbers[GP_EXE_NUM_DRIVERS];
     static GP_EXE_DRIVER_QUALIFYING    tmp_driver_qualifying[GP_EXE_NUM_DRIVERS];
     static GP_EXE_DRIVER_RACE          tmp_driver_race[GP_EXE_NUM_DRIVERS];
     static GP_EXE_HELMET_COLOURS       tmp_helmet_colours;
     static GPE_REC_AI_CAR_GRIP         tmp_ai_car_grip;
     static GPE_REC_PLAYERS_BHP         tmp_players_bhp;
     BOOL                               f_tmp_names              = FALSE;
     BOOL                               f_tmp_car_colours        = FALSE;
     BOOL                               f_tmp_car_power          = FALSE;
     BOOL                               f_tmp_car_numbers        = FALSE;
     BOOL                               f_tmp_driver_qualifying  = FALSE;
     BOOL                               f_tmp_driver_race        = FALSE;
     BOOL                               f_tmp_helmet_colours     = FALSE;
     BOOL                               f_tmp_ai_car_grip        = FALSE;
     BOOL                               f_tmp_players_bhp        = FALSE;
     BOOL                               ok   = TRUE;
     BOOL                               done = FALSE;

     while (ok && !done) {
          ok = fread(&rec, sizeof(rec), 1, fp);
          if (ok) {
               ok = (rec.magic == GPE_RECORD_MAGIC) ? TRUE : FALSE;
          }

          if (ok) {
               switch (rec.type) {
               case GPE_RT_EOF:
                    done = TRUE;
                    break;

               case GPE_RT_EXE_CAR_COLOURS:
                    ok = (rec.size == sizeof(tmp_car_colours)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(tmp_car_colours, sizeof(tmp_car_colours), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(tmp_car_colours, sizeof(tmp_car_colours))) ? TRUE : FALSE;
                    }
                    f_tmp_car_colours = TRUE;
                    break;

               case GPE_RT_EXE_HELMET_COLOURS:
                    ok = (rec.size == sizeof(tmp_helmet_colours)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(&tmp_helmet_colours, sizeof(tmp_helmet_colours), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(&tmp_helmet_colours, sizeof(tmp_helmet_colours))) ? TRUE : FALSE;
                    }
                    f_tmp_helmet_colours = TRUE;
                    break;

               case GPE_RT_EXE_CAR_POWER:
                    ok = (rec.size == sizeof(tmp_car_power)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(tmp_car_power, sizeof(tmp_car_power), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(tmp_car_power, sizeof(tmp_car_power))) ? TRUE : FALSE;
                    }
                    f_tmp_car_power = TRUE;
                    break;

               case GPE_RT_EXE_DRIVER_QUALIFYING:
                    ok = (rec.size == sizeof(tmp_driver_qualifying)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(tmp_driver_qualifying, sizeof(tmp_driver_qualifying), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(tmp_driver_qualifying, sizeof(tmp_driver_qualifying))) ? TRUE : FALSE;
                    }
                    f_tmp_driver_qualifying = TRUE;
                    break;

               case GPE_RT_EXE_DRIVER_RACE:
                    ok = (rec.size == sizeof(tmp_driver_race)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(tmp_driver_race, sizeof(tmp_driver_race), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(tmp_driver_race, sizeof(tmp_driver_race))) ? TRUE : FALSE;
                    }
                    f_tmp_driver_race = TRUE;
                    break;

               case GPE_RT_EXE_CAR_NUMBERS:
                    ok = (rec.size == sizeof(tmp_car_numbers)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(tmp_car_numbers, sizeof(tmp_car_numbers), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(tmp_car_numbers, sizeof(tmp_car_numbers))) ? TRUE : FALSE;
                    }
                    f_tmp_car_numbers = TRUE;
                    break;

               case GPE_RT_GAME_NAMES:
                    ok = (rec.size == sizeof(tmp_names)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(&tmp_names, sizeof(tmp_names), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(&tmp_names, sizeof(tmp_names))) ? TRUE : FALSE;
                    }
                    f_tmp_names = ok;
                    break;

               case GPE_RT_COMMENT:
                    if (ok) {
                         HLOCAL    hLocal    = LocalAlloc(LMEM_MOVEABLE, rec.size + 1);
                         char      *pszLocal;

                         ok = (hLocal != NULL) ? TRUE : FALSE;
                         if (ok) {
                              pszLocal = (char *) LocalLock(hLocal);
                              {
                                   ok = fread(pszLocal, rec.size, 1, fp);
                                   pszLocal[rec.size] = '\0';

                                   if (ok) {
                                        ok = (rec.crc == CalcCrc(pszLocal, rec.size)) ? TRUE : FALSE;
                                   }

                                   if (ok) {
                                        GpeImpCommentDialog dlg(Instance(), Handle(), pszLocal, hdr.time);
                                        (void) dlg.ProcessMessages();
                                   }
                              }
                              (void) LocalUnlock(hLocal);
                              (void) LocalFree(hLocal);
                         }
                         else {
                              strcpy(msg_buff, GetAppName());
                              MsgBoxError mbe(msg_buff, GetIdt(IDT_OUT_OF_MEMORY));
                              (void) mbe.ProcessMessages();
                         }
                    }
                    break;

               case GPE_RT_EXE_AI_CAR_GRIP:
                    ok = (rec.size == sizeof(tmp_ai_car_grip)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(&tmp_ai_car_grip, sizeof(tmp_ai_car_grip), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(&tmp_ai_car_grip, sizeof(tmp_ai_car_grip))) ? TRUE : FALSE;
                    }
                    f_tmp_ai_car_grip = ok;
                    break;

               case GPE_RT_EXE_PLAYERS_BHP:
                    ok = (rec.size == sizeof(tmp_players_bhp)) ? TRUE : FALSE;
                    if (ok) {
                         ok = fread(&tmp_players_bhp, sizeof(tmp_players_bhp), 1, fp);
                    }
                    if (ok) {
                         ok = (rec.crc == CalcCrc(&tmp_players_bhp, sizeof(tmp_players_bhp))) ? TRUE : FALSE;
                    }
                    f_tmp_players_bhp = ok;
                    break;

               default:
                    /*
                    ** Skip unknown record type.
                    */
                    (void) fseek(fp, (long) rec.size, SEEK_CUR);
                    break;
               }
          }
     }

     /*
     ** Only update all fields if we read the whole file OK.
     */
     if (ok) {
          if (f_tmp_names) {
               fNamesLoaded = TRUE;
               memcpy(&GP_names, &tmp_names, sizeof(GP_names));
          }
          if (f_tmp_car_colours) {
               memcpy(GP_EXE_car_colours, tmp_car_colours, sizeof(GP_EXE_car_colours));
          }
          if (f_tmp_car_power) {
               memcpy(GP_EXE_car_power, tmp_car_power, sizeof(GP_EXE_car_power));
          }
          if (f_tmp_car_numbers) {
               memcpy(GP_EXE_car_numbers, tmp_car_numbers, sizeof(GP_EXE_car_numbers));
          }
          if (f_tmp_driver_qualifying) {
               memcpy(GP_EXE_driver_qualifying, tmp_driver_qualifying, sizeof(GP_EXE_driver_qualifying));
          }
          if (f_tmp_driver_race) {
               memcpy(GP_EXE_driver_race, tmp_driver_race, sizeof(GP_EXE_driver_race));
          }
          if (f_tmp_helmet_colours) {
               memcpy(&GP_EXE_helmet_colours, &tmp_helmet_colours, sizeof(GP_EXE_helmet_colours));
          }
          if (f_tmp_ai_car_grip && lGameExeAiCarGripFileOffset) {
               GP_EXE_AiCarGrip = tmp_ai_car_grip.grip;
          }
          if (f_tmp_players_bhp && lGameExePlayersCarPerfFileOffset) {
               GP_EXE_PlayersCarPerf = tmp_players_bhp.bhp;
          }
     }

     return ok;
}

BOOL
AppWindow::GpeWrite(
     FILE *fp                           /* In  File to write to            */
) {
     GPE_REC_AI_CAR_GRIP grip;
     GPE_REC_PLAYERS_BHP players_bhp;
     time_t              t;
     BOOL                ok = TRUE;

     (void) time(&t);

     hdr.magic                     = GPE_HEADER_MAGIC;
     hdr.gpeditor_version_major    = VERSION_MAJOR;
     hdr.gpeditor_version_minor    = VERSION_MINOR;
     hdr.time                      = t;
     rec.magic                     = GPE_RECORD_MAGIC;

     /*
     ** Header.
     */
     ok = fwrite(&hdr, sizeof(hdr), 1, fp);

     /*
     ** Ask for comment.
     */
     if (ok) {
          GpeExpCommentDialog dlg(Instance(), Handle(), fp, &ok);
          (void) dlg.ProcessMessages();
     }

     /*
     ** Car Colours.
     */
     if (ok) {
          rec.type  = GPE_RT_EXE_CAR_COLOURS;
          rec.size  = sizeof(GP_EXE_car_colours);
          rec.crc   = CalcCrc(GP_EXE_car_colours, sizeof(GP_EXE_car_colours));
          ok = fwrite(&rec, sizeof(rec), 1, fp);
     }
     if (ok) {
          ok = fwrite(GP_EXE_car_colours, sizeof(GP_EXE_car_colours), 1, fp);
     }

     /*
     ** Helmet Colours.
     */
     if (ok) {
          rec.type  = GPE_RT_EXE_HELMET_COLOURS;
          rec.size  = sizeof(GP_EXE_helmet_colours);
          rec.crc   = CalcCrc(&GP_EXE_helmet_colours, sizeof(GP_EXE_helmet_colours));
          ok = fwrite(&rec, sizeof(rec), 1, fp);
     }
     if (ok) {
          ok = fwrite(&GP_EXE_helmet_colours, sizeof(GP_EXE_helmet_colours), 1, fp);
     }

     /*
     ** Car Power.
     */
     if (ok) {
          rec.type  = GPE_RT_EXE_CAR_POWER;
          rec.size  = sizeof(GP_EXE_car_power);
          rec.crc   = CalcCrc(GP_EXE_car_power, sizeof(GP_EXE_car_power));
          ok = fwrite(&rec, sizeof(rec), 1, fp);
     }
     if (ok) {
          ok = fwrite(GP_EXE_car_power, sizeof(GP_EXE_car_power), 1, fp);
     }

     /*
     ** Driver Qualifying.
     */
     if (ok) {
          rec.type  = GPE_RT_EXE_DRIVER_QUALIFYING;
          rec.size  = sizeof(GP_EXE_driver_qualifying);
          rec.crc   = CalcCrc(GP_EXE_driver_qualifying, sizeof(GP_EXE_driver_qualifying));
          ok = fwrite(&rec, sizeof(rec), 1, fp);
     }
     if (ok) {
          ok = fwrite(GP_EXE_driver_qualifying, sizeof(GP_EXE_driver_qualifying), 1, fp);
     }
     
     /*
     ** Driver Race.
     */
     if (ok) {
          rec.type  = GPE_RT_EXE_DRIVER_RACE;
          rec.size  = sizeof(GP_EXE_driver_race);
          rec.crc   = CalcCrc(GP_EXE_driver_race, sizeof(GP_EXE_driver_race));
          ok = fwrite(&rec, sizeof(rec), 1, fp);
     }
     if (ok) {
          ok = fwrite(GP_EXE_driver_race, sizeof(GP_EXE_driver_race), 1, fp);
     }

     /*
     ** Car Numbers.
     */
     if (ok) {
          rec.type  = GPE_RT_EXE_CAR_NUMBERS;
          rec.size  = sizeof(GP_EXE_car_numbers);
          rec.crc   = CalcCrc(GP_EXE_car_numbers, sizeof(GP_EXE_car_numbers));
          ok = fwrite(&rec, sizeof(rec), 1, fp);
     }
     if (ok) {
          ok = fwrite(GP_EXE_car_numbers, sizeof(GP_EXE_car_numbers), 1, fp);
     }

     /*
     ** Driver Names.
     */
     if (ok && fNamesLoaded) {
          rec.type  = GPE_RT_GAME_NAMES;
          rec.size  = sizeof(GP_names);
          rec.crc   = CalcCrc(&GP_names, sizeof(GP_names));
          ok = fwrite(&rec, sizeof(rec), 1, fp);
     }
     if (ok && fNamesLoaded) {
          ok = fwrite(&GP_names, sizeof(GP_names), 1, fp);
     }

     /*
     ** AI Car Grip
     */
     grip.grip = GP_EXE_AiCarGrip;
     if (ok && lGameExeAiCarGripFileOffset) {
          rec.type  = GPE_RT_EXE_AI_CAR_GRIP;
          rec.size  = sizeof(grip);
          rec.crc   = CalcCrc(&grip, sizeof(grip));
          ok = fwrite(&rec, sizeof(rec), 1, fp);
     }
     if (ok && lGameExeAiCarGripFileOffset) {
          ok = fwrite(&grip, sizeof(grip), 1, fp);
     }

     /*
     ** Player's BHP.
     */
     players_bhp.bhp = GP_EXE_PlayersCarPerf;
     if (ok && lGameExePlayersCarPerfFileOffset) {
          rec.type  = GPE_RT_EXE_PLAYERS_BHP;
          rec.size  = sizeof(players_bhp);
          rec.crc   = CalcCrc(&players_bhp, sizeof(players_bhp));
          ok = fwrite(&rec, sizeof(rec), 1, fp);
     }
     if (ok && lGameExePlayersCarPerfFileOffset) {
          ok = fwrite(&players_bhp, sizeof(players_bhp), 1, fp);
     }

     /*
     ** EOF record.
     */
     if (ok) {
          rec.type  = GPE_RT_EOF;
          rec.size  = 0;
          rec.crc   = 0;
          ok = fwrite(&rec, sizeof(rec), 1, fp);
     }
     
     return ok;
}

static unsigned short
CalcCrc(
     void const     *ptr,               /* In  Data area                   */
     unsigned short len                 /* In  Length                      */
) {
     unsigned char const *p = (unsigned char const *) ptr;
     unsigned char       check_a = 0xff;
     unsigned char       check_b = 0xff;

     while (len--) {
          check_a += *p++;
          check_b += check_a;
     }

     return ((check_b << 8) | check_a);
}

GpeImpCommentDialog::GpeImpCommentDialog(
     HINSTANCE      inst,                    /* In  Instance handle        */
     HWND           parent,                  /* In  Parent handle          */
     char           *text,                   /* In  Text comment           */
     time_t         t                        /* In  Creation time          */
) : DialogWindow(inst, parent, GetDlgTemplateName(APP_DIALOG_GPE_IMP_COMMENT)) {
     pText     = text;
     Time      = t;
}

GpeImpCommentDialog::~GpeImpCommentDialog(
     void
) {
     ;
}

void
GpeImpCommentDialog::Initialise(
     Event *
) {
     /*
     ** Append GPE file creation time to caption.
     */
     (void) GetWindowText(Handle(), msg_buff, sizeof(msg_buff));
     strcat(msg_buff, " - ");
     strcat(msg_buff, ctime(&Time));
     msg_buff[strlen(msg_buff) - 1] = '\0';

     SetWindowText(Handle(), msg_buff);

     (void) SendDlgItemMessage(Handle(), IDD_GPE_IMP_COMMENT, WM_SETTEXT, 0, (LONG) (void far *) pText);
}

BOOL
GpeImpCommentDialog::UserEventHandler(
     Event *event
) {
     BOOL message_processed = TRUE;

     switch (event->wParam) {
     case IDD_GPE_IMP_COMMENT_SAVE:
          Save();
          break;

     default:
          message_processed = FALSE;
          break;
     }

     return message_processed;
}

void
GpeImpCommentDialog::Save(
     void
) {
     FILE           *fp;
     BOOL           ok;
     char           szFilterSpec[128];
     char           tmp_buff[128];

     strcpy(tmp_buff, szCommentFilename);
     strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_TXT));
     ConvertFilterSpec(szFilterSpec);
     StdFileOpenDialog dialog(     Instance(), Handle(),
                                   GetIdt(IDT_GPE_SAVE_COMMENT_DLG_TITLE),
                                   tmp_buff, sizeof(tmp_buff),
                                   COMMENT_FILE_EXTENSION,
                                   szFilterSpec
                              );
     ok = dialog.ProcessMessages();

     if (ok) {
          if (access(tmp_buff, 0x00) == 0) {
               sprintf(msg_buff, GetIdt(IDT_OVERWRITE_EXISTING_FILE), tmp_buff);

               MsgBoxYesNo    mb(GetAppName(), msg_buff);
               (void) mb.ProcessMessages();
               if (mb.No()) {
                    ok = FALSE;
               }
          }
     }

     if (ok) {
          Cursor cursor(IDC_WAIT);
          cursor.Select();
          {
               if ((fp = fopen(tmp_buff, "wb")) != NULL) {
                    HLOCAL    hLocal;
                    char      *pszLocal;

                    hLocal = (HLOCAL) SendDlgItemMessage(Handle(), IDD_GPE_IMP_COMMENT, EM_GETHANDLE, 0, 0L);
                    if (hLocal) {
                         pszLocal = (char *) LocalLock(hLocal);
                         {
                              ok = fwrite(pszLocal, strlen(pszLocal), 1, fp);
                         }
                         (void) LocalUnlock(hLocal);

                         if (!ok) {
                              sprintf(msg_buff, GetIdt(IDT_WRITE_FAILED), tmp_buff);
                              MsgBoxError mb(GetAppName(), msg_buff);
                              (void) mb.ProcessMessages();
                         }
                    }
                    else {
                         ok = FALSE;
                         
                         strcpy(msg_buff, GetAppName());
                         MsgBoxError mbe(msg_buff, GetIdt(IDT_OUT_OF_MEMORY));
                         (void) mbe.ProcessMessages();
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
          strcpy(szCommentFilename, tmp_buff);

          ProfileString ps(APP_INIFILE_KEYWORD, APP_INIFILE);
          (void) ps.Write(APP_PROF_STR_COMMENT_FILENAME, szCommentFilename);
     }
}

GpeExpCommentDialog::GpeExpCommentDialog(
     HINSTANCE      inst,                    /* In  Instance handle        */
     HWND           parent,                  /* In  Parent handle          */
     FILE           *ip_fp,                  /* In  File to write to       */
     BOOL           *ip_pOk                  /* Out Success or failure     */
) : DialogWindow(inst, parent, GetDlgTemplateName(APP_DIALOG_GPE_EXP_COMMENT)) {
     fp   = ip_fp;
     pOk  = ip_pOk;
     *pOk = FALSE;
}

GpeExpCommentDialog::~GpeExpCommentDialog(
     void
) {
     ;
}

BOOL
GpeExpCommentDialog::UserEventHandler(
     Event *event
) {
     BOOL message_processed = TRUE;

     switch (event->wParam) {
     case IDD_GPE_EXP_COMMENT_LOAD:
          Load();
          break;

     default:
          message_processed = FALSE;
          break;
     }

     return message_processed;
}

void
GpeExpCommentDialog::Load(
     void
) {
     FILE           *fp_comment;
     BOOL           ok;
     char           szFilterSpec[128];
     char           tmp_buff[128];

     strcpy(tmp_buff, szCommentFilename);
     strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_TXT));
     ConvertFilterSpec(szFilterSpec);
     StdFileOpenDialog dialog(     Instance(), Handle(),
                                   GetIdt(IDT_GPE_LOAD_COMMENT_DLG_TITLE),
                                   tmp_buff, sizeof(tmp_buff),
                                   COMMENT_FILE_EXTENSION,
                                   szFilterSpec
                              );
     ok = dialog.ProcessMessages();

     if (ok) {
          Cursor cursor(IDC_WAIT);
          cursor.Select();
          {
               if ((fp_comment = fopen(tmp_buff, "rb")) != NULL) {
                    HLOCAL         hOldMem;
                    HLOCAL         hNewMem = LocalAlloc(LMEM_MOVEABLE, TEXT_BUFF_SIZE);
                    char           *pszLocal;
                    unsigned short count;

                    if (hNewMem) {
                         hOldMem = (HLOCAL) SendDlgItemMessage(Handle(), IDD_GPE_EXP_COMMENT, EM_GETHANDLE, 0, 0L);
                         (void) LocalFree(hOldMem);

                         pszLocal = (char *) LocalLock(hNewMem);
                         {
                              count = fread(pszLocal, 1, TEXT_BUFF_SIZE - 1, fp_comment);
                              pszLocal[count + 1] = '\0';
                              (void) LocalReAlloc(hNewMem, count + 1, 0);
                         }
                         (void) LocalUnlock(hNewMem);

                         (void) SendDlgItemMessage(Handle(), IDD_GPE_EXP_COMMENT, EM_SETHANDLE, (WPARAM) hNewMem, 0L);
                    }
                    else {
                         ok = FALSE;
                         
                         strcpy(msg_buff, GetAppName());
                         MsgBoxError mbe(msg_buff, GetIdt(IDT_OUT_OF_MEMORY));
                         (void) mbe.ProcessMessages();
                    }

                    (void) fclose(fp_comment);
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
          strcpy(szCommentFilename, tmp_buff);

          ProfileString ps(APP_INIFILE_KEYWORD, APP_INIFILE);
          (void) ps.Write(APP_PROF_STR_COMMENT_FILENAME, szCommentFilename);
     }
}

void
GpeExpCommentDialog::OK(
     void
) {
     HLOCAL         hMem;
     char           *pszLocal;
     unsigned short cnt;
     BOOL           ok = FALSE;
     Cursor         cursor(IDC_WAIT);

     cursor.Select();
     {
          hMem = (HLOCAL) SendDlgItemMessage(Handle(), IDD_GPE_EXP_COMMENT, EM_GETHANDLE, 0, 0L);
          pszLocal = (char *) LocalLock(hMem);
          {
               cnt = strlen(pszLocal);

               if (cnt) {
                    rec.type  = GPE_RT_COMMENT;
                    rec.size  = cnt;
                    rec.crc   = CalcCrc(pszLocal, cnt);

                    ok = fwrite(&rec, sizeof(rec), 1, fp);
                    if (ok) {
                         ok = fwrite(pszLocal, cnt, 1, fp);
                    }
               }
               else {
                    ok = TRUE;
               }
          }
          (void) LocalUnlock(hMem);
     }
     cursor.Deselect();

     *pOk = ok;
     End(TRUE);
}

/*---------------------------------------------------------------------------
** End of File
*/

