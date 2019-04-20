/*
** File   : lang.cpp
** Author : TK 
** Date   : 24/02/94
**
** $Header:   L:/GPEDITOR/VCS/LANG.CP_   1.3   24 Jun 1995 11:36:50   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Language dialog box.
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

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

class LanguageDialog : public DialogWindow {
public:
     LanguageDialog(HINSTANCE inst, HWND parent);
     ~LanguageDialog();

private:
     void      Initialise(Event *event);
     BOOL      UserEventHandler(Event *event);
     void      OK(void);
     void      UpdateDlgCaption(void);

     RadioButtons   *rb;
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

void
AppWindow::Language(
     void
) {
     LANGUAGE original_language = CurrentLanguage;

     LanguageDialog dialog(Instance(), Handle());

     if (dialog.ProcessMessages()) {
          /*
          ** Redraw current caption in new language.
          */
          UpdateCaption(NULL);

          SelectNewMenu();
          
          /*
          ** Setup default names with new language.
          */
          if (!fNamesLoaded) {
               InitDefaultNames();
          }
     }
     else {
          /*
          ** If user quits restore original language.
          */
          CurrentLanguage = original_language;
     }

     /*
     ** Allways update profile string (this ensures it is written the
     ** very first time).
     */
     (void) pProfile->Write(APP_PROF_STR_LANGUAGE, CurrentLanguage);
}

LanguageDialog::LanguageDialog(
     HINSTANCE      inst,                    /* In  Instance handle        */
     HWND           parent                   /* In  Parent handle          */
) : DialogWindow(inst, parent, APP_DIALOG_LANGUAGE) {
     rb = NULL;
}

LanguageDialog::~LanguageDialog(
     void
) {
     if (rb) {
          delete rb;
          rb = NULL;
     }
}

void
LanguageDialog::Initialise(
     Event *
) {
     rb = new RadioButtons(IDD_LANGUAGE_RB_ENGLISH, IDD_LANGUAGE_RB_GERMAN, IDD_LANGUAGE_RB_ENGLISH, Handle());

     switch (CurrentLanguage) {
     case LANG_NOT_SUPPORTED:
     default:
     case LANG_ENGLISH:
          rb->SetActiveButton(IDD_LANGUAGE_RB_ENGLISH);
          break;

     case LANG_FRENCH:
          rb->SetActiveButton(IDD_LANGUAGE_RB_FRENCH);
          break;

     case LANG_GERMAN:
          rb->SetActiveButton(IDD_LANGUAGE_RB_GERMAN);
          break;
     }

     /*
     ** Now we know the language update the caption.
     */
     UpdateDlgCaption();
}

BOOL
LanguageDialog::UserEventHandler(
     Event *event
) {
     BOOL message_processed = TRUE;

     switch (event->wParam) {
     case IDD_LANGUAGE_RB_ENGLISH:
     case IDD_LANGUAGE_RB_FRENCH:
     case IDD_LANGUAGE_RB_GERMAN:
          rb->SetActiveButton(event->wParam);
          switch (event->wParam) {
          default:
          case IDD_LANGUAGE_RB_ENGLISH:
               CurrentLanguage = LANG_ENGLISH;
               break;

          case IDD_LANGUAGE_RB_FRENCH:
               CurrentLanguage = LANG_FRENCH;
               break;

          case IDD_LANGUAGE_RB_GERMAN:
               CurrentLanguage = LANG_GERMAN;
               break;
          }
          UpdateDlgCaption();
          break;

     default:
          message_processed = FALSE;
          break;
     }

     return message_processed;
}

void
LanguageDialog::UpdateDlgCaption(
     void
) {
     char buff[80];

     strcpy(buff, GetIdt(IDT_LANGUAGE));
     sprintf(msg_buff, "%s - %s", GetAppName(), buff);
     SetWindowText(Handle(), msg_buff);

     SetWindowText(GetDlgItem(Handle(), 2), GetIdt(IDT_CANCEL));
}

void
LanguageDialog::OK(
     void
) {
     End(TRUE);
}

/*---------------------------------------------------------------------------
** End of File
*/

