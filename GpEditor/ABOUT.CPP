/*
** File   : about.cpp
** Author : TK 
** Date   :  1/01/94
**
** $Header:   L:/GPEDITOR/VCS/ABOUT.CP_   1.3   01 Oct 1994 15:28:00   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** About dialog box.
*/

/*lint -e713 -e732 -e737 Ignore signed/unsigned */
/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <cppwin.hpp>
/*lint +elib(???) */
#include "GPeditor.hpp"
#include "GPeditor.h"
#pragma hdrstop

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

class AboutGpDialog : public DialogWindow {
public:
     AboutGpDialog( HINSTANCE inst, HWND parent, BOOL fGameLoaded,
                    char const *pszExeVerString, long lExeOffset);
     ~AboutGpDialog();

private:
     void Initialise(Event *event);

     StaticText     *pSt_game_exe_version;
     char const     *pszGameExeVer;
     long           lGameExeOffset;
     char           msg_buff[128];
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

/*---------------------------------------------------------------------------
** Purpose:    Display 'about' dialogue.
**
** Returns:    Nothing.
**
** Notes  :    
*/
void
AppWindow::About(
     void     
) {
     AboutGpDialog   about_dialog( Instance(), Handle(),
                                   fGpExeIsFound,
                                   game_exe_version,
                                   lGameExeColourFileOffset
                              );
     (void) about_dialog.ProcessMessages();
}

AboutGpDialog::AboutGpDialog(
     HINSTANCE      inst,                    /* In  Instance handle        */
     HWND           parent,                  /* In  Parent handle          */
     BOOL           fGameLoaded,             /* In  If game EXE is loaded  */
     char const     *pszExeVerString,        /* In  Version of game EXE    */
     long           lExeOffset               /* In  Colours offset in EXE  */
) : DialogWindow(inst, parent, GetDlgTemplateName(APP_DIALOG_ABOUT)) {
     pSt_game_exe_version = NULL;
     if (fGameLoaded) {
          pszGameExeVer  = pszExeVerString;
          lGameExeOffset = lExeOffset;
     }
     else {
          pszGameExeVer = NULL;
     }
}

AboutGpDialog::~AboutGpDialog(
     void
) {
     if (pSt_game_exe_version != NULL) {
          delete pSt_game_exe_version;
          pSt_game_exe_version = NULL;
     }
}

void
AboutGpDialog::Initialise(
     Event *
) {
     if (pszGameExeVer) {
          pSt_game_exe_version = new StaticText(IDD_ABOUT_GAME_EXE_VERSION, Handle());

          sprintf(msg_buff, GetIdt(IDT_ABOUT_GAME_EXE_VERSION), pszGameExeVer, lGameExeOffset);
          pSt_game_exe_version->SetText(msg_buff);
     }
}

/*---------------------------------------------------------------------------
** End of File
*/

