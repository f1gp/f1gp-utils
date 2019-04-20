/*
** File   : design.cpp
** Author : TK 
** Date   : 26/02/94
**
** $Header:   L:/GPEDITOR/VCS/DESIGN.CP_   1.2   01 Oct 1994 15:27:46   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Design handling.
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
#include <io.h>
/*lint +elib(???) */
#include "cntrlbar.hpp"
#include "drawarea.hpp"
#include "drawhelm.hpp"
#include "palette.hpp"
#include "teams.hpp"

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

/*---------------------------------------------------------------------------
** Functions
*/

/*---------------------------------------------------------------------------
** Purpose:    Clears the current design.
**
** Returns:    Nothing.
**
** Notes  :    Updates the caption.
*/
void
AppWindow::DesignNew(
     void
) {
     (void) SendMessage(pDrawingWindow->Handle(), IDE_DESIGN_NEW, 0, 0L);

     if (!fGpExeIsFound) {
          fDesignLoaded     = FALSE;
          fCarEditorEnabled = TRUE;
          SwitchEditor();
     }
}

/*---------------------------------------------------------------------------
** Purpose:    Attempts to open the user specified car design file.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::DesignLoad(
     void
) {
     BOOL ok;
     char szFilterSpec[128];

     fCarEditorEnabled = TRUE;
     SwitchEditor();

     strcpy(tmp_buff, szDesignFilename);
     strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_CAR));
     ConvertFilterSpec(szFilterSpec);
     StdFileOpenDialog dialog(     Instance(), Handle(),
                                   GetIdt(IDT_OPEN_CAR_DESIGN_DLG_TITLE),
                                   tmp_buff, sizeof(tmp_buff),
                                   "CAR", szFilterSpec
                              );
     ok = dialog.ProcessMessages();

     if (ok) {
          ok = (BOOL) SendMessage(pDrawingWindow->Handle(), IDE_DESIGN_LOAD, 0, (LONG) (void *) tmp_buff);
     }

     if (ok) {
          strcpy(szDesignFilename, tmp_buff);
          (void) pProfile->Write(APP_PROF_STR_CAR_FILENAME, szDesignFilename);

          fDesignLoaded = TRUE;
          if (!fGpExeIsFound) {
               SwitchEditor();
          }
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Saves the current car design file if the user specified file.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::DesignSave(
     void
) {
     BOOL ok;
     char szFilterSpec[128];

     fCarEditorEnabled = TRUE;
     SwitchEditor();

     strcpy(tmp_buff, szDesignFilename);
     strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_CAR));
     ConvertFilterSpec(szFilterSpec);
     StdFileOpenDialog dialog(     Instance(), Handle(),
                                   GetIdt(IDT_SAVE_CAR_DESIGN_DLG_TITLE),
                                   tmp_buff, sizeof(tmp_buff),
                                   "CAR", szFilterSpec
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
          ok = (BOOL) SendMessage(pDrawingWindow->Handle(), IDE_DESIGN_SAVE, 0, (LONG) (void *) tmp_buff);
     }

     if (ok) {
          strcpy(szDesignFilename, tmp_buff);
          (void) pProfile->Write(APP_PROF_STR_CAR_FILENAME, szDesignFilename);
          if (!fGpExeIsFound) {
               SwitchEditor();
          }
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Clears the current helmet design.
**
** Returns:    Nothing.
**
** Notes  :    Updates the caption.
*/
void
AppWindow::HelmetNew(
     void
) {
     (void) SendMessage(pHelmetWindow->Handle(), IDE_HELMET_NEW, 0, 0L);

     if (!fGpExeIsFound) {
          fHelmetLoaded     = FALSE;
          fCarEditorEnabled = FALSE;
          SwitchEditor();
     }
}

/*---------------------------------------------------------------------------
** Purpose:    Attempts to open the user specified helmet design file.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::HelmetLoad(
     void
) {
     BOOL ok;
     char szFilterSpec[128];

     fCarEditorEnabled = FALSE;
     SwitchEditor();

     strcpy(tmp_buff, szHelmetFilename);
     strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_HEL));
     ConvertFilterSpec(szFilterSpec);
     StdFileOpenDialog dialog(     Instance(), Handle(),
                                   GetIdt(IDT_OPEN_HELMET_DLG_TITLE),
                                   tmp_buff, sizeof(tmp_buff),
                                   "HEL", szFilterSpec
                              );
     ok = dialog.ProcessMessages();

     if (ok) {
          ok = (BOOL) SendMessage(pHelmetWindow->Handle(), IDE_HELMET_LOAD, 0, (LONG) (void *) tmp_buff);
     }

     if (ok) {
          strcpy(szHelmetFilename, tmp_buff);
          (void) pProfile->Write(APP_PROF_STR_HELMET_FILENAME, szHelmetFilename);

          fHelmetLoaded = TRUE;
          if (!fGpExeIsFound) {
               SwitchEditor();
          }
     }

     return ok;
}

/*---------------------------------------------------------------------------
** Purpose:    Saves the current helmet design file if the user specified file.
**
** Returns:    True if successful, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::HelmetSave(
     void
) {
     BOOL ok;
     char szFilterSpec[128];

     fCarEditorEnabled = FALSE;
     SwitchEditor();

     strcpy(tmp_buff, szHelmetFilename);
     strcpy(szFilterSpec, GetIdt(IDT_COMDLG_FILE_TEMPLATE_HEL));
     ConvertFilterSpec(szFilterSpec);
     StdFileOpenDialog dialog(     Instance(), Handle(),
                                   GetIdt(IDT_SAVE_HELMET_DLG_TITLE),
                                   tmp_buff, sizeof(tmp_buff),
                                   "HEL", szFilterSpec
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
          ok = (BOOL) SendMessage(pHelmetWindow->Handle(), IDE_HELMET_SAVE, 0, (LONG) (void *) tmp_buff);
     }

     if (ok) {
          strcpy(szHelmetFilename, tmp_buff);
          (void) pProfile->Write(APP_PROF_STR_HELMET_FILENAME, szHelmetFilename);
          if (!fGpExeIsFound) {
               SwitchEditor();
          }
     }

     return ok;
}

/*---------------------------------------------------------------------------
** End of File
*/

