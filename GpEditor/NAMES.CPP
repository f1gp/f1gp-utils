/*
** File   : names.cpp
** Author : TK 
** Date   : 26/02/94
**
** $Header:   L:/GPEDITOR/VCS/NAMES.CP_   1.4   01 Oct 1994 15:27:58   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Team/Driver name handling.
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
#include <ctype.h>
#include <stdfopen.hpp>
#include <io.h>
/*lint +elib(???) */
#include "cntrlbar.hpp"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define NAM_FILE_EXTENSION    "NAM"

/*---------------------------------------------------------------------------
** Typedefs
*/

class NamesDialog : public DialogWindow {
public:
     NamesDialog(HINSTANCE inst, HWND parent);
     ~NamesDialog();

private:
     void Initialise(Event *event);
     BOOL UserEventHandler(Event *event);
     void OK();
     void SelectEntry(unsigned short team_index);

     ComboBox                      *cb_teams;
     EditTextWhiteSpaceStripped    *et_engine;
     EditTextWhiteSpaceStripped    *et_driver1;
     EditTextWhiteSpaceStripped    *et_driver2;
     StaticText                    *st_driver1;
     StaticText                    *st_driver2;

     unsigned short                usTeamIndex;

     GP_NAMES                      names;
     char                          msg_buff[128];
};

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/

static void CalculateNamesCrc(GP_NAMES *gp_names);
static void rotate_left3(unsigned short *ptr_us);

/*---------------------------------------------------------------------------
** Purpose:    Attempts to open the user specified driver/team/engine name
**             file.
**
** Returns:    True if opened OK, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::FileOpenNames(
     char *filename
) {
     BOOL ok = TRUE;
     FILE *fp;
     char szFilterSpec[128];

     if (filename) {
          strcpy(tmp_buff, filename);
     }
     else {
          strcpy(tmp_buff, szNamesFilename);
          strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_NAM));
          ConvertFilterSpec(szFilterSpec);
          StdFileOpenDialog dialog(     Instance(), Handle(),
                                        GetIdt(IDT_OPEN_NAMES_DLG_TITLE),
                                        tmp_buff, sizeof(tmp_buff),
                                        NAM_FILE_EXTENSION, szFilterSpec
                                   );
          ok = dialog.ProcessMessages();
     }

     if (ok) {
          Cursor cursor(IDC_WAIT);
          cursor.Select();
          {
               if ((fp = fopen(tmp_buff, "rb")) != NULL) {
                    ok = (fread(&GP_names, sizeof(GP_names), 1, fp) == 1);
                    fclose(fp);
               }
               else {
                    ok = FALSE;
                    sprintf(msg_buff, GetIdt(IDT_UNABLE_TO_OPEN), tmp_buff);
                    MsgBoxWarning mb(GetAppName(), msg_buff);
                    mb.ProcessMessages();
               }

               /*
               ** Validate information.
               */
               unsigned short j;
               unsigned short i;
               unsigned char  c;

               for (j = 0; ok && j < NUM_ELEMENTS(GP_names.drivers); j++) {
                    for (i = 0; ok && i < sizeof(GP_names.drivers[j].driver); i++) {
                         c = GP_names.drivers[j].driver[i];
                         if (c == '\0') {
                              break;
                         }
                         if (!isascii(c)) {
                              ok = FALSE;
                         }
                    }
               }
               if (c != '\0') {
                    ok = FALSE;
               }

               for (j = 0; ok && j < NUM_ELEMENTS(GP_names.teams); j++) {
                    for (i = 0; ok && i < sizeof(GP_names.teams[j].team); i++) {
                         c = GP_names.teams[j].team[i];
                         if (c == '\0') {
                              break;
                         }
                         if (!isascii(c)) {
                              ok = FALSE;
                         }
                    }
               }
               if (c != '\0') {
                    ok = FALSE;
               }

               for (j = 0; ok && j < NUM_ELEMENTS(GP_names.engines); j++) {
                    for (i = 0; ok && i < sizeof(GP_names.engines[j].engine); i++) {
                         c = GP_names.engines[j].engine[i];
                         if (c == '\0') {
                              break;
                         }
                         if (!isascii(c)) {
                              ok = FALSE;
                         }
                    }
               }
               if (c != '\0') {
                    ok = FALSE;
               }
          }
          cursor.Deselect();
     }

     if (ok) {
          strcpy(szNamesFilename, tmp_buff);
          (void) pProfile->Write(APP_PROF_STR_NAMES_FILENAME, szNamesFilename);
          fNamesLoaded = TRUE;
          UpdateMenu();

          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_LOAD_NAMES, 0, 0L);
          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_LOAD_DRIVER_NAMES, 0, 0L);
     }

     return ok;
}

BOOL
AppWindow::FileSaveNames(
     void
) {
     FILE           *fp;
     BOOL           ok;
     char           szFilterSpec[128];

     strcpy(tmp_buff, szNamesFilename);
     strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_NAM));
     ConvertFilterSpec(szFilterSpec);
     StdFileOpenDialog dialog(     Instance(), Handle(),
                                   GetIdt(IDT_SAVE_NAMES_DLG_TITLE),
                                   tmp_buff, sizeof(tmp_buff),
                                   NAM_FILE_EXTENSION,
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
                    CalculateNamesCrc(&GP_names);
                    ok = fwrite(&GP_names, sizeof(GP_names), 1, fp);

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
          strcpy(szNamesFilename, tmp_buff);
          (void) pProfile->Write(APP_PROF_STR_NAMES_FILENAME, szNamesFilename);
     }

     return ok;
}

void
AppWindow::Names(
     void
) {
     NamesDialog dialog(Instance(), Handle());

     if (dialog.ProcessMessages()) {
          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_LOAD_NAMES, 0, 0L);
          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_LOAD_DRIVER_NAMES, 0, 0L);
     }
}

NamesDialog::NamesDialog(
     HINSTANCE                inst,          /* In  Instance handle        */
     HWND                     parent         /* In  Parent handle          */
) : DialogWindow(inst, parent, GetDlgTemplateName(APP_DIALOG_NAMES)) {
     cb_teams       = NULL;
     et_engine      = NULL;
     et_driver1     = NULL;
     et_driver2     = NULL;
     st_driver1     = NULL;
     st_driver2     = NULL;
}

NamesDialog::~NamesDialog(
     void
) {
     if (cb_teams) {
          delete cb_teams;
          cb_teams = NULL;
     }
     if (et_engine) {
          delete et_engine;
          et_engine = NULL;
     }
     if (et_driver1) {
          delete et_driver1;
          et_driver1 = NULL;
     }
     if (et_driver2) {
          delete et_driver2;
          et_driver2 = NULL;
     }
     if (st_driver1) {
          delete st_driver1;
          st_driver1 = NULL;
     }
     if (st_driver2) {
          delete st_driver2;
          st_driver2 = NULL;
     }
}

void
NamesDialog::Initialise(
     Event *
) {
     unsigned short i;

     cb_teams       = new ComboBox(IDD_NAMES_TEAM, Handle());
     et_engine      = new EditTextWhiteSpaceStripped(IDD_NAMES_ENGINE, Handle());
     et_driver1     = new EditTextWhiteSpaceStripped(IDD_NAMES_DRIVER1, Handle());
     et_driver2     = new EditTextWhiteSpaceStripped(IDD_NAMES_DRIVER2, Handle());
     st_driver1     = new StaticText(IDD_NAMES_DRIVER1_TEXT, Handle());
     st_driver2     = new StaticText(IDD_NAMES_DRIVER2_TEXT, Handle());

     memcpy(&names, &GP_names, sizeof(names));

     for (i = 0; i < NUM_ELEMENTS(names.teams); i++) {
          (void) cb_teams->AddString(names.teams[i].team);
     }

     (void) SendMessage(cb_teams->Handle(), CB_LIMITTEXT, sizeof(names.teams[0].team) - 1, 0L);
     et_engine->SetMaxFieldLength(sizeof(names.engines[0].engine) - 1);
     et_driver1->SetMaxFieldLength(sizeof(names.drivers[0].driver) - 1);
     et_driver2->SetMaxFieldLength(sizeof(names.drivers[0].driver) - 1);

     SelectEntry(0);
}

BOOL
NamesDialog::UserEventHandler(
     Event *event
) {
     BOOL           message_processed = TRUE;
     unsigned short driver_index;

     switch (event->wParam) {
     case IDD_NAMES_TEAM:
          if (HIWORD(event->lParam) == CBN_SELCHANGE) {
               /*
               ** Update edited string.
               */
               unsigned short usNewSelection = cb_teams->GetCurrentSelection();

               (void) GetWindowText(cb_teams->Handle(), msg_buff, sizeof(msg_buff));
               (void) cb_teams->DeleteString(usTeamIndex);
               (void) cb_teams->AddString(msg_buff, usTeamIndex);
               memset(names.teams[usTeamIndex].team, 0x00, sizeof(names.teams[usTeamIndex].team));
               strcpy(names.teams[usTeamIndex].team, msg_buff);

               SelectEntry(usNewSelection);
          }
          else if (HIWORD(event->lParam) == CBN_KILLFOCUS) {
               (void) GetWindowText(cb_teams->Handle(), msg_buff, sizeof(msg_buff));
               (void) cb_teams->DeleteString(usTeamIndex);
               (void) cb_teams->AddString(msg_buff, usTeamIndex);
               (void) cb_teams->SetCurrentSelection(usTeamIndex);
               memset(names.teams[usTeamIndex].team, 0x00, sizeof(names.teams[usTeamIndex].team));
               strcpy(names.teams[usTeamIndex].team, msg_buff);
          }
          break;

     case IDD_NAMES_ENGINE:
          if (HIWORD(event->lParam) == EN_KILLFOCUS) {
               (void) et_engine->Get(msg_buff, sizeof(msg_buff));
               memset(names.engines[usTeamIndex].engine, 0x00, sizeof(names.engines[usTeamIndex].engine));
               strcpy(names.engines[usTeamIndex].engine, msg_buff);
               et_engine->Set(msg_buff);
          }
          break;

     case IDD_NAMES_DRIVER1:
          if (HIWORD(event->lParam) == EN_KILLFOCUS) {
               driver_index = (usTeamIndex * 2) + 0;
               if (IsDriverEnabled(driver_index)) {
                    (void) et_driver1->Get(msg_buff, sizeof(msg_buff));
                    memset(names.drivers[DriverIndex(driver_index)].driver, 0x00, sizeof(names.drivers[DriverIndex(driver_index)].driver));
                    strcpy(names.drivers[DriverIndex(driver_index)].driver, msg_buff);
                    et_driver1->Set(msg_buff);
               }
          }
          break;

     case IDD_NAMES_DRIVER2:
          if (HIWORD(event->lParam) == EN_KILLFOCUS) {
               driver_index = (usTeamIndex * 2) + 1;
               if (IsDriverEnabled(driver_index)) {
                    (void) et_driver2->Get(msg_buff, sizeof(msg_buff));
                    memset(names.drivers[DriverIndex(driver_index)].driver, 0x00, sizeof(names.drivers[DriverIndex(driver_index)].driver));
                    strcpy(names.drivers[DriverIndex(driver_index)].driver, msg_buff);
                    et_driver2->Set(msg_buff);
               }
          }
          break;

     default:
          message_processed = FALSE;
          break;
     }

     return message_processed;
}

void
NamesDialog::SelectEntry(
     unsigned short team_index
) {
     unsigned short driver_index;

     ASSERT(team_index < NUM_ELEMENTS(names.teams));

     usTeamIndex = team_index;
     
     (void) cb_teams->SetCurrentSelection(usTeamIndex);

     et_engine->SetText(names.engines[usTeamIndex].engine);

     driver_index = (usTeamIndex * 2) + 0;
     if (IsDriverEnabled(driver_index)) {
          et_driver1->SetText(names.drivers[DriverIndex(driver_index)].driver);
          et_driver1->Enable();
          st_driver1->Enable();
     }
     else {
          et_driver1->SetText("---");
          et_driver1->Disable();
          st_driver1->Disable();
     }

     driver_index = (usTeamIndex * 2) + 1;
     if (IsDriverEnabled(driver_index)) {
          et_driver2->SetText(names.drivers[DriverIndex(driver_index)].driver);
          et_driver2->Enable();
          st_driver2->Enable();
     }
     else {
          et_driver2->SetText("---");
          et_driver2->Disable();
          st_driver2->Disable();
     }
}

void
NamesDialog::OK(
     void
) {
     memcpy(&GP_names, &names, sizeof(GP_names));
     End(TRUE);
}

static void
CalculateNamesCrc(
     GP_NAMES       *gp_names           /* I/O  Names to calculate CRC for */
) {
     unsigned short i;
     unsigned short crc1 = 0;
     unsigned short crc2 = 0;
     unsigned char  *p = (unsigned char *) gp_names;

     for (i = 0; i < sizeof(*gp_names) - (sizeof(gp_names->crc1) + sizeof(gp_names->crc2)); i++) {
          crc1 += *p;
          rotate_left3(&crc2);
          crc2 += *p;
          ++p;
     }

     gp_names->crc1 = crc1;
     gp_names->crc2 = crc2;
}

static void
rotate_left3(
     unsigned short *ptr_us
) {
     unsigned long us = *ptr_us;

     us <<= 3;

     us |= (us >> 16) & 0x00000007U;
     *ptr_us = us;
}

/*---------------------------------------------------------------------------
** End of File
*/

