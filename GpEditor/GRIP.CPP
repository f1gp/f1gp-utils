/*
** File   : grip.cpp
** Author : TK 
** Date   :  6/03/94
**
** $Header:   L:/GPEDITOR/VCS/GRIP.CP_   1.4   01 Oct 1994 15:28:04   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** AI Car grip.
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

#define NORMAL_OFFSET         0x4000

#define OFFSET_TO_GRIP(v)     ((((v) - NORMAL_OFFSET) / 100) + 1)
#define GRIP_TO_OFFSET(v)     (((((v) - 1) * 100) + NORMAL_OFFSET))

#define NORMAL_GRIP           1

#define MAX_POS               100
#define MIN_POS               1
#define PER_LINE              1
#define PER_PAGE              5

/*---------------------------------------------------------------------------
** Typedefs
*/

class AiCarGripDialog : public DialogWindow {
public:
     AiCarGripDialog(HINSTANCE inst, HWND parent);
     ~AiCarGripDialog();

private:
     void Initialise(Event *event);
     BOOL UserEventHandler(Event *event);
     void ScrollBar(EventScrollBar *event);
     void OK();
     void UpdateGrip(unsigned short grip);

     ScrollBarControl    *sb_grip;
     StaticText          *st_text;
     char                msg_buff[128];
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
AppWindow::AiCarGrip(
     void
) {
     AiCarGripDialog dialog(Instance(), Handle());
     (void) dialog.ProcessMessages();
}

AiCarGripDialog::AiCarGripDialog(
     HINSTANCE                inst,          /* In  Instance handle        */
     HWND                     parent         /* In  Parent handle          */
) : DialogWindow(inst, parent, GetDlgTemplateName(APP_DIALOG_AI_CAR_GRIP)) {
     sb_grip = NULL;
     st_text = NULL;
}

AiCarGripDialog::~AiCarGripDialog(
     void
) {
     if (sb_grip) {
          delete sb_grip;
          sb_grip = NULL;
     }
     if (st_text) {
          delete st_text;
          st_text = NULL;
     }
}

void
AiCarGripDialog::Initialise(
     Event *
) {
     sb_grip = new ScrollBarControl(Handle(), IDD_AI_CAR_GRIP_SB, MAX_POS - MIN_POS, PER_PAGE, PER_LINE, MIN_POS);
     st_text = new StaticText(IDD_AI_CAR_GRIP_TEXT, Handle());

     UpdateGrip(OFFSET_TO_GRIP(GP_EXE_AiCarGrip));
}

BOOL
AiCarGripDialog::UserEventHandler(
     Event *event
) {
     BOOL message_processed = TRUE;

     switch (event->wParam) {
     case IDD_AI_CAR_GRIP_NORMAL:
          UpdateGrip(NORMAL_GRIP);
          break;

     default:
          message_processed = FALSE;
          break;
     }

     return message_processed;
}

void
AiCarGripDialog::ScrollBar(
     EventScrollBar *event
) {
     if ((HWND) event->Id() == GetDlgItem(Handle(), IDD_AI_CAR_GRIP_SB)) {
          sb_grip->Update(event);
          UpdateGrip(sb_grip->GetThumbPos());
     }
}

void
AiCarGripDialog::OK(
     void
) {
     GP_EXE_AiCarGrip = GRIP_TO_OFFSET(sb_grip->GetThumbPos());
     End();
}

void
AiCarGripDialog::UpdateGrip(
     unsigned short grip
) {
     sb_grip->SetThumbPos(grip);
     sprintf(msg_buff, "%u", grip);
     st_text->SetText(msg_buff);
}

/*---------------------------------------------------------------------------
** End of File
*/

