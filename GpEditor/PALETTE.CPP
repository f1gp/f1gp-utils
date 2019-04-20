/*
** File   : palette.cpp
** Author : TK 
** Date   : 20/11/93
**
** $Header:   L:/GPEDITOR/VCS/PALETTE.CP_   1.3   01 Oct 1994 15:27:56   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Palette Window.
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
#include "palette.hpp"

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

PaletteWindow::PaletteWindow(
     HINSTANCE      hInst,
     HWND           hWndParent,
     unsigned short usWidth,
     unsigned short usHeight
) : ChildWindow(hInst, PALETTE_CLASS_NAME, GetAppName(), WS_CHILD, CHILD_ID_PALETTE, hWndParent) {
     pClientBitmap = new Bitmap(pF1CarBitmap, usWidth, usHeight);
     ASSERT(pClientBitmap != NULL);

     pCursorPalette  = new Cursor(Instance(), APP_CURSOR_PALETTE);
     ASSERT(pCursorPalette != NULL);

     /*
     ** Build palette image once only in memory.
     */
     PaintWindowUpdate        pwu(Handle());
     PaintCompatibleWindow    pcw(pwu.DC());
     HPALETTE                 hOldPalette;
     unsigned short           i, x, y, w;
     RECT                     rect;

     hOldPalette = SelectPalette(pcw.DC(), hPalette, FALSE);
     (void) RealizePalette(pcw.DC());

     ASSERT(pClientBitmap != NULL);
     pcw.SelectBitmap(pClientBitmap);
     {
          /*
          ** Draw palette in client bitmap.
          */
          w = usWidth / PALETTE_BOX_ITEMS_PER_LINE;
          for (i = 0, x = 0, y = 0; i < NUM_COLOURS_IN_PALETTE; i++, x += w) {
               SolidBrush     brush(PALETTEINDEX(i));

               if (i % PALETTE_BOX_ITEMS_PER_LINE == 0 && i != 0) {
                    x = 0;
                    y += PALETTE_BOX_ITEM_HEIGHT;
               }

               rect.bottom    = y;
               rect.top       = y + PALETTE_BOX_ITEM_HEIGHT;
               rect.left      = x;
               rect.right     = x + w;
               pcw.FillRect(&rect, &brush);
               pcw.MoveTo(rect.left,  rect.bottom);
               pcw.LineTo(rect.left,  rect.top);
               pcw.LineTo(rect.right, rect.top);
               pcw.LineTo(rect.right, rect.bottom);
               pcw.LineTo(rect.left,  rect.bottom);
          }
     }
     pcw.DeselectBitmap();
     (void) SelectPalette(pcw.DC(), hOldPalette, FALSE);
}

PaletteWindow::~PaletteWindow(
     void
) {
     (void) SetCursor(LoadCursor(NULL, IDC_ARROW));
     delete pCursorPalette;

     if (pClientBitmap) {
          delete pClientBitmap;
          pClientBitmap = NULL;
     }
}

BOOL
PaletteWindow::UnknownSysEventHandler(
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
          (void) SetCursor((HICON) pCursorPalette->Handle());
          *r_val_p = TRUE;               
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
PaletteWindow::Paint(
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
               (void) BitBlt( pwu.DC(),
                              0, 0,
                              GetClientWidth(), GetClientHeight(),
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
PaletteWindow::Mouse(
     EventMouse *event
) {
     ASSERT(event != NULL);
     ASSERT(pF1CarBitmap != NULL);
     ASSERT(pClientBitmap != NULL);

     if (event->LeftButtonDown()) {
          ASSERT(event->X() <= pClientBitmap->Width());
          ASSERT(event->Y() <= pClientBitmap->Height());

          int  x = event->X();
          int  y = event->Y();
          int  index;

          x /= pClientBitmap->Width() / PALETTE_BOX_ITEMS_PER_LINE;
          y /= PALETTE_BOX_ITEM_HEIGHT;

          index = (y * PALETTE_BOX_ITEMS_PER_LINE) + x;
          ASSERT(index >= 0 && index <= 255);
          if (index >= 0 && index <= 255) {
               (void) SendMessage(Parent(), IDE_DESIGN_NEW_PALETTE_COLOUR, index, 0L);
               (void) SendMessage(Parent(), IDE_HELMET_NEW_PALETTE_COLOUR, index, 0L);
          }
     }
}

/*---------------------------------------------------------------------------
** End of File
*/

