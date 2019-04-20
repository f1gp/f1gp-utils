/*
** File   : drvperf.cpp
** Author : TK 
** Date   :  3/01/94
**
** $Header:   L:/GPEDITOR/VCS/DRVPERF.CP_   1.4   01 Oct 1994 15:27:54   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Driver Performance.
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

#define MAX_POS               GP_EXE_NUM_DRIVERS
#define MIN_POS               1
#define PER_LINE              1
#define PER_PAGE              5

#define DISABLED_POSITION     MAX_POS

/*---------------------------------------------------------------------------
** Typedefs
*/

class DriverPerformanceDialog : public DialogWindow {
public:
     DriverPerformanceDialog(  HINSTANCE inst,
                              HWND parent,
                              GP_EXE_DRIVER_QUALIFYING *qualifying,
                              GP_EXE_DRIVER_RACE       *race
                           );
     ~DriverPerformanceDialog();

private:
     void Initialise(Event *event);
     BOOL UserEventHandler(Event *event);
     void ScrollBar(EventScrollBar *event);
     void OK();

     ComboBox                 *cb_drivers;
     ScrollBarControl         *sb_qualifying;
     ScrollBarControl         *sb_race;
     StaticText               *st_qualifying_text;
     StaticText               *st_race_text;

     GP_EXE_DRIVER_QUALIFYING driver_qualifying[GP_EXE_NUM_DRIVERS];
     GP_EXE_DRIVER_RACE       driver_race[GP_EXE_NUM_DRIVERS];

     GP_EXE_DRIVER_QUALIFYING *ptr_driver_qualifying;
     GP_EXE_DRIVER_RACE       *ptr_driver_race;

     char                     msg_buff[128];
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
AppWindow::DriverPerformance(
     void
) {
     DriverPerformanceDialog dialog(     Instance(),
                                        Handle(),
                                        GP_EXE_driver_qualifying,
                                        GP_EXE_driver_race
                                   );
     (void) dialog.ProcessMessages();
}

DriverPerformanceDialog::DriverPerformanceDialog(
     HINSTANCE                inst,          /* In  Instance handle        */
     HWND                     parent,        /* In  Parent handle          */
     GP_EXE_DRIVER_QUALIFYING *qualifying,   /* I/O Qualifying positions   */
     GP_EXE_DRIVER_RACE       *race          /* I/O Race positions         */
) : DialogWindow(inst, parent, GetDlgTemplateName(APP_DIALOG_DRIVER_PERFORMANCE)) {
     #pragma warn -rch
     #pragma warn -ccc
     ASSERT(sizeof(GP_EXE_driver_qualifying) == sizeof(driver_qualifying));
     ASSERT(sizeof(GP_EXE_driver_race) == sizeof(driver_race));
     #pragma warn -ccc
     #pragma warn -rch

     cb_drivers          = NULL;
     sb_qualifying       = NULL;
     sb_race             = NULL;
     st_qualifying_text  = NULL;
     st_race_text        = NULL;

     ptr_driver_qualifying    = qualifying;
     ptr_driver_race          = race;

     memcpy(driver_qualifying, qualifying, sizeof(driver_qualifying));
     memcpy(driver_race, race, sizeof(driver_race));
}

DriverPerformanceDialog::~DriverPerformanceDialog(
     void
) {
     if (cb_drivers) {
          delete cb_drivers;
          cb_drivers = NULL;
     }

     if (sb_qualifying) {
          delete sb_qualifying;
          sb_qualifying = NULL;
     }

     if (sb_race) {
          delete sb_race;
          sb_race = NULL;
     }

     if (st_qualifying_text) {
          delete st_qualifying_text;
          st_qualifying_text = NULL;
     }

     if (st_race_text) {
          delete st_race_text;
          st_race_text = NULL;
     }
}

void
DriverPerformanceDialog::Initialise(
     Event *
) {
     unsigned short i;

     cb_drivers          = new ComboBox(IDD_DRIVER_PERF_DRIVER, Handle());
     st_qualifying_text  = new StaticText(IDD_DRIVER_PERF_QUALIFYING_TXT, Handle());
     st_race_text        = new StaticText(IDD_DRIVER_PERF_RACE_TXT, Handle());
     sb_qualifying       = new ScrollBarControl(Handle(), IDD_DRIVER_PERF_QUALIFYING, MAX_POS - MIN_POS, PER_PAGE, PER_LINE, MIN_POS);
     sb_race             = new ScrollBarControl(Handle(), IDD_DRIVER_PERF_RACE, MAX_POS - MIN_POS, PER_PAGE, PER_LINE, MIN_POS);

     for (i = 0; i < NUM_ELEMENTS(GP_EXE_car_numbers); i++) {
          if (IsDriverEnabled(i)) {
               (void) cb_drivers->AddString(GP_names.drivers[DriverIndex(i)].driver);
          }
          else {
               (void) cb_drivers->AddString("---");
          }
     }
     (void) cb_drivers->SetCurrentSelection(0);
}

BOOL
DriverPerformanceDialog::UserEventHandler(
     Event *event
) {
     BOOL           message_processed = TRUE;
     unsigned short index;
     unsigned short v;

     switch (event->wParam) {
     case IDD_DRIVER_PERF_DRIVER:
          index = cb_drivers->GetCurrentSelection();

          (void) EnableWindow(GetDlgItem(Handle(), IDD_DRIVER_PERF_QUALIFYING),     IsDriverEnabled(index));
          (void) EnableWindow(GetDlgItem(Handle(), IDD_DRIVER_PERF_RACE),           IsDriverEnabled(index));
          (void) EnableWindow(GetDlgItem(Handle(), IDD_DRIVER_PERF_QUALIFYING_TXT), IsDriverEnabled(index));
          (void) EnableWindow(GetDlgItem(Handle(), IDD_DRIVER_PERF_RACE_TXT),       IsDriverEnabled(index));

          if (IsDriverEnabled(index)) {
               v = driver_qualifying[DriverIndex(index)].qualifying;
          }
          else {
               v = DISABLED_POSITION;
          }
          sb_qualifying->SetThumbPos(v);
          sprintf(msg_buff, "%u", v);
          st_qualifying_text->SetText(msg_buff);

          if (IsDriverEnabled(index)) {
               v = driver_race[DriverIndex(index)].race;
          }
          else {
               v = DISABLED_POSITION;
          }
          sb_race->SetThumbPos(v);
          sprintf(msg_buff, "%u", v);
          st_race_text->SetText(msg_buff);
          break;

     case IDD_DRIVER_PERF_HELP:
          {
               Cursor cursor(IDC_WAIT);

               cursor.Select();
               {
                    if (WinHelp(Handle(), help_filename, HELP_CONTEXT, (long) IDH_DRIVER_PERFORMANCE)) {
                         help_active = TRUE;
                    }
               }
               cursor.Deselect();
          }
          break;

     default:
          message_processed = FALSE;
          break;
     }

     return message_processed;
}

void
DriverPerformanceDialog::ScrollBar(
     EventScrollBar *event
) {
     if ((HWND) event->Id() == GetDlgItem(Handle(), IDD_DRIVER_PERF_QUALIFYING)) {
          sb_qualifying->Update(event);
          sprintf(msg_buff, "%u", sb_qualifying->GetThumbPos());
          st_qualifying_text->SetText(msg_buff);

          driver_qualifying[DriverIndex(cb_drivers->GetCurrentSelection())].qualifying = sb_qualifying->GetThumbPos();
     }

     if ((HWND) event->Id() == GetDlgItem(Handle(), IDD_DRIVER_PERF_RACE)) {
          sb_race->Update(event);
          sprintf(msg_buff, "%u", sb_race->GetThumbPos());
          st_race_text->SetText(msg_buff);

          driver_race[DriverIndex(cb_drivers->GetCurrentSelection())].race = sb_race->GetThumbPos();
     }
}

void
DriverPerformanceDialog::OK(
     void
) {
     memcpy(ptr_driver_qualifying, driver_qualifying, sizeof(driver_qualifying));
     memcpy(ptr_driver_race, driver_race, sizeof(driver_race));

     End();
}

/*---------------------------------------------------------------------------
** End of File
*/

