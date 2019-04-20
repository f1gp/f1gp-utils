/*
** File   : drawhelm.cpp
** Author : TK 
** Date   :  8/01/94
**
** $Header:   L:/GPEDITOR/VCS/DRAWHELM.CP_   1.5   01 Oct 1994 15:28:02   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Helmet Editor.
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
#include "drawhelm.hpp"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define HELMET_DEFAULT_COLOUR_INDEX     7
#define STARTUP_PALETTE_COLOUR_INDEX    (NUM_COLOURS_IN_PALETTE - 1)

#define MAGIC_HELMET                    0x45484b54L

#define PACKED_RECORD_MAX_REGIONS       6

#define UNUSED_COORD                    1000

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct {
     POINT          *points;
     unsigned short usPoints;
     HRGN           hRgn;
} HELMET_REGIONS;

typedef struct {
     unsigned char  usRegionColour[NUM_HELMET_REGIONS];
} HELMET_COLOURS;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

static POINT Region0[] = {
     {   140,  123  },
     {   149,  114  },
     {   164,   83  },
     {   205,   83  },
     {   244,   98  },
     {   244,  115  },
     {   236,  123  },
     {   140,  123  },
};

static POINT Region1[] = {    /* Unknown region! Always 0x01 in EXE file */
     { UNUSED_COORD,     UNUSED_COORD        },
     { UNUSED_COORD + 5, UNUSED_COORD        },
     { UNUSED_COORD + 5, UNUSED_COORD + 5    },
     { UNUSED_COORD,     UNUSED_COORD        },
};

static POINT Region2[] = {
     {   198,   58   },
     {   227,   43   },
     {   284,   43   },
     {   313,   58   },
     {   198,   58   },
};

static POINT Region3[] = {
     {   182,   66  },
     {   198,   58  },
     {   314,   58  },
     {   322,   66  },
     {   182,   66  },
};

static POINT Region4[] = {
     {   173,   74  },
     {   181,   66  },
     {   322,   66  },
     {   327,   74  },
     {   173,   74  },
};

static POINT Region5[] = {
     {   240,   82  },
     {   216,   74  },
     {   327,   74  },
     {   330,   82  },
     {   240,   82  },
};

static POINT Region6[] = {
     {   140,  138  },
     {   140,  123  },
     {   236,  123  },
     {   244,  115  },
     {   244,   98  },
     {   205,   83  },
     {   164,   83  },
     {   173,   74  },
     {   216,   74  },
     {   268,   91  },
     {   268,   99  },
     {   260,   99  },
     {   260,  106  },
     {   268,  106  },
     {   268,  114  },
     {   244,  138  },
     {   140,  138  },
};

static POINT Region7[] = {
     {   268,   99  },
     {   268,   91  },
     {   241,   82  },
     {   331,   82  },
     {   331,   99  },
     {   268,   99  },
};

static POINT Region8[] = {
     {   268,  114  },
     {   268,   99  },
     {   331,   99  },
     {   331,  114  },
     {   268,  114  },
};

static POINT Region9[] = {
     {   259,  123  },
     {   268,  114  },
     {   331,  114  },
     {   331,  123  },
     {   259,  123  },
};

static POINT Region10[] = {
     {   251,  131  },
     {   259,  123  },
     {   331,  123  },
     {   331,  131  },
     {   251,  131  },
};

static POINT Region11[] = {
     {   244,  138  },
     {   251,  131  },
     {   331,  131  },
     {   331,  138  },
     {   244,  138  },
};

static POINT Region12[] = {
     {   140,  147  },
     {   140,  138  },
     {   331,  138  },
     {   327,  147  },
     {   140,  147  },
};

static POINT Region13[] = {
     {   140,  154  },
     {   140,  147  },
     {   327,  147  },
     {   325,  154  },
};

static POINT Region14[] = {
     {   143,  163  },
     {   140,  154  },
     {   324,  154  },
     {   321,  163  },
     {   143,  163  },
};

static POINT Region15[] = {
     {   148,  178  },
     {   143,  163  },
     {   321,  163  },
     {   315,  178  },
     {   148,  178  },
};

static HELMET_REGIONS  helmet_regions[NUM_HELMET_REGIONS] = {
     {    Region0,  NUM_ELEMENTS(Region0)    },   /*  0 */
     {    Region1,  NUM_ELEMENTS(Region1)    },   /*  1 */
     {    Region2,  NUM_ELEMENTS(Region2)    },   /*  2 */
     {    Region3,  NUM_ELEMENTS(Region3)    },   /*  3 */
     {    Region4,  NUM_ELEMENTS(Region4)    },   /*  4 */
     {    Region5,  NUM_ELEMENTS(Region5)    },   /*  5 */
     {    Region6,  NUM_ELEMENTS(Region6)    },   /*  6 */
     {    Region7,  NUM_ELEMENTS(Region7)    },   /*  7 */
     {    Region8,  NUM_ELEMENTS(Region8)    },   /*  8 */
     {    Region9,  NUM_ELEMENTS(Region9)    },   /*  9 */
     {    Region10, NUM_ELEMENTS(Region10)   },   /* 10 */
     {    Region11, NUM_ELEMENTS(Region11)   },   /* 11 */
     {    Region12, NUM_ELEMENTS(Region12)   },   /* 12 */
     {    Region13, NUM_ELEMENTS(Region13)   },   /* 13 */
     {    Region14, NUM_ELEMENTS(Region14)   },   /* 14 */
     {    Region15, NUM_ELEMENTS(Region14)   },   /* 15 */
};

static HELMET_COLOURS    driver_colours[GP_EXE_NUM_DRIVERS];

/*---------------------------------------------------------------------------
** Functions
*/

HelmetWindow::HelmetWindow(
     HINSTANCE hInst,
     HWND      hWndParent
) : ChildWindow(hInst, HELMET_CLASS_NAME, GetAppName(), WS_CHILD, CHILD_ID_HELMET, hWndParent) {
     unsigned short i;

     #pragma warn -rch
     #pragma warn -ccc
     ASSERT(GP_EXE_NUM_DRIVERS == NUM_ELEMENTS(driver_colours));
     #pragma warn -ccc
     #pragma warn -rch

     usSelectedPaletteIndex        = STARTUP_PALETTE_COLOUR_INDEX;
     fInRegion                     = FALSE;
     fImageChanged                 = FALSE;
     usCurrentDriverIndex          = 0;

     /*
     ** Select first enabled driver.
     */
     for (i = 0; i < NUM_ELEMENTS(driver_colours); i++) {
          if (IsDriverEnabled(i)) {
               usCurrentDriverIndex = DriverIndex(i);
               break;
          }
     }

     HELMET_REGIONS *pHR = helmet_regions;
     for (i = 0; i < NUM_ELEMENTS(helmet_regions); i++, pHR++) {
          pHR->hRgn = CreatePolygonRgn(pHR->points, pHR->usPoints, ALTERNATE);
          ASSERT(pHR->hRgn != NULL);
     }

     pCursorSprayCan = new Cursor(Instance(), APP_CURSOR_SPRAYCAN);
     ASSERT(pCursorSprayCan != NULL);

     DragAcceptFiles(Handle(), TRUE);
}

HelmetWindow::~HelmetWindow(
     void
) {
     (void) SetCursor(LoadCursor(NULL, IDC_ARROW));
     if (pCursorSprayCan) {
          delete pCursorSprayCan;
          pCursorSprayCan = NULL;
     }

     unsigned short i;
     for (i = 0; i < NUM_ELEMENTS(helmet_regions); i++) {
          (void) DeleteObject(helmet_regions[i].hRgn);
          helmet_regions[i].hRgn = NULL;
     }
}

void
HelmetWindow::DestroyWindow(
     void
) {
     DragAcceptFiles(Handle(), FALSE);
}

BOOL
HelmetWindow::UnknownSysEventHandler(
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

     case IDE_HELMET_NEW_PALETTE_COLOUR:
          usSelectedPaletteIndex = event->wParam;
          RepaintWindow();
          break;

     case IDE_HELMET_NEW:
          DesignNew();
          break;

     case IDE_HELMET_LOAD:
          ASSERT(event->lParam != 0L);
          *r_val_p = DesignLoad((char *) event->lParam);
          break;

     case IDE_HELMET_SAVE:
          ASSERT(event->lParam != 0L);
          *r_val_p = DesignSave((char *) event->lParam);
          break;

     case IDE_HELMET_LOAD_EXE_COLOURS:
          LoadExeColours((GP_EXE_HELMET_COLOURS *) event->lParam);
          break;

     case IDE_HELMET_SAVE_EXE_COLOURS:
          SaveExeColours((GP_EXE_HELMET_COLOURS *) event->lParam);
          break;

     case IDE_HELMET_DRIVER_SELECT:
          *r_val_p = SelectDriver(event->wParam);
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
HelmetWindow::Paint(
     void
) {
     static BOOL first = TRUE;

     if (first) {
          first = FALSE;

          if (!fGpExeIsFound) {
               DesignNew();
          }
     }

     PaintWindowUpdate        pwu(Handle());
     PaintCompatibleWindow    pcw(pwu.DC());
     HPALETTE                 hOldPalette;
     HPALETTE                 hOldPalette2;
     Cursor                   cursor(IDC_WAIT);

     cursor.Select();
     {
          hOldPalette = SelectPalette(pwu.DC(), hPalette, FALSE);
          hOldPalette2 = SelectPalette(pcw.DC(), hPalette, FALSE);

          ASSERT(pHelmetBitmap != NULL);
          pcw.SelectBitmap(pHelmetBitmap);
          {
               unsigned short x;
               RECT           rect;

               /*
               ** Copy Helmet bitmap to client bitmap.
               */
               BitBlt(   pwu.DC(),
                         0, 0,
                         pHelmetBitmap->Width(), pHelmetBitmap->Height(),
                         pcw.DC(),
                         0, 0,
                         SRCCOPY
                    );

               /*
               ** Draw box with currently selected palette colour.
               */
               x = pHelmetBitmap->Width() / PALETTE_BOX_ITEMS_PER_LINE;

               SolidBrush brush(PALETTEINDEX(usSelectedPaletteIndex));
               rect.right   = pHelmetBitmap->Width() - x;
               rect.left    = rect.right - x * 2;
               rect.bottom  = pHelmetBitmap->Height() - PALETTE_BOX_ITEM_HEIGHT;
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
HelmetWindow::Mouse(
     EventMouse *event
) {
     unsigned short i;
     unsigned short usHelmetRegionIndex = 0;
     BOOL           found;
     BOOL           fLast = fInRegion;

     ASSERT(event != NULL);
     ASSERT(pHelmetBitmap != NULL);
     ASSERT(event->X() <= pHelmetBitmap->Width());
     ASSERT(event->Y() <= pHelmetBitmap->Height());

     for (i = 0, found = FALSE; !found && i < NUM_ELEMENTS(helmet_regions); i++) {
          if (PtInRegion(helmet_regions[i].hRgn, event->X(), event->Y())) {
               found               = TRUE;
               usHelmetRegionIndex = i;
          }
     }

     fInRegion = found;

     /*
     ** Select current panel colour when Right mouse button is pressed,
     */
     if (event->RightButtonDown() && found) {
          usSelectedPaletteIndex = driver_colours[usCurrentDriverIndex].usRegionColour[usHelmetRegionIndex];
          RepaintWindow();
     }

     /*
     ** Fill panel when Left mouse button is pressed.
     */
     if (event->LeftButtonDown() && found) {
          /*
          ** Special handling of linked regions.
          */
          if ( usHelmetRegionIndex >= PACKED_RECORD_MAX_REGIONS - 1 && IsLinkedHelmet(usCurrentDriverIndex)) {
               for (i = PACKED_RECORD_MAX_REGIONS - 1; i < NUM_ELEMENTS(helmet_regions); i++) {
                    FillHelmetRegion(i, usSelectedPaletteIndex);
               }
          }
          else {
               FillHelmetRegion(usHelmetRegionIndex, usSelectedPaletteIndex);
          }

          fImageChanged = TRUE;
          RepaintWindow();
     }

     if (fLast != fInRegion) {
          SetOurCursor();
     }
}

void
HelmetWindow::SetOurCursor(
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
HelmetWindow::FillHelmetRegion(
     unsigned short usHelmetRegionIndex,
     unsigned short usPaletteIndex
) {
     PaintRealWindow          prw(Handle());
     PaintCompatibleWindow    pcw(prw.DC());
     unsigned short           i = usHelmetRegionIndex;
     HPALETTE                 hOldPalette;

     ASSERT(usHelmetRegionIndex <= NUM_ELEMENTS(helmet_regions));
     ASSERT(usPaletteIndex <= 255);

     if (helmet_regions[i].points[0].x >= UNUSED_COORD) {
          return;
     }

     hOldPalette = SelectPalette(pcw.DC(), hPalette, FALSE);

     pcw.SelectBitmap(pHelmetBitmap);
     (void) SelectObject(pcw.DC(), helmet_regions[i].hRgn);
     {
          SolidBrush brush(PALETTEINDEX(usPaletteIndex));
          SolidBrush brush_lines(RGB(255,255,255));

          (void) FillRgn(pcw.DC(), helmet_regions[i].hRgn, (HBRUSH) brush.Handle()); 
          driver_colours[usCurrentDriverIndex].usRegionColour[i] = usPaletteIndex;

          Pen pen(1, RGB(255,255,255));
          pcw.SelectPen(&pen);
          {
               unsigned short j;

               for (j = 1; j < helmet_regions[i].usPoints; j++) {
                    pcw.MoveTo(helmet_regions[i].points[j-1].x, helmet_regions[i].points[j-1].y);
                    pcw.LineTo(helmet_regions[i].points[j].x, helmet_regions[i].points[j].y);
               }
          }
          pcw.DeselectPen();
     }
     pcw.DeselectBitmap();
     (void) SelectPalette(pcw.DC(), hOldPalette, FALSE);
}

void
HelmetWindow::DesignNew(
     void
) {
     Cursor         cursor(IDC_WAIT);
     unsigned short i;

     cursor.Select();
     {
          for (i = 0; i < NUM_ELEMENTS(helmet_regions); i++) {
               if (i == 0) {
                    driver_colours[usCurrentDriverIndex].usRegionColour[i] = 0;
               }
               else {
                    driver_colours[usCurrentDriverIndex].usRegionColour[i] = HELMET_DEFAULT_COLOUR_INDEX;
               }
               FillHelmetRegion(i, driver_colours[usCurrentDriverIndex].usRegionColour[i]);
          }

          fImageChanged = TRUE;
          RepaintWindow();
     }
     cursor.Deselect();
}

BOOL
HelmetWindow::DesignLoad(
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
                    if (l == MAGIC_HELMET) {
                         for (i = 0; ok && i < NUM_ELEMENTS(helmet_regions); i++) {
                              ok = fread(&driver_colours[usCurrentDriverIndex].usRegionColour[i], sizeof(driver_colours[usCurrentDriverIndex].usRegionColour[i]), 1, fp);
                         }

                         if (!ok) {
                              sprintf(msg_buff, GetIdt(IDT_READ_FAILED), filename);
                              MsgBoxError mb(GetAppName(), msg_buff);
                              (void) mb.ProcessMessages();
                         }
                    }
                    else {
                         ok = FALSE;
                         sprintf(msg_buff, GetIdt(IDT_NOT_F1_HELMET_FILE), filename);
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
               for (i = 0; i < NUM_ELEMENTS(helmet_regions); i++) {
                    FillHelmetRegion(i, driver_colours[usCurrentDriverIndex].usRegionColour[i]);
               }
               fImageChanged = TRUE;
               RepaintWindow();
          }
     }
     cursor.Deselect();

     return ok;
}

BOOL
HelmetWindow::DesignSave(
     char *filename
) {
     BOOL           ok = TRUE;
     Cursor         cursor(IDC_WAIT);
     FILE           *fp;
     unsigned short i;

     cursor.Select();
     {
          if ((fp = fopen(filename, "wb")) != NULL) {
               long l = MAGIC_HELMET;
               ok = fwrite(&l, sizeof(l), 1, fp);
               for (i = 0; ok && i < NUM_ELEMENTS(helmet_regions); i++) {
                    ok = fwrite(&driver_colours[usCurrentDriverIndex].usRegionColour[i], sizeof(driver_colours[usCurrentDriverIndex].usRegionColour[i]), 1, fp);
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
HelmetWindow::LoadExeColours(
     GP_EXE_HELMET_COLOURS    *pNewGpExeHelmetColours
) {
     memcpy(&driver_colours[ 0], pNewGpExeHelmetColours->driver01, sizeof(driver_colours[ 0]));
     memcpy(&driver_colours[ 1], pNewGpExeHelmetColours->driver02, sizeof(driver_colours[ 1]));
     memcpy(&driver_colours[ 2], pNewGpExeHelmetColours->driver03, sizeof(driver_colours[ 2]));
     memcpy(&driver_colours[ 3], pNewGpExeHelmetColours->driver04, sizeof(driver_colours[ 3]));
     memcpy(&driver_colours[ 4], pNewGpExeHelmetColours->driver05, sizeof(driver_colours[ 4]));
     memcpy(&driver_colours[ 5], pNewGpExeHelmetColours->driver06, sizeof(driver_colours[ 5]));
     memcpy(&driver_colours[ 6], pNewGpExeHelmetColours->driver07, sizeof(driver_colours[ 6]));
     memcpy(&driver_colours[ 7], pNewGpExeHelmetColours->driver08, sizeof(driver_colours[ 7]));
     memcpy(&driver_colours[ 8], pNewGpExeHelmetColours->driver09, sizeof(driver_colours[ 8]));
     memcpy(&driver_colours[ 9], pNewGpExeHelmetColours->driver10, sizeof(driver_colours[ 9]));
     memcpy(&driver_colours[10], pNewGpExeHelmetColours->driver11, sizeof(driver_colours[10]));
     memcpy(&driver_colours[11], pNewGpExeHelmetColours->driver12, sizeof(driver_colours[11]));
     /* Driver 13 (index 12) is a packed record */
     memcpy(&driver_colours[13], pNewGpExeHelmetColours->driver14, sizeof(driver_colours[13]));
     /* Driver 15 (index 14) is a packed record */
     memcpy(&driver_colours[15], pNewGpExeHelmetColours->driver16, sizeof(driver_colours[15]));
     memcpy(&driver_colours[16], pNewGpExeHelmetColours->driver17, sizeof(driver_colours[16]));
     memcpy(&driver_colours[17], pNewGpExeHelmetColours->driver18, sizeof(driver_colours[17]));
     memcpy(&driver_colours[18], pNewGpExeHelmetColours->driver19, sizeof(driver_colours[18]));
     memcpy(&driver_colours[19], pNewGpExeHelmetColours->driver20, sizeof(driver_colours[19]));
     memcpy(&driver_colours[20], pNewGpExeHelmetColours->driver21, sizeof(driver_colours[20]));
     memcpy(&driver_colours[21], pNewGpExeHelmetColours->driver22, sizeof(driver_colours[21]));
     memcpy(&driver_colours[22], pNewGpExeHelmetColours->driver23, sizeof(driver_colours[22]));
     memcpy(&driver_colours[23], pNewGpExeHelmetColours->driver24, sizeof(driver_colours[23]));
     memcpy(&driver_colours[24], pNewGpExeHelmetColours->driver25, sizeof(driver_colours[24]));
     memcpy(&driver_colours[25], pNewGpExeHelmetColours->driver26, sizeof(driver_colours[25]));
     memcpy(&driver_colours[26], pNewGpExeHelmetColours->driver27, sizeof(driver_colours[26]));
     memcpy(&driver_colours[27], pNewGpExeHelmetColours->driver28, sizeof(driver_colours[27]));
     memcpy(&driver_colours[28], pNewGpExeHelmetColours->driver29, sizeof(driver_colours[28]));
     memcpy(&driver_colours[29], pNewGpExeHelmetColours->driver30, sizeof(driver_colours[29]));
     memcpy(&driver_colours[30], pNewGpExeHelmetColours->driver31, sizeof(driver_colours[30]));
     memcpy(&driver_colours[31], pNewGpExeHelmetColours->driver32, sizeof(driver_colours[31]));
     memcpy(&driver_colours[32], pNewGpExeHelmetColours->driver33, sizeof(driver_colours[32]));
     memcpy(&driver_colours[33], pNewGpExeHelmetColours->driver34, sizeof(driver_colours[33]));
     memcpy(&driver_colours[34], pNewGpExeHelmetColours->driver35, sizeof(driver_colours[34]));
     /* Driver 36 (index 35) is a packed record */
     /* Driver 37 (index 36) is a packed record */
     /* Driver 38 (index 37) is a packed record */
     /* Driver 39 (index 38) is a packed record */
     /* Driver 40 (index 39) is a packed record */

     /*
     ** Special handling for packed records.
     */
     char c;
     
     c = pNewGpExeHelmetColours->driver13[10];
     memset(&driver_colours[12], c, sizeof(driver_colours[12]));
     memcpy(&driver_colours[12], pNewGpExeHelmetColours->driver13, PACKED_RECORD_MAX_REGIONS);

     c = pNewGpExeHelmetColours->driver15[10];
     memset(&driver_colours[14], c, sizeof(driver_colours[14]));
     memcpy(&driver_colours[14], pNewGpExeHelmetColours->driver15, PACKED_RECORD_MAX_REGIONS);

     c = pNewGpExeHelmetColours->driver36[10];
     memset(&driver_colours[35], c, sizeof(driver_colours[14]));
     memcpy(&driver_colours[35], pNewGpExeHelmetColours->driver36, PACKED_RECORD_MAX_REGIONS);

     c = pNewGpExeHelmetColours->driver37[10];
     memset(&driver_colours[36], c, sizeof(driver_colours[14]));
     memcpy(&driver_colours[36], pNewGpExeHelmetColours->driver37, PACKED_RECORD_MAX_REGIONS);

     c = pNewGpExeHelmetColours->driver38[10];
     memset(&driver_colours[37], c, sizeof(driver_colours[14]));
     memcpy(&driver_colours[37], pNewGpExeHelmetColours->driver38, PACKED_RECORD_MAX_REGIONS);

     c = pNewGpExeHelmetColours->driver39[10];
     memset(&driver_colours[38], c, sizeof(driver_colours[14]));
     memcpy(&driver_colours[38], pNewGpExeHelmetColours->driver39, PACKED_RECORD_MAX_REGIONS);

     c = pNewGpExeHelmetColours->driver40[10];
     memset(&driver_colours[39], c, sizeof(driver_colours[14]));
     memcpy(&driver_colours[39], pNewGpExeHelmetColours->driver40, PACKED_RECORD_MAX_REGIONS);

     fImageChanged = TRUE;
     RepaintWindow();
}

void
HelmetWindow::SaveExeColours(
     GP_EXE_HELMET_COLOURS    *pNewGpExeHelmetColours
) {
     memcpy(pNewGpExeHelmetColours->driver01, &driver_colours[ 0], sizeof(pNewGpExeHelmetColours->driver01));
     memcpy(pNewGpExeHelmetColours->driver02, &driver_colours[ 1], sizeof(pNewGpExeHelmetColours->driver02));
     memcpy(pNewGpExeHelmetColours->driver03, &driver_colours[ 2], sizeof(pNewGpExeHelmetColours->driver03));
     memcpy(pNewGpExeHelmetColours->driver04, &driver_colours[ 3], sizeof(pNewGpExeHelmetColours->driver04));
     memcpy(pNewGpExeHelmetColours->driver05, &driver_colours[ 4], sizeof(pNewGpExeHelmetColours->driver05));
     memcpy(pNewGpExeHelmetColours->driver06, &driver_colours[ 5], sizeof(pNewGpExeHelmetColours->driver06));
     memcpy(pNewGpExeHelmetColours->driver07, &driver_colours[ 6], sizeof(pNewGpExeHelmetColours->driver07));
     memcpy(pNewGpExeHelmetColours->driver08, &driver_colours[ 7], sizeof(pNewGpExeHelmetColours->driver08));
     memcpy(pNewGpExeHelmetColours->driver09, &driver_colours[ 8], sizeof(pNewGpExeHelmetColours->driver09));
     memcpy(pNewGpExeHelmetColours->driver10, &driver_colours[ 9], sizeof(pNewGpExeHelmetColours->driver10));
     memcpy(pNewGpExeHelmetColours->driver11, &driver_colours[10], sizeof(pNewGpExeHelmetColours->driver11));
     memcpy(pNewGpExeHelmetColours->driver12, &driver_colours[11], sizeof(pNewGpExeHelmetColours->driver12));
     /* Driver 13 (index 12) is a packed record */
     memcpy(pNewGpExeHelmetColours->driver14, &driver_colours[13], sizeof(pNewGpExeHelmetColours->driver14));
     /* Driver 15 (index 14) is a packed record */
     memcpy(pNewGpExeHelmetColours->driver16, &driver_colours[15], sizeof(pNewGpExeHelmetColours->driver16));
     memcpy(pNewGpExeHelmetColours->driver17, &driver_colours[16], sizeof(pNewGpExeHelmetColours->driver17));
     memcpy(pNewGpExeHelmetColours->driver18, &driver_colours[17], sizeof(pNewGpExeHelmetColours->driver18));
     memcpy(pNewGpExeHelmetColours->driver19, &driver_colours[18], sizeof(pNewGpExeHelmetColours->driver19));
     memcpy(pNewGpExeHelmetColours->driver20, &driver_colours[19], sizeof(pNewGpExeHelmetColours->driver20));
     memcpy(pNewGpExeHelmetColours->driver21, &driver_colours[20], sizeof(pNewGpExeHelmetColours->driver21));
     memcpy(pNewGpExeHelmetColours->driver22, &driver_colours[21], sizeof(pNewGpExeHelmetColours->driver22));
     memcpy(pNewGpExeHelmetColours->driver23, &driver_colours[22], sizeof(pNewGpExeHelmetColours->driver23));
     memcpy(pNewGpExeHelmetColours->driver24, &driver_colours[23], sizeof(pNewGpExeHelmetColours->driver24));
     memcpy(pNewGpExeHelmetColours->driver25, &driver_colours[24], sizeof(pNewGpExeHelmetColours->driver25));
     memcpy(pNewGpExeHelmetColours->driver26, &driver_colours[25], sizeof(pNewGpExeHelmetColours->driver26));
     memcpy(pNewGpExeHelmetColours->driver27, &driver_colours[26], sizeof(pNewGpExeHelmetColours->driver27));
     memcpy(pNewGpExeHelmetColours->driver28, &driver_colours[27], sizeof(pNewGpExeHelmetColours->driver28));
     memcpy(pNewGpExeHelmetColours->driver29, &driver_colours[28], sizeof(pNewGpExeHelmetColours->driver29));
     memcpy(pNewGpExeHelmetColours->driver30, &driver_colours[29], sizeof(pNewGpExeHelmetColours->driver30));
     memcpy(pNewGpExeHelmetColours->driver31, &driver_colours[30], sizeof(pNewGpExeHelmetColours->driver31));
     memcpy(pNewGpExeHelmetColours->driver32, &driver_colours[31], sizeof(pNewGpExeHelmetColours->driver32));
     memcpy(pNewGpExeHelmetColours->driver33, &driver_colours[32], sizeof(pNewGpExeHelmetColours->driver33));
     memcpy(pNewGpExeHelmetColours->driver34, &driver_colours[33], sizeof(pNewGpExeHelmetColours->driver34));
     memcpy(pNewGpExeHelmetColours->driver35, &driver_colours[34], sizeof(pNewGpExeHelmetColours->driver35));
     /* Driver 36 (index 35) is a packed record */
     /* Driver 37 (index 36) is a packed record */
     /* Driver 38 (index 37) is a packed record */
     /* Driver 39 (index 38) is a packed record */
     /* Driver 40 (index 39) is a packed record */

     /*
     ** Special handling for packed records.
     */
     pNewGpExeHelmetColours->driver13[10] = driver_colours[12].usRegionColour[PACKED_RECORD_MAX_REGIONS - 1];
     memcpy(pNewGpExeHelmetColours->driver13, &driver_colours[12], PACKED_RECORD_MAX_REGIONS);

     pNewGpExeHelmetColours->driver15[10] = driver_colours[14].usRegionColour[PACKED_RECORD_MAX_REGIONS - 1];
     memcpy(pNewGpExeHelmetColours->driver15, &driver_colours[14], PACKED_RECORD_MAX_REGIONS);

     pNewGpExeHelmetColours->driver36[10] = driver_colours[35].usRegionColour[PACKED_RECORD_MAX_REGIONS - 1];
     memcpy(pNewGpExeHelmetColours->driver36, &driver_colours[35], PACKED_RECORD_MAX_REGIONS);

     pNewGpExeHelmetColours->driver37[10] = driver_colours[36].usRegionColour[PACKED_RECORD_MAX_REGIONS - 1];
     memcpy(pNewGpExeHelmetColours->driver37, &driver_colours[36], PACKED_RECORD_MAX_REGIONS);

     pNewGpExeHelmetColours->driver38[10] = driver_colours[37].usRegionColour[PACKED_RECORD_MAX_REGIONS - 1];
     memcpy(pNewGpExeHelmetColours->driver38, &driver_colours[37], PACKED_RECORD_MAX_REGIONS);

     pNewGpExeHelmetColours->driver39[10] = driver_colours[38].usRegionColour[PACKED_RECORD_MAX_REGIONS - 1];
     memcpy(pNewGpExeHelmetColours->driver39, &driver_colours[38], PACKED_RECORD_MAX_REGIONS);

     pNewGpExeHelmetColours->driver40[10] = driver_colours[39].usRegionColour[PACKED_RECORD_MAX_REGIONS - 1];
     memcpy(pNewGpExeHelmetColours->driver40, &driver_colours[39], PACKED_RECORD_MAX_REGIONS);
}

BOOL
HelmetWindow::SelectDriver(
     unsigned short usNewDriverIndex
) {
     unsigned short i;
     BOOL           fSaved = fImageChanged;
     Cursor         cursor(IDC_WAIT);

     cursor.Select();
     {
          /*
          ** Helmets are stored relative to the car number index.
          */
          if (IsDriverEnabled(usNewDriverIndex)) {
               usCurrentDriverIndex = DriverIndex(usNewDriverIndex);
          }
          else {
               usCurrentDriverIndex = DriverDisabledIndex(usNewDriverIndex);
          }

          for (i = 0; i < NUM_ELEMENTS(helmet_regions); i++) {
               FillHelmetRegion(i, driver_colours[usCurrentDriverIndex].usRegionColour[i]);
          }
          fImageChanged = FALSE;
          RepaintWindow();
     }
     cursor.Deselect();

     return fSaved;
}

BOOL
HelmetWindow::IsLinkedHelmet(
     unsigned short index
) {
     unsigned short const packed_helmets[] = { 13, 15, 36, 37, 38, 39, 40 };
     unsigned short i;

     for (i = 0; i < NUM_ELEMENTS(packed_helmets); i++) {
          if (index + 1 == packed_helmets[i])  {
               return TRUE;
          }
     }

     return FALSE;
}

void
HelmetWindow::DoDrag(
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

/*---------------------------------------------------------------------------
** End of File
*/

