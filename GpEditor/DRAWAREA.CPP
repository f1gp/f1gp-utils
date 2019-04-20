/*
** File   : drawarea.cpp
** Author : TK 
** Date   : 21/11/93
**
** $Header:   L:/GPEDITOR/VCS/DRAWAREA.CP_   1.4   01 Oct 1994 15:28:06   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Car Editor.
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
#include "drawarea.hpp"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define CAR_DEFAULT_COLOUR_INDEX        7
#define STARTUP_PALETTE_COLOUR_INDEX    (NUM_COLOURS_IN_PALETTE - 1)

#define MAGIC_DESIGN                    0x31464b54L

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct {
     POINT          *points;
     unsigned short usPoints;
     unsigned short usOtherRegionIndex1;
     unsigned short usOtherRegionIndex2;
     HRGN           hRgn;
} CAR_REGIONS;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static unsigned short MapRegionIndexToExeColourIndex(unsigned short usRegionIndex);
static unsigned short MapExeColourIndexToRegionIndex(unsigned short usExeColourIndex);

/*---------------------------------------------------------------------------
** Data
*/

static POINT FrontWingRight[] = {
     {    20,  173  },
     {    68,  150  },
     {    69,  150  },
     {    90,  158  },
     {    49,  185  },
};

static POINT FrontWingLeft[] = {
     {    60,  192  },
     {    94,  211  },
     {    96,  208  },
     {   143,  177  },
     {   121,  169  },
};

static POINT FrontWingLeftEndPlate[] = {
     {   149,  174  },
     {   149,  195  },
     {    96,  211  },
     {    95,  211  },
     {    96,  209  },
};

static POINT RearWing[] = {
     {   358,   21  },
     {   409,   25  },
     {   409,   41  },
     {   341,   34  },
};

static POINT RearWingLeftEndPlate[] = {
     {   425,   21  },
     {   425,   62  },
     {   410,   62  },
     {   410,   24  },
};

static POINT SidePodSide[] = {
     {   317,  150  },
     {   317,  111  },
     {   397,   90  },
     {   397,  110  },
     {   399,  111  },
     {   399,  115  },
     {   396,  120  },
     {   389,  128  },
};

static POINT SidePodTop[] = {
     {   316,  111  },
     {   293,  107  },
     {   293,  105  },
     {   357,   92  },
     {   366,   84  },
     {   370,   78  },
     {   397,   84  },
     {   397,   89  },
     {   317,  110  },
};

static POINT TubSide1[] = {
     {   143,  176  },
     {   121,  168  },
     {   189,  139  },
     {   192,  164  },
     {   150,  175  },
     {   150,  173  },
     {   148,  173  },
     {   148,  174  },
     {   146,  176  },
     {   145,  176  },
};

static POINT TubSide2[] = {
     {   269,  110  },
     {   291,  106  },
     {   291,  141  },
     {   280,  143  },
     {   276,  126  },
};

static POINT EngineCoverTop[] = {
     {   260,   33  },
     {   302,   33  },
     {   339,   57  },
     {   339,   59  },
     {   340,   60  },
     {   342,   63  },
     {   342,   74  },
     {   280,   83  },
     {   265,   83  },
     {   265,   78  },
     {   268,   77  },
     {   270,   72  },
     {   270,   67  },
     {   266,   59  },
     {   262,   55  },
     {   259,   53  },
     {   254,   53  },
     {   254,   48  },
     {   258,   47  },
     {   258,   36  },
};

static POINT EngineCoverSide[] = {
     {   342,   75  },
     {   280,   84  },
     {   265,   84  },
     {   288,  104  },
     {   293,  104  },
     {   357,   91  },
     {   361,   87  },
};

static POINT EngineCoverBack[] = {
     {   343,   61  },
     {   343,   75  },
     {   361,   86  },
     {   362,   87  },
     {   366,   83  },
     {   366,   81  },
     {   367,   81  },
     {   368,   79  },
     {   368,   78  },
     {   367,   77  },
};

static POINT CockpitSide[] = {
     {   218,   97  },
     {   265,   85  },
     {   288,  105  },
     {   288,  106  },
     {   277,  107  },
     {   277,  108  },
     {   271,  108  },
     {   271,  109  },
     {   269,  109  },
     {   254,  101  },
     {   220,  101  },
};

static POINT CockpitTop[] = {
     {   209,   81  },
     {   172,  104  },
     {   172,  105  },
     {   219,   96  },
     {   219,   95  },
     {   223,   95  },
     {   223,   94  },
     {   226,   94  },
};

static POINT FrontTop[] = {
     {    49,  186  },
     {   124,  136  },
     {   171,  105  },
     {   171,  106  },
     {   173,  106  },
     {   182,  104  },
     {   190,  103  },
     {   192,  102  },
     {   200,  101  },
     {   202,  100  },
     {   210,   99  },
     {   214,   98  },
     {   217,   98  },
     {   218,  101  },
     {   217,  101  },
     {   205,  109  },
     {   113,  162  },
     {    59,  192  },
};

static POINT FrontSide[] = {
     {    59,  192  },
     {   131,  152  },
     {   205,  109  },
     {   197,  118  },
     {   191,  127  },
     {   189,  138  },
     {   189,  139  },
     {   121,  168  },
     {   121,  169  },
     {    76,  186  },
};

static CAR_REGIONS  car_regions[NUM_CAR_REGIONS] = {
 { FrontWingRight,        NUM_ELEMENTS(FrontWingRight),         1, 3 },  /*  0 */
 { FrontWingLeft,         NUM_ELEMENTS(FrontWingLeft),          0, 3 },  /*  1 */
 { FrontWingLeftEndPlate, NUM_ELEMENTS(FrontWingLeftEndPlate), 99,99 },  /*  2 */
 { RearWing,              NUM_ELEMENTS(RearWing),               0, 1 },  /*  3 */
 { RearWingLeftEndPlate,  NUM_ELEMENTS(RearWingLeftEndPlate),  99,99 },  /*  4 */
 { SidePodSide,           NUM_ELEMENTS(SidePodSide),           99,99 },  /*  5 */
 { SidePodTop,            NUM_ELEMENTS(SidePodTop),            99,99 },  /*  6 */
 { TubSide1,              NUM_ELEMENTS(TubSide1),               8,99 },  /*  7 */
 { TubSide2,              NUM_ELEMENTS(TubSide2),               7,99 },  /*  8 */
 { EngineCoverTop,        NUM_ELEMENTS(EngineCoverTop),        99,99 },  /*  9 */
 { EngineCoverSide,       NUM_ELEMENTS(EngineCoverSide),       99,99 },  /* 10 */
 { EngineCoverBack,       NUM_ELEMENTS(EngineCoverBack),       99,99 },  /* 11 */
 { CockpitSide,           NUM_ELEMENTS(CockpitSide),           99,99 },  /* 12 */
 { CockpitTop,            NUM_ELEMENTS(CockpitTop),            99,99 },  /* 13 */
 { FrontTop,              NUM_ELEMENTS(FrontTop),              99,99 },  /* 14 */
 { FrontSide,             NUM_ELEMENTS(FrontSide),             99,99 },  /* 15 */
};

TEAM_COLOURS   team_colours[GP_EXE_NUM_TEAMS];

/*---------------------------------------------------------------------------
** Functions
*/

DrawingWindow::DrawingWindow(
     HINSTANCE hInst,
     HWND      hWndParent
) : ChildWindow(hInst, DRAWING_CLASS_NAME, GetAppName(), WS_CHILD, CHILD_ID_DRAWING, hWndParent) {
     unsigned short i;
     unsigned short j;

     usSelectedPaletteIndex        = STARTUP_PALETTE_COLOUR_INDEX;
     usCurrentTeamIndex            = 0;
     fInRegion                     = FALSE;
     fImageChanged                 = FALSE;

     CAR_REGIONS    *pCR = car_regions;
     for (i = 0; i < NUM_ELEMENTS(car_regions); i++, pCR++) {
          pCR->hRgn = CreatePolygonRgn(pCR->points, pCR->usPoints, ALTERNATE);
          ASSERT(pCR->hRgn != NULL);
     }

     pCursorSprayCan = new Cursor(Instance(), APP_CURSOR_SPRAYCAN);
     ASSERT(pCursorSprayCan != NULL);

     for (j = 0; j < NUM_ELEMENTS(team_colours); j ++) {
          for (i = 0; i < NUM_ELEMENTS(car_regions); i++) {
               team_colours[j].usRegionColour[i] = CAR_DEFAULT_COLOUR_INDEX;
          }
     }

     DragAcceptFiles(Handle(), TRUE);
}

DrawingWindow::~DrawingWindow(
     void
) {
     (void) SetCursor(LoadCursor(NULL, IDC_ARROW));
     if (pCursorSprayCan) {
          delete pCursorSprayCan;
          pCursorSprayCan = NULL;
     }

     unsigned short i;
     for (i = 0; i < NUM_ELEMENTS(car_regions); i++) {
          (void) DeleteObject(car_regions[i].hRgn);
          car_regions[i].hRgn = NULL;
     }
}

void
DrawingWindow::DestroyWindow(
     void
) {
     DragAcceptFiles(Handle(), FALSE);
}

BOOL
DrawingWindow::UnknownSysEventHandler(
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

     case IDE_DESIGN_NEW_PALETTE_COLOUR:
          usSelectedPaletteIndex = event->wParam;
          RepaintWindow();
          break;

     case IDE_DESIGN_NEW:
          DesignNew();
          break;

     case IDE_DESIGN_LOAD:
          ASSERT(event->lParam != 0L);
          *r_val_p = DesignLoad((char *) event->lParam);
          break;

     case IDE_DESIGN_SAVE:
          ASSERT(event->lParam != 0L);
          *r_val_p = DesignSave((char *) event->lParam);
          break;

     case IDE_DESIGN_LOAD_EXE_COLOURS:
          LoadExeColours((GP_EXE_CAR_COLOURS *) event->lParam);
          break;

     case IDE_DESIGN_SAVE_EXE_COLOURS:
          SaveExeColours((GP_EXE_CAR_COLOURS *) event->lParam);
          break;

     case IDE_DESIGN_TEAM_SELECT:
          *r_val_p = SelectTeam(event->wParam);
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
DrawingWindow::Paint(
     void
) {
     PaintWindowUpdate        pwu(Handle());
     PaintCompatibleWindow    pcw(pwu.DC());
     HPALETTE                 hOldPalette;
     HPALETTE                 hOldPalette2;
     Cursor                   cursor(IDC_WAIT);

     cursor.Select();
     {
          hOldPalette = SelectPalette(pwu.DC(), hPalette, FALSE);
          hOldPalette2 = SelectPalette(pcw.DC(), hPalette, FALSE);

          ASSERT(pF1CarBitmap != NULL);
          pcw.SelectBitmap(pF1CarBitmap);
          {
               unsigned short x;
               RECT           rect;

               /*
               ** Copy F1 Car bitmap to client bitmap.
               */
               BitBlt(   pwu.DC(),
                         0, 0,
                         pF1CarBitmap->Width(), pF1CarBitmap->Height(),
                         pcw.DC(),
                         0, 0,
                         SRCCOPY
                    );

               /*
               ** Draw box with currently selected palette colour.
               */
               x = pF1CarBitmap->Width() / PALETTE_BOX_ITEMS_PER_LINE;

               SolidBrush brush(PALETTEINDEX(usSelectedPaletteIndex));
               rect.right   = pF1CarBitmap->Width() - x;
               rect.left    = rect.right - x * 2;
               rect.bottom  = pF1CarBitmap->Height() - PALETTE_BOX_ITEM_HEIGHT;
               rect.top     = rect.bottom - PALETTE_BOX_ITEM_HEIGHT * 2;
               pwu.FillRect(&rect, &brush);

               SolidBrush     BlackBrush(RGB(0,0,0));
               (void) FrameRect(pwu.DC(), &rect, (HBRUSH) BlackBrush.Handle());
          }
          pcw.DeselectBitmap();
          (void) SelectPalette(pwu.DC(), hOldPalette, FALSE);
          (void) SelectPalette(pcw.DC(), hOldPalette2, FALSE);
     }
     cursor.Deselect();

     return TRUE;
}

void
DrawingWindow::Mouse(
     EventMouse *event
) {
     unsigned short i;
     unsigned short usCarRegionIndex    = 0;
     BOOL           found;
     BOOL           fLast = fInRegion;

     ASSERT(event != NULL);
     ASSERT(pF1CarBitmap != NULL);
     ASSERT(event->X() <= pF1CarBitmap->Width());
     ASSERT(event->Y() <= pF1CarBitmap->Height());

     for (i = 0, found = FALSE; !found && i < NUM_ELEMENTS(car_regions); i++) {
          if (PtInRegion(car_regions[i].hRgn, event->X(), event->Y())) {
               found            = TRUE;
               usCarRegionIndex = i;
          }
     }

     fInRegion = found;

     /*
     ** Select current panel colour when Right mouse button is pressed,
     */
     if (event->RightButtonDown() && found) {
          usSelectedPaletteIndex = team_colours[usCurrentTeamIndex].usRegionColour[usCarRegionIndex];
          RepaintWindow();
     }

     /*
     ** Fill panel when Left mouse button is pressed.
     */
     if (event->LeftButtonDown() && found) {
          FillCarRegion(usCarRegionIndex, usSelectedPaletteIndex);

          if (car_regions[usCarRegionIndex].usOtherRegionIndex1 < NUM_ELEMENTS(car_regions)) {
               FillCarRegion(car_regions[usCarRegionIndex].usOtherRegionIndex1, usSelectedPaletteIndex);
          }

          if (car_regions[usCarRegionIndex].usOtherRegionIndex2 < NUM_ELEMENTS(car_regions)) {
               FillCarRegion(car_regions[usCarRegionIndex].usOtherRegionIndex2, usSelectedPaletteIndex);
          }

          fImageChanged = TRUE;
          RepaintWindow();
     }

     if (fLast != fInRegion) {
          SetOurCursor();
     }
}

void
DrawingWindow::SetOurCursor(
     void
) {
     if (fInRegion) {
          (void) SetCursor((HICON) pCursorSprayCan->Handle());
     }
     else {
          (void) SetCursor(LoadCursor(NULL, IDC_ARROW));
     }
}

void
DrawingWindow::FillCarRegion(
     unsigned short usCarRegionIndex,
     unsigned short usPaletteIndex
) {
     PaintRealWindow          prw(Handle());
     PaintCompatibleWindow    pcw(prw.DC());
     unsigned short           i = usCarRegionIndex;
     HPALETTE                 hOldPalette;

     ASSERT(usCarRegionIndex <= NUM_ELEMENTS(car_regions));
     ASSERT(usPaletteIndex <= 255);

     hOldPalette = SelectPalette(pcw.DC(), hPalette, FALSE);

     pcw.SelectBitmap(pF1CarBitmap);
     (void) SelectObject(pcw.DC(), car_regions[i].hRgn);
     {
          SolidBrush  brush(PALETTEINDEX(usPaletteIndex));
          SolidBrush  brush_lines(RGB(255,255,255));

          (void) FillRgn(pcw.DC(), car_regions[i].hRgn, (HBRUSH) brush.Handle());
          (void) FrameRgn(pcw.DC(), car_regions[i].hRgn, (HBRUSH) brush_lines.Handle(), 1, 1);

          team_colours[usCurrentTeamIndex].usRegionColour[i] = usPaletteIndex;
     }
     pcw.DeselectBitmap();
     (void) SelectPalette(pcw.DC(), hOldPalette, FALSE);
}

void
DrawingWindow::DesignNew(
     void
) {
     Cursor         cursor(IDC_WAIT);
     unsigned short i;

     cursor.Select();
     {
          for (i = 0; i < NUM_ELEMENTS(car_regions); i++) {
               team_colours[usCurrentTeamIndex].usRegionColour[i] = CAR_DEFAULT_COLOUR_INDEX;
               FillCarRegion(i, team_colours[usCurrentTeamIndex].usRegionColour[i]);
          }

          fImageChanged = TRUE;
          RepaintWindow();
     }
     cursor.Deselect();
}

BOOL
DrawingWindow::DesignLoad(
     char *filename
) {
     BOOL           ok = TRUE;
     Cursor         cursor(IDC_WAIT);
     FILE           *fp;
     unsigned short i;

     cursor.Select();
     {
          if ((fp = fopen(filename, "rb")) != NULL) {
               long l;

               ok = fread(&l, sizeof(l), 1, fp);
               if (ok) {
                    if (l == MAGIC_DESIGN) {
                         for (i = 0; ok && i < NUM_ELEMENTS(car_regions); i++) {
                              ok = fread(&team_colours[usCurrentTeamIndex].usRegionColour[i], sizeof(team_colours[usCurrentTeamIndex].usRegionColour[i]), 1, fp);
                         }

                         if (!ok) {
                              sprintf(msg_buff, GetIdt(IDT_READ_FAILED), filename);
                              MsgBoxError mb(GetAppName(), msg_buff);
                              (void) mb.ProcessMessages();
                         }
                    }
                    else {
                         ok = FALSE;
                         sprintf(msg_buff, GetIdt(IDT_NOT_F1_DESIGN_FILE), filename);
                         MsgBoxWarning mb(GetAppName(), msg_buff);
                         (void) mb.ProcessMessages();
                    }
               }
               else {
                    sprintf(msg_buff, GetIdt(IDT_READ_FAILED), filename);
                    MsgBoxError mb(GetAppName(), msg_buff);
                    (void) mb.ProcessMessages();
               }

               (void) fclose(fp);
          }
          else {
               ok = FALSE;
               sprintf(msg_buff, GetIdt(IDT_UNABLE_TO_OPEN), filename);
               MsgBoxWarning mb(GetAppName(), msg_buff);
               (void) mb.ProcessMessages();
          }

          if (ok) {
               for (i = 0; i < NUM_ELEMENTS(car_regions); i++) {
                    FillCarRegion(i, team_colours[usCurrentTeamIndex].usRegionColour[i]);
               }
               fImageChanged = TRUE;
               RepaintWindow();
          }
     }
     cursor.Deselect();

     return ok;
}

BOOL
DrawingWindow::DesignSave(
     char *filename
) {
     BOOL           ok = TRUE;
     Cursor         cursor(IDC_WAIT);
     FILE           *fp;
     unsigned short i;

     cursor.Select();
     {
          if ((fp = fopen(filename, "wb")) != NULL) {
               long l = MAGIC_DESIGN;
               ok = fwrite(&l, sizeof(l), 1, fp);
               for (i = 0; ok && i < NUM_ELEMENTS(car_regions); i++) {
                    ok = fwrite(&team_colours[usCurrentTeamIndex].usRegionColour[i], sizeof(team_colours[usCurrentTeamIndex].usRegionColour[i]), 1, fp);
               }

               if (!ok) {
                    sprintf(msg_buff, GetIdt(IDT_WRITE_FAILED), filename);
                    MsgBoxError mb(GetAppName(), msg_buff);
                    (void) mb.ProcessMessages();
               }

               (void) fclose(fp);
          }
          else {
               ok = FALSE;
               sprintf(msg_buff, GetIdt(IDT_UNABLE_TO_OPEN), filename);
               MsgBoxWarning mb(GetAppName(), msg_buff);
               (void) mb.ProcessMessages();
          }
     }
     cursor.Deselect();

     return ok;
}

void
DrawingWindow::LoadExeColours(
     GP_EXE_CAR_COLOURS  *pNewGpExeCarColours
) {
     unsigned short i, j;
     unsigned short exe_index;

     for (j = 0; j < NUM_ELEMENTS(team_colours); j ++) {
          for (i = 0; i < NUM_ELEMENTS(car_regions); i++) {
               exe_index = MapRegionIndexToExeColourIndex(i);
               team_colours[j].usRegionColour[i] = pNewGpExeCarColours[j].colours[exe_index];
          }
     }
     fImageChanged = TRUE;
}

void
DrawingWindow::SaveExeColours(
     GP_EXE_CAR_COLOURS  *pNewGpExeCarColours
) {
     unsigned short i, j;
     unsigned short index;

     for (j = 0; j < NUM_ELEMENTS(team_colours); j++) {
          for (i = 0; i < NUM_ELEMENTS(pNewGpExeCarColours[j].colours); i++) {
               if (i != 8 && i != 9) {
                    index = MapExeColourIndexToRegionIndex(i);
                    pNewGpExeCarColours[j].colours[i] = team_colours[j].usRegionColour[index];
               }
          }
     }
}

BOOL
DrawingWindow::SelectTeam(
     unsigned short usNewTeamIndex
) {
     unsigned short i;
     BOOL           fSaved = fImageChanged;
     Cursor         cursor(IDC_WAIT);

     cursor.Select();
     {
          usCurrentTeamIndex = usNewTeamIndex;

          for (i = 0; i < NUM_ELEMENTS(car_regions); i++) {
               FillCarRegion(i, team_colours[usCurrentTeamIndex].usRegionColour[i]);
          }
          fImageChanged = FALSE;
          RepaintWindow();
     }
     cursor.Deselect();

     return fSaved;
}

void
DrawingWindow::DoDrag(
     WORD hDragInfo
) {
     unsigned short usNumOfFiles;
     char           filename[128];

     usNumOfFiles = DragQueryFile((HDROP) hDragInfo, -1, NULL, 0);
     if (usNumOfFiles) {
          (void) DragQueryFile((HDROP) hDragInfo, 0, filename, sizeof(filename));
          (void) DesignLoad(filename);
     }

     if (usNumOfFiles > 1) {
          MessageBeep(MB_ICONEXCLAMATION);
     }
}

static unsigned short
MapRegionIndexToExeColourIndex(
     unsigned short usRegionIndex
) {
     static unsigned short map[] = {
          6, 6, 2, 6, 3, 5, 14, 4, 4, 0, 12, 13, 11, 1, 7, 10
     };

     ASSERT(usRegionIndex < NUM_ELEMENTS(map));
     return map[usRegionIndex];
}

static unsigned short
MapExeColourIndexToRegionIndex(
     unsigned short usExeColourIndex
) {
     static unsigned short map[] = {
           9,       /* Index: 0 Panel: 1 "Engine Cover Top"           */
          13,       /* Index: 1 Panel: 2 "Cockpit Top"                */
           2,       /* Index: 2 Panel: 3 "Left Front Wing End Plate"  */
           4,       /* Index: 3 Panel: 4 "Left Rear Wing End Plate"   */
           7,       /* Index: 4 Panel: 5 "Tub Side"                   */
           5,       /* Index: 5 Panel: 6 "Side Pod Side"              */
           0,       /* Index: 6 Panel: 7 "Right Front Wing"           */
          14,       /* Index: 7 Panel: 8 "Front Top"                  */
           0,       /* Index: 8 Panel: 9 "--- Unused ---"             */
           0,       /* Index: 9 Panel:10 "--- Unused ---"             */
          15,       /* Index:10 Panel:11 "Front Side"                 */
          12,       /* Index:11 Panel:12 "Cockpit Side"               */
          10,       /* Index:12 Panel:13 "Engine Cover Side"          */
          11,       /* Index:13 Panel:14 "Engine Cover Back"          */
           6        /* Index:14 Panel:15 "Side Pod Top"               */
     };             

     ASSERT(usExeColourIndex < NUM_ELEMENTS(map));
     ASSERT(usExeColourIndex != 8);     /* Not supported */
     ASSERT(usExeColourIndex != 9);     /* Not supported */

     return map[usExeColourIndex];
}

/*---------------------------------------------------------------------------
** End of File
*/

