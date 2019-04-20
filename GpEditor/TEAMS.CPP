/*
** File   : teams.cpp
** Author : TK 
** Date   : 11/12/93
**
** $Header:   L:/GPEDITOR/VCS/TEAMS.CP_   1.4   01 Oct 1994 15:27:54   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
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
#include <shellapi.h>
/*lint +elib(???) */
#include "teams.hpp"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct {
     POINT          *points;
     unsigned short usPoints;
     HRGN           hRgn;
} CAR_REGIONS;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

static POINT FrontWingRight[] = {
     {     3,   28  },
     {    10,   25  },
     {    13,   26  },
     {     7,   31  },
};

static POINT FrontWingLeft[] = {
     {     8,   32  },
     {    13,   35  },
     {    20,   30  },
     {    17,   28  },
};

static POINT FrontWingLeftEndPlate[] = {
     {    21,   29  },
     {    21,   32  },
     {    14,   35  },
};

static POINT RearWing[] = {
     {    51,    3  },
     {    58,    4  },
     {    58,    7  },
     {    48,    6  },
};

static POINT RearWingLeftEndPlate[] = {
     {    60,    4  },
     {    60,    9  },
     {    58,    9  },
     {    58,    4  },
};

static POINT SidePodSide[] = {
     {    45,   25  },
     {    45,   18  },
     {    55,   15  },
     {    55,   18  },
     {    55,   19  },
     {    55,   20  },
     {    54,   21  },
};

static POINT SidePodTop[] = {
     {    45,   18  },
     {    42,   18  },
     {    51,   15  },
     {    52,   14  },
     {    53,   13  },
     {    55,   14  },
     {    55,   15  },
};

static POINT TubSide1[] = {
     {    20,   29  },
     {    17,   28  },
     {    26,   23  },
     {    26,   27  },
     {    21,   29  },
};

static POINT TubSide2[] = {
     {    39,   18  },
     {    41,   18  },
     {    41,   23  },
     {    40,   23  },
     {    39,   21  },
};

static POINT EngineCoverTop[] = {
     {    37,    5  },
     {    43,    5  },
     {    48,    9  },
     {    48,   10  },
     {    48,   12  },
     {    40,   13  },
     {    38,   13  },
     {    38,   12  },
     {    38,   11  },
     {    38,   10  },
     {    37,    9  },
     {    37,    8  },
     {    37,    6  },
};

static POINT EngineCoverSide[] = {
     {    48,   12  },
     {    40,   14  },
     {    37,   14  },
     {    41,   17  },
     {    43,   17  },
     {    53,   13  },
};

static POINT EngineCoverBack[] = {
     {    49,   10  },
     {    49,   12  },
     {    51,   14  },
     {    53,   13  },
};

static POINT CockpitSide[] = {
     {    31,   16  },
     {    38,   14  },
     {    41,   17  },
     {    39,   17  },
     {    39,   18  },
     {    36,   17  },
};

static POINT CockpitTop[] = {
     {    30,   13  },
     {    25,   17  },
     {    31,   16  },
     {    32,   15  },
};

static POINT FrontTop[] = {
     {     7,   31  },
     {    18,   22  },
     {    24,   17  },
     {    28,   17  },
     {    28,   16  },
     {    31,   16  },
     {    31,   17  },
     {    29,   18  },
     {    16,   27  },
     {     8,   32  },
};

static POINT FrontSide[] = {
     {     8,   32  },
     {    19,   25  },
     {    29,   18  },
     {    28,   19  },
     {    27,   21  },
     {    27,   23  },
     {    17,   28  },
     {    11,   31  },
};

static CAR_REGIONS  car_regions[NUM_CAR_REGIONS] = {
 { FrontWingRight,        NUM_ELEMENTS(FrontWingRight)           },  /*  0 */
 { FrontWingLeft,         NUM_ELEMENTS(FrontWingLeft)            },  /*  1 */
 { FrontWingLeftEndPlate, NUM_ELEMENTS(FrontWingLeftEndPlate)    },  /*  2 */
 { RearWing,              NUM_ELEMENTS(RearWing)                 },  /*  3 */
 { RearWingLeftEndPlate,  NUM_ELEMENTS(RearWingLeftEndPlate)     },  /*  4 */
 { SidePodSide,           NUM_ELEMENTS(SidePodSide)              },  /*  5 */
 { SidePodTop,            NUM_ELEMENTS(SidePodTop)               },  /*  6 */
 { TubSide1,              NUM_ELEMENTS(TubSide1)                 },  /*  7 */
 { TubSide2,              NUM_ELEMENTS(TubSide2)                 },  /*  8 */
 { EngineCoverTop,        NUM_ELEMENTS(EngineCoverTop)           },  /*  9 */
 { EngineCoverSide,       NUM_ELEMENTS(EngineCoverSide)          },  /* 10 */
 { EngineCoverBack,       NUM_ELEMENTS(EngineCoverBack)          },  /* 11 */
 { CockpitSide,           NUM_ELEMENTS(CockpitSide)              },  /* 12 */
 { CockpitTop,            NUM_ELEMENTS(CockpitTop)               },  /* 13 */
 { FrontTop,              NUM_ELEMENTS(FrontTop)                 },  /* 14 */
 { FrontSide,             NUM_ELEMENTS(FrontSide)                },  /* 15 */
};

/*---------------------------------------------------------------------------
** Functions
*/

TeamsWindow::TeamsWindow(
     HINSTANCE      hInst,
     HWND           hWndParent,
     unsigned short usWidth,
     unsigned short usHeight
) : ChildWindow(hInst, TEAMS_CLASS_NAME, GetAppName(), WS_CHILD, CHILD_ID_TEAMS, hWndParent) {
     unsigned short i;

     fInBitmap      = FALSE;
     usSelectedTeam = 0;

     usClientWidth  = usWidth;
     usClientHeight = usHeight;

     usMiniCarWidth  = usClientWidth / TEAMS_NUM_X;
     usMiniCarHeight = usClientHeight / TEAMS_NUM_Y;

     pClientBitmap = new Bitmap(pF1CarBitmap, usClientWidth, usClientHeight);
     ASSERT(pClientBitmap != NULL);

     pCursorTeamCar = new Cursor(Instance(), APP_CURSOR_TEAMCAR);
     ASSERT(pCursorTeamCar != NULL);


     CAR_REGIONS    *pCR = car_regions;
     for (i = 0; i < NUM_ELEMENTS(car_regions); i++, pCR++) {
          pCR->hRgn = CreatePolygonRgn(pCR->points, pCR->usPoints, ALTERNATE);
          ASSERT(pCR->hRgn != NULL);
     }

     UpdateMemoryImage();
     DragAcceptFiles(Handle(), TRUE);
}

TeamsWindow::~TeamsWindow(
     void
) {
     (void) SetCursor(LoadCursor(NULL, IDC_ARROW));
     delete pCursorTeamCar;

     if (pClientBitmap) {
          delete pClientBitmap;
          pClientBitmap = NULL;
     }

     unsigned short i;
     for (i = 0; i < NUM_ELEMENTS(car_regions); i++) {
          (void) DeleteObject(car_regions[i].hRgn);
          car_regions[i].hRgn = NULL;
     }
}

void
TeamsWindow::DestroyWindow(
     void
) {
     DragAcceptFiles(Handle(), FALSE);
}

BOOL
TeamsWindow::UnknownSysEventHandler(
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

     case WM_SETCURSOR:
          SetOurCursor();
          *r_val_p = TRUE;               
          break;

     case WM_DROPFILES:
          DoDrag(event->wParam);
          break;

     case IDE_TEAMS_UPDATE_ALL_CARS:
          UpdateMemoryImage();
          break;

     case IDE_TEAMS_TEAM_SELECT:
          ASSERT(event->wParam < GP_EXE_NUM_TEAMS);
          FrameActiveTeam(event->wParam);
          usSelectedTeam = event->wParam;
          if (event->lParam) {
               UpdateMemoryImage();
          }
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
TeamsWindow::Paint(
     void
) {
     PaintWindowUpdate        pwu(Handle());
     PaintCompatibleWindow    pcw(pwu.DC());
     HPALETTE                 hOldPalette;
     HPALETTE                 hOldPalette2;
     Cursor                   cursor(IDC_WAIT);

     cursor.Select();
     {
          /*
          ** Fast paint, just BitBlt image to screen.
          */
          hOldPalette = SelectPalette(pwu.DC(), hPalette, FALSE);
          hOldPalette2 = SelectPalette(pcw.DC(), hPalette, FALSE);
          (void) RealizePalette(pwu.DC());

          ASSERT(pClientBitmap != NULL);
          pcw.SelectBitmap(pClientBitmap);
          {
               /*
               ** Copy memory bitmap to real window.
               */
               BitBlt(   pwu.DC(),
                         0, 0,
                         usClientWidth, usClientHeight,
                         pcw.DC(),
                         0, 0,
                         SRCCOPY
                    );
          }
          pcw.DeselectBitmap();
          (void) SelectPalette(pwu.DC(), hOldPalette, FALSE);
          (void) SelectPalette(pcw.DC(), hOldPalette2, FALSE);
     }
     cursor.Deselect();

     return TRUE;
}

void
TeamsWindow::UpdateMemoryImage(
     void
) {
     PaintRealWindow          prw(Handle());
     PaintCompatibleWindow    pcw_client(prw.DC());
     PaintCompatibleWindow    pcw_car(prw.DC());
     HPALETTE                 hOldPalette;
     HPALETTE                 hOldPalette2;
     Cursor                   cursor(IDC_WAIT);
     unsigned short           usX;
     unsigned short           usY;
     unsigned short           usTmpX;
     unsigned short           usTmpY;
     unsigned short           usTeamIndex;
     unsigned short           i;

     cursor.Select();
     {
          hOldPalette = SelectPalette(pcw_client.DC(), hPalette, FALSE);
          hOldPalette2 = SelectPalette(pcw_car.DC(), hPalette, FALSE);

          ASSERT(pClientBitmap != NULL);
          pcw_client.SelectBitmap(pClientBitmap);
          ASSERT(pF1CarSmallBitmap != NULL);
          pcw_car.SelectBitmap(pF1CarSmallBitmap);
          {
               RECT           rRect;
               SolidBrush     GrayBrush(RGB(192,192,192));

               rRect.left    = 0;
               rRect.right   = usClientWidth;
               rRect.top     = 0;
               rRect.bottom  = usClientHeight;
               pcw_client.FillRect(&rRect, &GrayBrush);

               usTeamIndex = 0;
               for (usY = 0; usY < TEAMS_NUM_Y; usY++) {
                    for (usX = 0; usX < TEAMS_NUM_X; usX++) {
                         usTmpX = (usX % TEAMS_NUM_X) * usMiniCarWidth;
                         usTmpY = (usY % TEAMS_NUM_Y) * usMiniCarHeight;

                         for (i = 0; i < NUM_ELEMENTS(car_regions); i++) {
                              (void) SelectObject(pcw_car.DC(), car_regions[i].hRgn);
                              SolidBrush  brush(PALETTEINDEX(team_colours[usTeamIndex].usRegionColour[i]));

                              (void) FillRgn(pcw_car.DC(), car_regions[i].hRgn, (HBRUSH) brush.Handle());
                         }

                         BitBlt(   pcw_client.DC(),
                                   usTmpX, usTmpY,
                                   pF1CarSmallBitmap->Width(), pF1CarSmallBitmap->Height(),
                                   pcw_car.DC(),
                                   0, 0,
                                   SRCCOPY
                              );

                         (void) FrameTeam(&pcw_client, usTeamIndex, usTeamIndex == usSelectedTeam);
                         ++usTeamIndex;
                    }
               }
          }
          pcw_client.DeselectBitmap();
          pcw_car.DeselectBitmap();
          (void) SelectPalette(pcw_client.DC(), hOldPalette, FALSE);
          (void) SelectPalette(pcw_car.DC(), hOldPalette2, FALSE);
     }
     cursor.Deselect();

     RepaintWindow();
}

void
TeamsWindow::FrameActiveTeam(
     unsigned short usNewActiveTeam
) {
     PaintRealWindow          prw(Handle());
     PaintCompatibleWindow    pcw(prw.DC());

     ASSERT(pClientBitmap != NULL);
     pcw.SelectBitmap(pClientBitmap);
     {
          FrameTeam(&prw, usSelectedTeam, FALSE);
          FrameTeam(&pcw, usSelectedTeam, FALSE);
          FrameTeam(&prw, usNewActiveTeam, TRUE);
          FrameTeam(&pcw, usNewActiveTeam, TRUE);
     }
     pcw.DeselectBitmap();
}

void
TeamsWindow::FrameTeam(
     PaintWindow    *pPW,
     unsigned short usTeamIndex,
     BOOL           fActive
) {
     RECT           rRect;
     SolidBrush     WhiteBrush(RGB(255,255,255));
     SolidBrush     BlackBrush(RGB(0,0,0));
     SolidBrush     *pBrush;
     unsigned short usTmpX;
     unsigned short usTmpY;

     ASSERT(usTeamIndex < GP_EXE_NUM_TEAMS);

     usTmpX = (usTeamIndex % TEAMS_NUM_X) * usMiniCarWidth;
     usTmpY = (usTeamIndex / TEAMS_NUM_X) * usMiniCarHeight;

     rRect.left    = usTmpX;
     rRect.right   = rRect.left + usMiniCarWidth;
     rRect.top     = usTmpY;
     rRect.bottom  = rRect.top + usMiniCarHeight;

     pBrush = (fActive) ? &WhiteBrush : &BlackBrush;
     (void) FrameRect(pPW->DC(), &rRect, (HBRUSH) pBrush->Handle());
}

void
TeamsWindow::Mouse(
     EventMouse *event
) {
     ASSERT(event != NULL);

     ASSERT(event->X() <= usClientWidth);
     ASSERT(event->Y() <= usClientHeight);

     BOOL fLast     = fInBitmap;
     int  x         = event->X();
     int  y         = event->Y();
     int  index;

     if (y >= 0 && y < (usMiniCarHeight * TEAMS_NUM_Y)) {
          fInBitmap = TRUE;

          if (event->LeftButtonDown()) {
               x /= usMiniCarWidth;
               y /= usMiniCarHeight;

               index = (y * TEAMS_NUM_X) + x;
               ASSERT(index >= 0 && index < GP_EXE_NUM_TEAMS);
               if (index >= 0 && index < GP_EXE_NUM_TEAMS) {
                    FrameActiveTeam(index);
                    usSelectedTeam = index;
                    if (SendMessage(Parent(), IDE_DESIGN_TEAM_SELECT, usSelectedTeam, 0L)) {
                         UpdateMemoryImage();
                    }
                    (void) SendMessage(Parent(), IDE_CBAR_TEAM_SELECT, usSelectedTeam, 0L);
               }
          }
     }
     else {
          fInBitmap = FALSE;
     }

     if (fLast != fInBitmap) {
          SetOurCursor();
     }
}

void
TeamsWindow::SetOurCursor(
     void
) {
     if (fInBitmap) {
          (void) SetCursor((HICON) pCursorTeamCar->Handle());
     }
     else {
          (void) SetCursor(LoadCursor(NULL, IDC_ARROW));
     }
}

void
TeamsWindow::DoDrag(
     WORD hDragInfo
) {
     char           filename[128];
     unsigned short usNumOfFiles;
     unsigned short i;
     short          index;
     POINT          pt;
     BOOL           drop_ok = FALSE;

     usNumOfFiles = DragQueryFile((HDROP) hDragInfo, -1, NULL, 0);

     if (DragQueryPoint((HDROP) hDragInfo, &pt)) {
          short     x = pt.x;
          short     y = pt.y;

          x /= usMiniCarWidth;
          y /= usMiniCarHeight;

          index = (y * TEAMS_NUM_X) + x;

          for (i = 0; i < usNumOfFiles; i++) {
               (void) DragQueryFile((HDROP) hDragInfo, i, filename, sizeof(filename));

               ASSERT(index >= 0 && index < GP_EXE_NUM_TEAMS);
               if (index >= 0 && index < GP_EXE_NUM_TEAMS) {
                    FrameActiveTeam(index);
                    usSelectedTeam = index;
                    if (SendMessage(Parent(), IDE_DESIGN_TEAM_SELECT, usSelectedTeam, 0L)) {
                         UpdateMemoryImage();
                    }
                    (void) SendMessage(Parent(), IDE_CBAR_TEAM_SELECT, usSelectedTeam, 0L);

                    /*
                    ** Now load new car colours.
                    */
                    drop_ok = (BOOL) SendMessage(Parent(), IDE_DESIGN_LOAD, 0, (LONG) (void *) filename);
                    UpdateMemoryImage();
               }

               /*
               ** Wrap to next time on a multi-file drop.
               */
               if (drop_ok && ++index >= GP_EXE_NUM_TEAMS) {
                    index = 0;
               }
          }
     }
}

/*---------------------------------------------------------------------------
** End of File
*/

