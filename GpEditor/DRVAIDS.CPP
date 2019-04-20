/*
** File   : drvaids.cpp
** Author : TK 
** Date   : 30/01/94
**
** $Header:   L:/GPEDITOR/VCS/DRVAIDS.CP_   1.3   01 Oct 1994 15:27:48   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Driving Aids.
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

#define NUMBER_OF_LEVELS      5
#define NUMBER_OF_AIDS        6

#define GPEDICON              "GPedIcon"

/*---------------------------------------------------------------------------
** Typedefs
*/

class DrivingAidsDialog : public DialogWindow {
public:
     DrivingAidsDialog(HINSTANCE inst, HWND parent);
     ~DrivingAidsDialog();

private:
     void Initialise(Event *event);
     void Activate();
     BOOL UserEventHandler(Event *event);
     void UpdateIcons(void);
     void OK();

     void SetNormal();

     RadioButtons   *rb_level;
};

/*---------------------------------------------------------------------------
** Local function prototypes
*/

LRESULT WINAPI _export AnimatedIconProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL    AnimatedIconInit(HINSTANCE instance, HWND parent);

/*---------------------------------------------------------------------------
** Data
*/

static WNDPROC           AnimatedIconBaseClassesWndProc;
static HINSTANCE         hSavedInstance;
static BOOL              Aids[NUMBER_OF_LEVELS][NUMBER_OF_AIDS];
static unsigned short    level;

static const BOOL NormalAids[NUMBER_OF_LEVELS][NUMBER_OF_AIDS] = {
     {    TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  TRUE      },   /* Rookie      */
     {    FALSE, TRUE,  TRUE,  TRUE,  TRUE,  TRUE      },   /* Amateur     */
     {    FALSE, FALSE, TRUE,  TRUE,  TRUE,  TRUE      },   /* Semi-Pro    */
     {    FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE      },   /* Pro         */
     {    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE     }    /* Ace         */
};

/*---------------------------------------------------------------------------
** Functions
*/

void
AppWindow::DrivingAids(
     void
) {
     static BOOL first = TRUE;

     if (first) {
          first = FALSE;
          (void) AnimatedIconInit(Instance(), Handle());
     }

     DrivingAidsDialog dialog(Instance(), Handle());

     (void) dialog.ProcessMessages();
}

DrivingAidsDialog::DrivingAidsDialog(
     HINSTANCE      inst,                    /* In  Instance handle        */
     HWND           parent                   /* In  Parent handle          */
) : DialogWindow(inst, parent, GetDlgTemplateName(APP_DIALOG_DRIVINGS_AIDS)) {
     hSavedInstance = inst;
     rb_level       = NULL;
     level          = 0;

     unsigned short i, j;
     unsigned char  b;

     for (i = 0; i < NUMBER_OF_LEVELS; i++) {
          for (b = 0x01, j = 0; j < NUMBER_OF_AIDS; j++, b <<= 1) {
               Aids[i][j] = (GP_EXE_driving_aids[i] & b) ? TRUE : FALSE;
          }
     }
}

DrivingAidsDialog::~DrivingAidsDialog(
     void
) {
     if (rb_level) {
          delete rb_level;
          rb_level = NULL;
     }
}

void
DrivingAidsDialog::Initialise(
     Event *
) {
     rb_level = new RadioButtons(IDD_DRIVER_AID_ROOKIE, IDD_DRIVER_AID_ACE, IDD_DRIVER_AID_ACE, Handle());
     level = IDD_DRIVER_AID_ACE - IDD_DRIVER_AID_ROOKIE;

     UpdateIcons();
}

void
DrivingAidsDialog::Activate(
     void
) {
     UpdateIcons();
}

BOOL
DrivingAidsDialog::UserEventHandler(
     Event *event
) {
     BOOL message_processed = TRUE;

     switch (event->wParam) {
     case IDD_DRIVER_AID_ROOKIE:
     case IDD_DRIVER_AID_AMATEUR:
     case IDD_DRIVER_AID_SEMIPRO:
     case IDD_DRIVER_AID_PRO:
     case IDD_DRIVER_AID_ACE:
          rb_level->SetActiveButton(event->wParam);
          level = event->wParam - IDD_DRIVER_AID_ROOKIE;
          UpdateIcons();
          break;

     case IDD_DRIVER_AID_AUTO_BRAKES:
     case IDD_DRIVER_AID_AUTO_GEARS:
     case IDD_DRIVER_AID_SPIN:
     case IDD_DRIVER_AID_DAMAGE:
     case IDD_DRIVER_AID_BEST_LINE:
     case IDD_DRIVER_AID_GEAR_IND:
          Aids[level][event->wParam - IDD_DRIVER_AID_AUTO_BRAKES] = !Aids[level][event->wParam - IDD_DRIVER_AID_AUTO_BRAKES];
          UpdateIcons();
          break;

     case IDD_DRIVER_AID_NORMAL:
          SetNormal();
          UpdateIcons();
          break;

     case IDD_DRIVER_AID_ALLOW_ALL:
          memset(Aids, TRUE, sizeof(Aids));
          UpdateIcons();
          break;

     default:
          message_processed = FALSE;
          break;
     }

     return message_processed;
}

void
DrivingAidsDialog::UpdateIcons(
     void
) {
     unsigned short i;

     for (i = 0; i < NUMBER_OF_AIDS; i++) {
          InvalidateRect(GetDlgItem(Handle(), IDD_DRIVER_AID_F1 + i), NULL, FALSE);
     }
}

void
DrivingAidsDialog::OK(
     void
) {
     unsigned short i, j;
     unsigned char  b;

     memset(GP_EXE_driving_aids, 0x00, sizeof(GP_EXE_driving_aids));
     for (i = 0; i < NUMBER_OF_LEVELS; i++) {
          for (b = 0x01, j = 0; j < NUMBER_OF_AIDS; j++, b <<= 1) {
               GP_EXE_driving_aids[i] |= Aids[i][j] ? b : 0x00;
          }
     }

     End();
}

void
DrivingAidsDialog::SetNormal(
     void
) {
     memcpy(Aids, NormalAids, sizeof(Aids));
}

static BOOL
AnimatedIconInit(
     HINSTANCE instance,
     HWND      parent
) {
     HWND      hWnd      = NULL;
     BOOL      ok        = TRUE;
     WNDCLASS  Class;

     hWnd = CreateWindow("STATIC", "", WS_CHILD, 0, 0, 0, 0, parent, 0, instance, NULL);
     if (hWnd == NULL) {
          ok = FALSE;
     }

     if (ok) {
          AnimatedIconBaseClassesWndProc = (WNDPROC) GetWindowLong(hWnd, GWL_WNDPROC);

          Class.style         = GetClassWord(hWnd, GCW_STYLE);
          Class.style        &= ~CS_GLOBALCLASS;
          Class.lpfnWndProc   = AnimatedIconProc;
          Class.cbClsExtra    = GetClassWord(hWnd, GCW_CBCLSEXTRA);
          Class.cbWndExtra    = GetClassWord(hWnd, GCW_CBWNDEXTRA);
          Class.hInstance     = instance;
          Class.hIcon         = (HICON)   GetClassWord(hWnd, GCW_HICON);
          Class.hCursor       = (HCURSOR) GetClassWord(hWnd, GCW_HCURSOR);
          Class.hbrBackground = (HBRUSH)  GetClassWord(hWnd, GCW_HBRBACKGROUND);
          Class.lpszMenuName  = NULL;
          Class.lpszClassName = GPEDICON;
     }

     if (hWnd) {
          (void) DestroyWindow(hWnd);
     }

     if (ok) {
          ok = RegisterClass(&Class);
     }

     return ok;
}

LRESULT WINAPI _export
AnimatedIconProc(
     HWND      hWnd,
     UINT      msg,
     WPARAM    wParam,
     LPARAM    lParam
) {
     BOOL      message_processed   = TRUE;
     LRESULT   result              = 0;

     switch (msg) {
     case WM_PAINT:
          {
               PAINTSTRUCT    ps;
               HDC            hDC;
               char           buff[40];
               WORD           id = GetWindowWord(hWnd, GWW_ID);

               hDC = BeginPaint(hWnd, &ps);
               if (hDC != NULL) {
                    HBRUSH    hBrush;
                    RECT      rect;

                    /*
                    ** CTL3D.DLL always seems to use "Light Grey" as the background colour.
                    */
                    if (!No3D) {
                         hBrush = CreateSolidBrush(RGB(192,192,192));
                    }
                    else {
                         hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
                    }

                    ASSERT(id >= IDD_DRIVER_AID_F1 && id <= IDD_DRIVER_AID_F6);
                    id -= IDD_DRIVER_AID_F1;
                    sprintf(buff, "ICON_F%u_%s", id + 1, Aids[level][id] ? "YES" : "NO");

                    GetClientRect(hWnd, &rect);
                    (void) FillRect(hDC, &rect, hBrush);
                    (void) DrawIcon(hDC, 0, 0, LoadIcon(hSavedInstance, buff));
                    (void) DeleteObject(hBrush);
               }
               EndPaint(hWnd, &ps);
          }
          break;

     default:
          message_processed = FALSE;
          break;
     }

     if (!message_processed) {
          result = AnimatedIconBaseClassesWndProc(hWnd, msg, wParam, lParam);
     }

     return result;
}

/*---------------------------------------------------------------------------
** End of File
*/

