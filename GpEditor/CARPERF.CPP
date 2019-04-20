/*
** File   : carperf.cpp
** Author : TK 
** Date   :  3/01/94
**
** $Header:   L:/GPEDITOR/VCS/CARPERF.CP_   1.5   01 Oct 1994 15:27:58   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Car Performance.
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

#define MAX_POWER   999
#define MIN_POWER   500
#define PER_LINE    1
#define PER_PAGE    10

/*---------------------------------------------------------------------------
** Typedefs
*/

class CarPerfomanceDialog : public DialogWindow {
public:
     CarPerfomanceDialog(HINSTANCE inst, HWND parent);
     virtual ~CarPerfomanceDialog();

private:
     void Initialise(Event *event);
     BOOL UserEventHandler(Event *event);
     void ScrollBar(EventScrollBar *event);
     void OK();

     StaticText          *st_team[GP_EXE_NUM_TEAMS];
     StaticText          *st_power[GP_EXE_NUM_TEAMS];
     ScrollBarControl    *sb_perf[GP_EXE_NUM_TEAMS];
     CheckBox            *cb_scroll_all;

     char                msg_buff[128];
     unsigned short      bhps[GP_EXE_NUM_TEAMS];
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
AppWindow::CarPerformance(
     void
) {
     CarPerfomanceDialog dialog(Instance(), Handle());

     (void) dialog.ProcessMessages();
}

CarPerfomanceDialog::CarPerfomanceDialog(
     HINSTANCE      inst,                    /* In  Instance handle        */
     HWND           parent                   /* In  Parent handle          */
) : DialogWindow(inst, parent, GetDlgTemplateName(APP_DIALOG_CAR_PERFORMANCE)) {
     #pragma warn -rch
     #pragma warn -ccc
     ASSERT(NUM_ELEMENTS(GP_names.teams) == NUM_ELEMENTS(st_team));
     ASSERT(NUM_ELEMENTS(GP_EXE_car_power) == NUM_ELEMENTS(st_power));
     ASSERT(NUM_ELEMENTS(GP_EXE_car_power) == NUM_ELEMENTS(sb_perf));
     #pragma warn -ccc
     #pragma warn -rch

     memset(st_team, 0x00, sizeof(st_team));
     memset(st_power, 0x00, sizeof(st_power));
     memset(sb_perf, 0x00, sizeof(sb_perf));
     cb_scroll_all = NULL;
}

CarPerfomanceDialog::~CarPerfomanceDialog(
     void
) {
     unsigned short i;

     for (i = 0; i < NUM_ELEMENTS(st_team); i++) {
          if (st_team[i]) {
               delete st_team[i];
          }
     }

     for (i = 0; i < NUM_ELEMENTS(st_power); i++) {
          if (st_power[i]) {
               delete st_power[i];
          }
     }

     for (i = 0; i < NUM_ELEMENTS(sb_perf); i++) {
          if (sb_perf[i]) {
               delete sb_perf[i];
          }
     }

     if (cb_scroll_all) {
          delete cb_scroll_all;
     }
}

void
CarPerfomanceDialog::Initialise(
     Event *
) {
     unsigned short i;

     for (i = 0; i < NUM_ELEMENTS(st_team); i++) {
          st_team[i]  = new StaticText(IDD_CAR_PERF_TEAM_01 + i, Handle());
          st_power[i] = new StaticText(IDD_CAR_PERF_POWER_01 + i, Handle());
          sb_perf[i]  = new ScrollBarControl(     Handle(),
                                                  IDD_CAR_PERF_SCROLL_01 + i,
                                                  MAX_POWER - MIN_POWER,
                                                  PER_PAGE,
                                                  PER_LINE,
                                                  MIN_POWER);
     }

     for (i = 0; i < NUM_ELEMENTS(st_team); i++) {
          st_team[i]->SetText(GP_names.teams[i].team);

          sprintf(msg_buff, "%03u", GP_EXE_car_power[i].power);
          st_power[i]->SetText(msg_buff);
          
          bhps[i] = GP_EXE_car_power[i].power;
          sb_perf[i]->SetThumbPos(bhps[i]);

          (void) EnableWindow(GetDlgItem(Handle(), IDD_CAR_PERF_TEAM_01 + i), IsTeamEnabled(i));
          (void) EnableWindow(GetDlgItem(Handle(), IDD_CAR_PERF_POWER_01 + i), IsTeamEnabled(i));
          (void) EnableWindow(GetDlgItem(Handle(), IDD_CAR_PERF_SCROLL_01 + i), IsTeamEnabled(i));
     }

     cb_scroll_all = new CheckBox(IDD_CAR_PERF_SCROLL_ALL, Handle());
}

BOOL
CarPerfomanceDialog::UserEventHandler(
     Event *event
) {
     BOOL           message_processed = TRUE;
     unsigned short i;

     switch (event->wParam) {
     case IDD_CAR_PERF_HELP:
          {
               Cursor cursor(IDC_WAIT);

               cursor.Select();
               {
                    if (WinHelp(Handle(), help_filename, HELP_CONTEXT, (long) IDH_CAR_PERFORMANCE)) {
                         help_active = TRUE;
                    }
               }
               cursor.Deselect();
          }
          break;

     case IDD_CAR_PERF_SCROLL_ALL:
          cb_scroll_all->Check(!cb_scroll_all->IsChecked());
          break;

     case IDD_CAR_PERF_EQUAL_BHP:
          for (i = 1; i < NUM_ELEMENTS(bhps); i++) {
               if (IsTeamEnabled(i)) {
                    bhps[i] = bhps[0];
                    sb_perf[i]->SetThumbPos(bhps[i]);
                    sprintf(msg_buff, "%03u", sb_perf[i]->GetThumbPos());
                    st_power[i]->SetText(msg_buff);
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
CarPerfomanceDialog::ScrollBar(
     EventScrollBar *event
) {
     unsigned short i;
     short          delta;

     for (i = 0; i < NUM_ELEMENTS(sb_perf); i++) {
          if ((HWND) event->Id() == GetDlgItem(Handle(), IDD_CAR_PERF_SCROLL_01 + i)) {
               sb_perf[i]->Update(event);

               if (cb_scroll_all->IsChecked()) {
                    delta = sb_perf[i]->GetThumbPos() - bhps[i];

                    for (i = 0; i < NUM_ELEMENTS(bhps); i++) {
                         if (IsTeamEnabled(i)) {
                              bhps[i] += delta;
                              sb_perf[i]->SetThumbPos(bhps[i]);
                              bhps[i] = sb_perf[i]->GetThumbPos();
                              sprintf(msg_buff, "%03u", bhps[i]);
                              st_power[i]->SetText(msg_buff);
                         }
                    }
               }
               else {
                    bhps[i] = sb_perf[i]->GetThumbPos();
                    sprintf(msg_buff, "%03u", bhps[i]);
                    st_power[i]->SetText(msg_buff);
               }
               break;
          }
     }
}

void
CarPerfomanceDialog::OK(
     void
) {
     unsigned short i;

     for (i = 0; i < NUM_ELEMENTS(sb_perf); i++) {
          GP_EXE_car_power[i].power = sb_perf[i]->GetThumbPos();
     }
     End();
}

/*---------------------------------------------------------------------------
** End of File
*/

