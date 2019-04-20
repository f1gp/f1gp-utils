/*
** File   : cntrlbar.cpp
** Author : TK 
** Date   : 21/11/93
**
** $Header:   L:/GPEDITOR/VCS/CNTRLBAR.CP_   1.5   01 Oct 1994 15:27:56   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Control Bar Window.
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
#include <toolbar.hpp>
/*lint +elib(???) */
#include "cntrlbar.hpp"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define IDW_COMBO_TEAM_LIST        2
#define IDW_COMBO_DRIVER_LIST      3
#define IDW_BTN_CAR                4
#define IDW_BTN_HELMET             5
#define IDW_TOOLBAR                6

#define SEP                        4
#define TOOLBAR_WIDTH              320

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

static TBBUTTON tbButton[] = {
     {    0,   IDM_DESIGN_NEW,               TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {    1,   IDM_DESIGN_LOAD,              TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {    2,   IDM_DESIGN_SAVE,              TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {  SEP,   0,                            TBSTATE_ENABLED,    TBSTYLE_SEP    },
     {    4,   IDM_CAR_EDITOR,               TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {    5,   IDM_HELMET_EDITOR,            TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {  SEP,   0,                            TBSTATE_ENABLED,    TBSTYLE_SEP    },
     {    7,   IDM_PERFORMANCE_DRIVING_AIDS, TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {    8,   IDM_PERFORMANCE_CAR_NUMBERS,  TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {    9,   IDM_PERFORMANCE_CAR,          TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {   10,   IDM_PERFORMANCE_DRIVER,       TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {   11,   IDM_NAMES,                    TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {   12,   IDM_AI_CAR_GRIP,              TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {   13,   IDM_PLAYERS_CAR_PERFORMANCE,  TBSTATE_ENABLED,    TBSTYLE_BUTTON },
     {  SEP,   0,                            TBSTATE_ENABLED,    TBSTYLE_SEP    },
     {   15,   IDM_HELP_CONTENTS,            TBSTATE_ENABLED,    TBSTYLE_BUTTON },
};

ToolBar        *pToolbar;
ToolBarButton  *pToolbarButtons[NUM_ELEMENTS(tbButton)];

/*---------------------------------------------------------------------------
** Functions
*/

ControlBar::ControlBar(
     HINSTANCE      hInst,
     HWND           hWndParent,
     unsigned short usWidth,
     unsigned short usHeight,
     unsigned short usComboBoxHeight
) : ChildWindow(hInst, CONTROL_BAR_CLASS_NAME, GetAppName(), WS_CHILD | WS_BORDER, CHILD_ID_CONTROL_BAR, hWndParent) {
     HWND           hTeams;
     HWND           hDrivers;
     unsigned short i;

     pToolbar  = new ToolBar( Instance(),
                              Handle(),
                              WS_VISIBLE,
                              IDW_TOOLBAR,
                              NUM_ELEMENTS(tbButton),
                              IDR_TOOLBAR,
                              tbButton,
                              NUM_ELEMENTS(tbButton),
                              TOOLBAR_WIDTH,
                              usHeight
                         );

     for (i = 0; i < NUM_ELEMENTS(pToolbarButtons); i++) {
          pToolbarButtons[i] = new ToolBarButton(tbButton[i].idCommand, pToolbar->Handle());
     }

     hTeams = CreateWindow(   "COMBOBOX", "",
                              WS_CHILD | WS_VSCROLL | 
                              CBS_DROPDOWNLIST | CBS_HASSTRINGS,
                              0, 0, 1, 1,
                              Handle(), (HMENU) IDW_COMBO_TEAM_LIST, Instance(), NULL
                         );
     ASSERT(hTeams != NULL);
     pcbTeams = new ComboBox(IDW_COMBO_TEAM_LIST, Handle());
     ASSERT(pcbTeams != NULL);
     if (hTeams) {
          LoadNames();
     }

     hDrivers = CreateWindow( "COMBOBOX", "",
                              WS_CHILD | WS_VSCROLL |
                              CBS_DROPDOWNLIST | CBS_HASSTRINGS,
                              0, 0, 1, 1,
                              Handle(), (HMENU) IDW_COMBO_DRIVER_LIST, Instance(), NULL
                         );
     ASSERT(hDrivers != NULL);
     pcbDrivers = new ComboBox(IDW_COMBO_DRIVER_LIST, Handle());
     ASSERT(pcbDrivers != NULL);
     if (hDrivers) {
          LoadDriverNames();
     }

     unsigned short cbn_width;
     unsigned short cbn_item_height;
     unsigned short cbn_y_offset;

     cbn_width           = usWidth - (TOOLBAR_WIDTH + SEP);
     cbn_item_height     = SendMessage(pcbTeams->Handle(), CB_GETITEMHEIGHT, -1, 0L);
     cbn_y_offset        = ((usHeight - cbn_item_height) / 2) - 1;

     (void) MoveWindow(pcbTeams->Handle(), TOOLBAR_WIDTH, cbn_y_offset, cbn_width, usComboBoxHeight, FALSE);
     (void) MoveWindow(pcbDrivers->Handle(), TOOLBAR_WIDTH, cbn_y_offset, cbn_width, usComboBoxHeight, FALSE);

     pcbTeams->Hide();
     pcbDrivers->Hide();
}

ControlBar::~ControlBar(
     void
) {
     unsigned short i;
     for (i = 0; i < NUM_ELEMENTS(pToolbarButtons); i++) {
          if (pToolbarButtons[i]) {
               delete pToolbarButtons[i];
               pToolbarButtons[i] = NULL;
          }
     }

     if (pToolbar) {
          delete pToolbar;
          pToolbar = NULL;
     }

     if (pcbTeams) {
          delete pcbTeams;
          pcbTeams = NULL;
     }

     if (pcbDrivers) {
          delete pcbDrivers;
          pcbDrivers = NULL;
     }

     /*
     ** Combo box is destroyed automatically as it is a child window.
     */
}

void
ControlBar::UpdateToolBar(
     void
) {
     unsigned short i;

     for (i = 0; i <= NUM_ELEMENTS(pToolbarButtons); i++) {
          if (pToolbarButtons[i]) {
               switch (tbButton[i].idCommand) {
               case IDM_PERFORMANCE_DRIVING_AIDS:
               case IDM_PERFORMANCE_CAR_NUMBERS:
               case IDM_PERFORMANCE_CAR:
               case IDM_PERFORMANCE_DRIVER:
               case IDM_NAMES:
               case IDM_AI_CAR_GRIP:
               case IDM_PLAYERS_CAR_PERFORMANCE:
                    pToolbarButtons[i]->Enable(fGpExeIsFound);
                    break;

               default:
                    break;
               }
          }
     }
}

BOOL
ControlBar::UserEventHandler(
     Event     *event
) {
     BOOL event_processed = TRUE;

     ASSERT(event != NULL);

     switch (event->wParam) {
     case IDM_DESIGN_LOAD:
     case IDM_DESIGN_SAVE:
     case IDM_DESIGN_NEW:
     case IDM_HELMET_EDITOR:
     case IDM_CAR_EDITOR:
     case IDM_HELP_CONTENTS:
     case IDM_PERFORMANCE_DRIVING_AIDS:
     case IDM_PERFORMANCE_CAR_NUMBERS:
     case IDM_PERFORMANCE_CAR:
     case IDM_PERFORMANCE_DRIVER:
     case IDM_NAMES:
     case IDM_AI_CAR_GRIP:
     case IDM_PLAYERS_CAR_PERFORMANCE:
          (void) SendMessage(app_window->Handle(), WM_COMMAND, event->wParam, event->lParam);
          break;

     case IDW_BTN_CAR:
          if (HIWORD(event->lParam) == BN_CLICKED) {
               (void) SendMessage(Parent(), WM_COMMAND, IDM_CAR_EDITOR, 0L);
          }
          break;

     case IDW_BTN_HELMET:
          if (HIWORD(event->lParam) == BN_CLICKED) {
               (void) SendMessage(Parent(), WM_COMMAND, IDM_HELMET_EDITOR, 0L);
          }
          break;

     case IDW_COMBO_TEAM_LIST:
          if (HIWORD(event->lParam) == CBN_SELCHANGE) {
               BOOL fImageDirty;

               fImageDirty = (BOOL) SendMessage(Parent(), IDE_DESIGN_TEAM_SELECT, pcbTeams->GetCurrentSelection(), 0L);
               (void) SendMessage(Parent(), IDE_TEAMS_TEAM_SELECT, pcbTeams->GetCurrentSelection(), (long) fImageDirty);
          }
          break;

     case IDW_COMBO_DRIVER_LIST:
          if (HIWORD(event->lParam) == CBN_SELCHANGE) {
               BOOL fImageDirty;

               fImageDirty = (BOOL) SendMessage(Parent(), IDE_HELMET_DRIVER_SELECT, pcbTeams->GetCurrentSelection(), 0L);
               (void) SendMessage(Parent(), IDE_HELMET_DRIVER_SELECT, pcbDrivers->GetCurrentSelection(), (long) fImageDirty);
          }
          break;

     default:
          event_processed = FALSE;
          break;
     }

     return event_processed;
}

BOOL
ControlBar::UnknownSysEventHandler(
     Event     *event,
     long      *r_val_p
) {
     BOOL      message_processed = TRUE;

     ASSERT(event != NULL);

     switch (event->message) {
     case IDE_CBAR_CANCEL_DROPDOWN:
          /*
          ** The control bar combo box "leaves" its drop down window in
          ** its current location if the parent window is moved.
          ** We ensure that it is always deselected when we move the parent.
          */
          if (pcbTeams) {
               (void) SendMessage(pcbTeams->Handle(), CB_SHOWDROPDOWN, FALSE, 0L);
          }
          if (pcbDrivers) {
               (void) SendMessage(pcbDrivers->Handle(), CB_SHOWDROPDOWN, FALSE, 0L);
          }
          message_processed = FALSE;
          break;

     case IDE_CBAR_TEAM_SELECT:
          ASSERT(event->wParam < NUM_ELEMENTS(GP_names.teams));
          pcbDrivers->Hide();
          pcbTeams->Show();
          (void) pcbTeams->SetCurrentSelection(event->wParam);
          break;

     case IDE_CBAR_DRIVER_SELECT:
          ASSERT(event->wParam < NUM_ELEMENTS(GP_names.drivers));
          pcbTeams->Hide();
          pcbDrivers->Show();
          (void) pcbDrivers->SetCurrentSelection(event->wParam);
          break;

     case IDE_CBAR_SHOW_TEAMS:
          pcbDrivers->Hide();
          pcbTeams->Show();
          break;

     case IDE_CBAR_SHOW_DRIVERS:
          pcbTeams->Show(!event->wParam);
          pcbDrivers->Show(event->wParam);
          break;

     case IDE_CBAR_LOAD_NAMES:
          LoadNames();
          break;

     case IDE_CBAR_LOAD_DRIVER_NAMES:
          LoadDriverNames();
          break;

     default:
          message_processed = FALSE;
          break;
     }

     if (!message_processed) {
          message_processed = ChildWindow::UnknownSysEventHandler(event, r_val_p);
     }

     return message_processed;
}

BOOL
ControlBar::Paint(
     void
) {
     PaintWindowUpdate   pwu(Handle());
     RECT                rect;

     SolidBrush     brush(GetSysColor(COLOR_BTNFACE));

     pwu.rect(&rect);
     pwu.FillRect(&rect, &brush);
     return FALSE;
}

void
ControlBar::LoadNames(
     void
) {
     unsigned short i;
     unsigned short usSelection;
     char           buff[128];

     usSelection = pcbTeams->GetCurrentSelection();

     pcbTeams->DeleteAllStrings();
     for (i = 0; i < NUM_ELEMENTS(GP_names.teams); i++) {
          sprintf(buff, "%s %s", GP_names.teams[i].team, GP_names.engines[i].engine);
          (void) pcbTeams->AddString(buff);
     }
     (void) pcbTeams->SetCurrentSelection(usSelection);
}

void
ControlBar::LoadDriverNames(
     void
) {
     unsigned short i;
     unsigned short usSelection;

     usSelection = pcbDrivers->GetCurrentSelection();

     pcbDrivers->DeleteAllStrings();
     for (i = 0; i < NUM_ELEMENTS(GP_names.drivers); i++) {
          if (IsDriverEnabled(i)) {
               (void) pcbDrivers->AddString(GP_names.drivers[DriverIndex(i)].driver);
          }
          else {
               (void) pcbDrivers->AddString("---");
          }
     }
     (void) pcbDrivers->SetCurrentSelection(usSelection);
}

/*---------------------------------------------------------------------------
** End of File
*/

