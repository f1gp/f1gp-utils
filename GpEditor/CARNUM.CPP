/*
** File   : carnum.cpp
** Author : TK 
** Date   : 12/02/94
**
** $Header:   L:/GPEDITOR/VCS/CARNUM.CP_   1.5   01 Oct 1994 15:27:54   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Car Numbers.
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

class CarNumbersDialog : public DialogWindow {
public:
     CarNumbersDialog(HINSTANCE inst, HWND parent, unsigned char *car_nums);
     ~CarNumbersDialog();

private:
     void Initialise(Event *event);
     BOOL UserEventHandler(Event *event);
     void OK();

     void SetEntryValues(unsigned short index, unsigned short value1, unsigned short value2);
     void UpdateComboBoxes(void);

     ComboBox       *cb_1;
     ComboBox       *cb_2;
     RadioButtons   *rb_teams;
     StaticText     *st_text[GP_EXE_NUM_TEAMS];
     unsigned char  car_numbers[GP_EXE_NUM_DRIVERS];
     unsigned char  *op_cn_ptr;

     char           msg_buff[128];
};

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

static const unsigned char NormalNumbers[] = {
     1,  2,  3,   4,  5,  6,  7,  8,  9, 10,
     11, 12, 14,  0, 15, 16, 17, 18, 19, 20,
     21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
     31, 00, 32, 33, 34, 35,  0,  0,  0,  0
};

/*---------------------------------------------------------------------------
** Functions
*/

void
AppWindow::CarNumbers(
     void
) {
     GP_EXE_DRIVER_QUALIFYING driver_qualifying[GP_EXE_NUM_DRIVERS];
     GP_EXE_DRIVER_RACE       driver_race[GP_EXE_NUM_DRIVERS];
     GP_NAMES                 names;
     unsigned char            cn[GP_EXE_NUM_DRIVERS];
     unsigned char            *orig_cn = (unsigned char *) &GP_EXE_car_numbers;
     unsigned short           i;

     #pragma warn -rch
     #pragma warn -ccc
     ASSERT(sizeof(GP_EXE_car_numbers) == sizeof(cn));
     ASSERT(sizeof(GP_EXE_car_numbers) == sizeof(NormalNumbers));
     #pragma warn -ccc
     #pragma warn -rch

     memcpy(cn, GP_EXE_car_numbers, sizeof(cn));

     CarNumbersDialog dialog(Instance(), Handle(), cn);

     if (dialog.ProcessMessages()) {
          /*
          ** The game indexes the driver Names & Qualifying & Race performance
          ** via the Car Number minus 1 (0 means the car is disabled).
          ** We thus need to reorder these values based on the new car
          ** numbers.
          */
          memset(driver_qualifying, 0x00, sizeof(driver_qualifying));
          memset(driver_race, 0x00, sizeof(driver_race));
          memcpy(&names, &GP_names, sizeof(names));
          for (i = 0; i < NUM_ELEMENTS(cn); i++) {
               if (cn[i]) {
                    if (orig_cn[i] != 0) {
                         /*
                         ** Car was previously enabled.
                         */
                         driver_qualifying[cn[i] - 1].qualifying = GP_EXE_driver_qualifying[orig_cn[i] - 1].qualifying;
                         driver_race[cn[i] - 1].race             = GP_EXE_driver_race[orig_cn[i] - 1].race;
                         strcpy(names.drivers[cn[i] - 1].driver, GP_names.drivers[orig_cn[i] - 1].driver);
                    }
                    else {
                         /*
                         ** Car was previously disabled so use leave current values as they are unless invalid.
                         */
                         if (driver_qualifying[cn[i] - 1].qualifying < 1 || driver_qualifying[cn[i] - 1].qualifying > 40) {
                              driver_qualifying[cn[i] - 1].qualifying = 40;
                         }

                         if (driver_race[cn[i] - 1].race < 1 || driver_race[cn[i] - 1].race > 40) {
                              driver_race[cn[i] - 1].race = 40;
                         }
                    }
               }
          }

          memcpy(GP_EXE_car_numbers, cn, sizeof(cn));
          memcpy(GP_EXE_driver_qualifying, driver_qualifying, sizeof(GP_EXE_driver_qualifying));
          memcpy(GP_EXE_driver_race, driver_race, sizeof(GP_EXE_driver_race));
          memcpy(&GP_names, &names, sizeof(GP_names));
     }
}

CarNumbersDialog::CarNumbersDialog(
     HINSTANCE      inst,                    /* In  Instance handle        */
     HWND           parent,                  /* In  Parent handle          */
     unsigned char  *car_nums                /* In  Car Numbers            */
) : DialogWindow(inst, parent, GetDlgTemplateName(APP_DIALOG_CAR_NUMBERS)) {
     #pragma warn -rch
     #pragma warn -ccc
     ASSERT(sizeof(GP_EXE_car_numbers) == sizeof(car_numbers));
     ASSERT(NUM_ELEMENTS(GP_EXE_car_numbers) / 2 == NUM_ELEMENTS(st_text));
     #pragma warn -ccc
     #pragma warn -rch

     cb_1      = NULL;
     cb_2      = NULL;
     rb_teams  = NULL;
     memset(st_text, 0x00, sizeof(st_text));
     memcpy(car_numbers, car_nums, sizeof(car_numbers));
     op_cn_ptr = car_nums;
}

CarNumbersDialog::~CarNumbersDialog(
     void
) {
     unsigned short i;

     if (cb_1) {
          delete cb_1;
     }

     if (cb_2) {
          delete cb_2;
     }

     if (rb_teams) {
          delete rb_teams;
     }

     for (i = 0; i < NUM_ELEMENTS(st_text); i++) {
          if (st_text[i]) {
               delete st_text[i];
          }
     }
}

void
CarNumbersDialog::Initialise(
     Event *
) {
     unsigned short i;

     cb_1     = new ComboBox(IDD_CAR_NUMBER_LIST1, Handle());
     cb_2     = new ComboBox(IDD_CAR_NUMBER_LIST2, Handle());
     rb_teams = new RadioButtons(  IDD_CAR_NUMBER_RB_01,
                                   IDD_CAR_NUMBER_RB_20,
                                   IDD_CAR_NUMBER_RB_01,
                                   Handle()
                              );

     for (i = 0; i < NUM_ELEMENTS(st_text); i++) {
          st_text[i] = new StaticText(IDD_CAR_NUMBER_TXT_01 + i, Handle());

          SetEntryValues(i, car_numbers[i * 2], car_numbers[(i * 2) + 1]);

          rb_teams->GetIndividualButton(IDD_CAR_NUMBER_RB_01 + i)->SetText(GP_names.teams[i].team);
     }

     for (i = 0; i <= NUM_ELEMENTS(car_numbers); i++) {
          sprintf(msg_buff, "%02u", i);
          (void) cb_1->AddString(msg_buff);
          (void) cb_2->AddString(msg_buff);
     }

     UpdateComboBoxes();
}

BOOL
CarNumbersDialog::UserEventHandler(
     Event *event
) {
     BOOL           message_processed = TRUE;
     unsigned short i;

     switch (event->wParam) {
     case IDD_CAR_NUMBER_RB_01:
     case IDD_CAR_NUMBER_RB_02:
     case IDD_CAR_NUMBER_RB_03:
     case IDD_CAR_NUMBER_RB_04:
     case IDD_CAR_NUMBER_RB_05:
     case IDD_CAR_NUMBER_RB_06:
     case IDD_CAR_NUMBER_RB_07:
     case IDD_CAR_NUMBER_RB_08:
     case IDD_CAR_NUMBER_RB_09:
     case IDD_CAR_NUMBER_RB_10:
     case IDD_CAR_NUMBER_RB_11:
     case IDD_CAR_NUMBER_RB_12:
     case IDD_CAR_NUMBER_RB_13:
     case IDD_CAR_NUMBER_RB_14:
     case IDD_CAR_NUMBER_RB_15:
     case IDD_CAR_NUMBER_RB_16:
     case IDD_CAR_NUMBER_RB_17:
     case IDD_CAR_NUMBER_RB_18:
     case IDD_CAR_NUMBER_RB_19:
     case IDD_CAR_NUMBER_RB_20:
          rb_teams->SetActiveButton(event->wParam);
          UpdateComboBoxes();
          break;

     case IDD_CAR_NUMBER_LIST1:
          {
               short is_active = rb_teams->GetActiveButton() - IDD_CAR_NUMBER_RB_01;

               car_numbers[is_active * 2] = cb_1->GetCurrentSelection();
               SetEntryValues(is_active, car_numbers[is_active * 2], car_numbers[(is_active * 2) + 1]);
          }
          break;

     case IDD_CAR_NUMBER_LIST2:
          {
               short is_active = rb_teams->GetActiveButton() - IDD_CAR_NUMBER_RB_01;

               car_numbers[(is_active * 2) + 1] = cb_2->GetCurrentSelection();
               SetEntryValues(is_active, car_numbers[is_active * 2], car_numbers[(is_active * 2) + 1]);
          }
          break;

     case IDD_CAR_NUMBER_NORMAL:
          {
               memcpy(car_numbers, NormalNumbers, sizeof(car_numbers));
               for (i = 0; i < NUM_ELEMENTS(st_text); i++) {
                    SetEntryValues(i, car_numbers[i * 2], car_numbers[(i * 2) + 1]);
               }
               UpdateComboBoxes();
          }
          break;

     default:
          message_processed = FALSE;
          break;
     }

     return message_processed;
}

void
CarNumbersDialog::OK(
     void
) {
     unsigned short i;
     unsigned short index;
     BOOL           used[NUM_ELEMENTS(car_numbers)];
     BOOL           ok = TRUE;

     /*
     ** Ensure car numbers are only used once.
     */
     for (i = 0; i < NUM_ELEMENTS(car_numbers); i++) {
          used[i] = FALSE;
     }

     for (i = 0; ok && i < NUM_ELEMENTS(car_numbers); i++) {
          index = i;

          /*
          ** Entries numbered zero aren't used.
          */
          if (car_numbers[index]) {
               if (used[car_numbers[index] - 1]) {
                    ok = FALSE;
               }
               else {
                    used[car_numbers[index] - 1] = TRUE;
               }
          }
     }

     if (!ok) {
          rb_teams->SetActiveButton(IDD_CAR_NUMBER_RB_01 + (index / 2));
          UpdateComboBoxes();
          if ((index & 1) == 0) {
               cb_1->SetFocus();
          }
          else {
               cb_2->SetFocus();
          }
          MessageBeep(MB_ICONEXCLAMATION);
     }
     else {
          memcpy(op_cn_ptr, car_numbers, sizeof(car_numbers));
          End(TRUE);
     }
}

void
CarNumbersDialog::SetEntryValues(
     unsigned short      index,
     unsigned short      value1,
     unsigned short      value2
) {
     ASSERT(index < NUM_ELEMENTS(st_text));
     ASSERT(value1 <=  NUM_ELEMENTS(car_numbers));
     ASSERT(value2 <=  NUM_ELEMENTS(car_numbers));

     sprintf(msg_buff, "%02u, %02u", value1, value2);
     st_text[index]->SetText(msg_buff);
}

void
CarNumbersDialog::UpdateComboBoxes(
     void
) {
     short is_active = rb_teams->GetActiveButton() - IDD_CAR_NUMBER_RB_01;

     ASSERT(is_active >= 0 && is_active < NUM_ELEMENTS(car_numbers));
     (void) cb_1->SetCurrentSelection(car_numbers[(is_active * 2) + 0]);
     (void) cb_2->SetCurrentSelection(car_numbers[(is_active * 2) + 1]);
}

/*---------------------------------------------------------------------------
** End of File
*/

