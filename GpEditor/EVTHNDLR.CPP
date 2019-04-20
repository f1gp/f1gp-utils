/*
** File   : evthndlr.cpp
** Author : TK 
** Date   : 26/02/94
**
** $Header:   L:/GPEDITOR/VCS/EVTHNDLR.CP_   1.4   01 Oct 1994 15:27:50   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Event handlers.
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
** Purpose:    Main user (menu bar) generated event handler.
**
** Returns:    True if event was processed, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::UserEventHandler(
     Event     *event                   /* In  Event type                  */
) {
     static BOOL    fCommandLineOpenDone = FALSE;

     BOOL event_processed = TRUE;

     ASSERT(event != NULL);

     switch (event->wParam) {
     case IDM_FILE_OPEN_GAME_EXE:
          if (command_line_param1[0] != '\0' && !fCommandLineOpenDone) {
               (void) FileOpenGameExe(command_line_param1);

               if (command_line_param2[0] != '\0') {
                    (void) FileOpenNames(command_line_param2);
               }
               fCommandLineOpenDone = TRUE;
          }
          else {
               (void) FileOpenGameExe();
          }
          SwitchEditor();
          break;

     case IDM_FILE_UPDATE_GAME_EXE:
          (void) FileUpdateGameExe();
          break;

     case IDM_FILE_OPEN_NAMES:
          (void) FileOpenNames();
          break;

     case IDM_FILE_SAVE_NAMES:
          (void) FileSaveNames();
          break;

     case IDM_FILE_IMPORT_GPE:
          (void) ImportGpe();
          break;

     case IDM_FILE_EXPORT_GPE:
          (void) ExportGpe();
          break;

     case IDM_LANGUAGE:
          Language();
          break;

     case IDM_EXIT:
          Exit();
          break;

     case IDM_DESIGN_NEW:
          if (fCarEditorEnabled) {
               DesignNew();
          }
          else {
               HelmetNew();
          }
          break;

     case IDM_DESIGN_LOAD:
          if (fCarEditorEnabled) {
               (void) DesignLoad();
          }
          else {
               (void) HelmetLoad();
          }
          break;

     case IDM_DESIGN_SAVE:
          if (fCarEditorEnabled) {
               (void) DesignSave();
          }
          else {
               (void) HelmetSave();
          }
          break;

     case IDM_PERFORMANCE_CAR:
          CarPerformance();
          break;

     case IDM_PERFORMANCE_DRIVER:
          DriverPerformance();
          break;

     case IDM_PERFORMANCE_DRIVING_AIDS:
          DrivingAids();
          break;

     case IDM_PERFORMANCE_CAR_NUMBERS:
          CarNumbers();
          break;

     case IDM_AI_CAR_GRIP:
          AiCarGrip();
          break;

     case IDM_PLAYERS_CAR_PERFORMANCE:
          PlayersCarPerformance();
          break;

     case IDM_NAMES:
          Names();
          break;

     case IDM_CAR_EDITOR:
          fCarEditorEnabled = TRUE;
          SwitchEditor();
          break;

     case IDM_HELMET_EDITOR:
          fCarEditorEnabled = FALSE;
          SwitchEditor();
          break;

     case IDM_HELP_CONTENTS:
          {
               Cursor cursor(IDC_WAIT);
               cursor.Select();
               {
                    if (WinHelp(Handle(), help_filename, HELP_CONTENTS, 0L)) {
                         help_active = TRUE;
                    }
               }
               cursor.Deselect();
          }
          break;

     case IDM_HELP_USE:
          {
               Cursor cursor(IDC_WAIT);
               cursor.Select();
               {
                    if (WinHelp(Handle(), help_filename, HELP_HELPONHELP, 0L)) {
                         help_active = TRUE;
                    }
               }
               cursor.Deselect();
          }
          break;

     case IDM_ABOUT:
          About();
          break;

     default:
          event_processed = FALSE;
          break;
     }

     return event_processed;
}

/*---------------------------------------------------------------------------
** Purpose:    Main unknown event handler.
**
** Returns:    True if event was processed, false if not.
**
** Notes  :    
*/
BOOL
AppWindow::UnknownSysEventHandler(
     Event     *event,
     long      *r_val_p
) {
     BOOL      message_processed = TRUE;

     ASSERT(event != NULL);

     switch (event->message) {
     case WM_ERASEBKGND:
          /*
          ** Lie that we have erased.
          */
          break;

     case WM_GETMINMAXINFO:
          /*
          ** Limit the maximum window size.
          */
          {
               MINMAXINFO far *lpMinMaxInfo = (MINMAXINFO far *) event->lParam;

               lpMinMaxInfo->ptMaxSize.x      = frame_width;
               lpMinMaxInfo->ptMaxSize.y      = frame_height;
               lpMinMaxInfo->ptMaxTrackSize.x = frame_width;
               lpMinMaxInfo->ptMaxTrackSize.y = frame_height;
          }
          break;

     case WM_PALETTECHANGED:
          if (((HWND) event->wParam) == Handle()) {
               break;
          }
          /* Fall through */

     case WM_QUERYNEWPALETTE:
          {
               HPALETTE  hOldPalette;
               HDC       hDC = GetDC(Handle());
               int       i;

               hOldPalette = SelectPalette(hDC, hPalette, FALSE);
               i = RealizePalette(hDC);
               if (i) {
                    RepaintWindow();
               }
               (void) SelectPalette(hDC, hOldPalette, FALSE);
               (void) RealizePalette(hDC);
               (void) ReleaseDC(Handle(), hDC);
               *r_val_p = i;               
          }
          break;

     case WM_MOVE:
          /*
          ** The control bar combo box "leaves" its drop down window in
          ** its current location if the parent window is moved.
          ** We ensure that it is always deselected when we move the parent.
          */
          if (pControlBar) {
               (void) SendMessage(pControlBar->Handle(), IDE_CBAR_CANCEL_DROPDOWN, 0, 0L);
          }
          message_processed = FALSE;
          break;

     case WM_SYSCOLORCHANGE:
          if (!No3D) {
               UtlCtl3dColorChange();
          }
          break;

     case IDE_DESIGN_NEW:
     case IDE_DESIGN_LOAD:
     case IDE_DESIGN_SAVE:
     case IDE_DESIGN_NEW_PALETTE_COLOUR:
     case IDE_DESIGN_TEAM_SELECT:
          /*
          ** Forward event to correct child window.
          */
          if (event->message == IDE_DESIGN_TEAM_SELECT) {
               fCarEditorEnabled = TRUE;
               SwitchEditor();
          }
          *r_val_p = SendMessage(pDrawingWindow->Handle(), event->message, event->wParam, event->lParam);
          break;

     case IDE_HELMET_NEW:
     case IDE_HELMET_LOAD:
     case IDE_HELMET_SAVE:
     case IDE_HELMET_NEW_PALETTE_COLOUR:
     case IDE_HELMET_DRIVER_SELECT:
          /*
          ** Forward event to correct child window.
          */
          *r_val_p = SendMessage(pHelmetWindow->Handle(), event->message, event->wParam, event->lParam);
          break;

     case IDE_CBAR_TEAM_SELECT:
          /*
          ** Forward event to correct child window.
          */
          (void) SendMessage(pControlBar->Handle(), event->message, event->wParam, event->lParam);
          break;

     case IDE_TEAMS_TEAM_SELECT:
          /*
          ** Forward event to correct child window.
          */
          (void) SendMessage(pTeamsWindow->Handle(), event->message, event->wParam, event->lParam);
          break;

     case IDE_CBAR_DRIVER_SELECT:
          /*
          ** Forward event to correct child window.
          */
          (void) SendMessage(pControlBar->Handle(), event->message, event->wParam, event->lParam);
          break;

     default:
          message_processed = FALSE;
          break;
     }

     if (!message_processed) {
          message_processed = MainWindow::UnknownSysEventHandler(event, r_val_p);
     }

     return message_processed;
}

/*---------------------------------------------------------------------------
** End of File
*/

