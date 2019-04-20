/*
** File   : player.cpp
** Author : TK 
** Date   :  7/03/94
**
** $Header:   L:/GPEDITOR/VCS/PLAYER.CP_   1.3   01 Oct 1994 15:28:00   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Players car performance.
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

#define MEM_VAL_TO_BHP(mval)  (((mval) - 632U) / 22U)
#define BHP_TO_MEM_VAL(bhp)   (((bhp) * 22U) + 632U)

#define NORMAL_PERFORMANCE    716

#define MAX_POS               999
#define MIN_POS               450
#define PER_LINE              1
#define PER_PAGE              10

/*---------------------------------------------------------------------------
** Typedefs
*/

class PlayersCarPerfDialog : public DialogWindow {
public:
     PlayersCarPerfDialog(HINSTANCE inst, HWND parent);
     ~PlayersCarPerfDialog();

private:
     void Initialise(Event *event);
     BOOL UserEventHandler(Event *event);
     void ScrollBar(EventScrollBar *event);
     void OK();
     void UpdateGrip(unsigned short grip);

     ScrollBarControl    *sb_perf;
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
AppWindow::PlayersCarPerformance(
     void
) {
     PlayersCarPerfDialog dialog(Instance(), Handle());
     (void) dialog.ProcessMessages();
}

PlayersCarPerfDialog::PlayersCarPerfDialog(
     HINSTANCE                inst,          /* In  Instance handle        */
     HWND                     parent         /* In  Parent handle          */
) : DialogWindow(inst, parent, GetDlgTemplateName(APP_DIALOG_PLAYERS_CAR_PERF)) {
     sb_perf = NULL;
     st_text = NULL;
}

PlayersCarPerfDialog::~PlayersCarPerfDialog(
     void
) {
     if (sb_perf) {
          delete sb_perf;
          sb_perf = NULL;
     }
     if (st_text) {
          delete st_text;
          st_text = NULL;
     }
}

void
PlayersCarPerfDialog::Initialise(
     Event *
) {
     sb_perf = new ScrollBarControl(Handle(), IDD_PLAYERS_CAR_PERF_SB, MAX_POS - MIN_POS, PER_PAGE, PER_LINE, MIN_POS);
     st_text = new StaticText(IDD_PLAYERS_CAR_PERF_TEXT, Handle());

     UpdateGrip(MEM_VAL_TO_BHP(GP_EXE_PlayersCarPerf));
}

BOOL
PlayersCarPerfDialog::UserEventHandler(
     Event *event
) {
     BOOL message_processed = TRUE;

     switch (event->wParam) {
     case IDD_PLAYERS_CAR_PERF_NORMAL:
          UpdateGrip(NORMAL_PERFORMANCE);
          break;

     default:
          message_processed = FALSE;
          break;
     }

     return message_processed;
}

void
PlayersCarPerfDialog::ScrollBar(
     EventScrollBar *event
) {
     if ((HWND) event->Id() == GetDlgItem(Handle(), IDD_PLAYERS_CAR_PERF_SB)) {
          sb_perf->Update(event);
          UpdateGrip(sb_perf->GetThumbPos());
     }
}

void
PlayersCarPerfDialog::OK(
     void
) {
     GP_EXE_PlayersCarPerf = BHP_TO_MEM_VAL(sb_perf->GetThumbPos());
     End();
}

void
PlayersCarPerfDialog::UpdateGrip(
     unsigned short grip
) {
     sb_perf->SetThumbPos(grip);
     sprintf(msg_buff, "%u", grip);
     st_text->SetText(msg_buff);
}

/*---------------------------------------------------------------------------
** End of File
*/

