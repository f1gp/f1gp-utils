/*
** File   : toolbar.cpp
** Author : TK 
** Date   : 13/02/94
**
** Tool Bar
*/

/*---------------------------------------------------------------------------
** Includes
*/

#include <windows.h>
#include <memory.h>
#include "toolbar.hpp"
#include "version.ipp"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TOOLBAR_CLASS_NAME    "CppWinTB"

#define DEFAULTBITMAPX        16
#define DEFAULTBITMAPY        15
#define DEFAULTBUTTONX        24
#define DEFAULTBUTTONY        22
#define DEFAULTBARHEIGHT      27

#define BUTTONSEPERATOR       4
#define FIRSTBUTTON           4

#define GETWINDOWID(hwnd)		          GetWindowWord(hwnd, GWW_ID)

#define CMB_DISCARDABLE 0x01    // create bitmap as discardable
#define CMB_MASKED      0x02    // create image/mask pair in bitmap


#define TB_ENABLEBUTTON	(WM_USER + 1)
        // wParam: UINT, button ID
        // lParam: BOOL LOWORD, enable if nonzero; HIWORD not used, 0
        // return: not used
        //

#define TB_CHECKBUTTON	(WM_USER + 2)
        // wParam: UINT, button ID
        // lParam: BOOL LOWORD, check if nonzero; HIWORD not used, 0
        // return: not used
        //

#define TB_PRESSBUTTON	(WM_USER + 3)
        // wParam: UINT, button ID
        // lParam: BOOL LOWORD, press if nonzero; HIWORD not used, 0
        // return: not used
        //

#define TB_HIDEBUTTON	(WM_USER + 4)
        // wParam: UINT, button ID
        // lParam: BOOL LOWORD, hide if nonzero; HIWORD not used, 0
        // return: not used
        //
#define TB_INDETERMINATE	(WM_USER + 5)
        // wParam: UINT, button ID
        // lParam: BOOL LOWORD, make indeterminate if nonzero; HIWORD not used, 0
        // return: not used
        //

#define TB_ISBUTTONENABLED	(WM_USER + 9)
        // wParam: UINT, button ID
        // lParam: not used, 0
        // return: BOOL LOWORD, enabled if nonzero; HIWORD not used
        //

#define TB_ISBUTTONCHECKED	(WM_USER + 10)	
        // wParam: UINT, button ID
        // lParam: not used, 0
        // return: BOOL LOWORD, checked if nonzero; HIWORD not used
        //

#define TB_ISBUTTONPRESSED	(WM_USER + 11)	
        // wParam: UINT, button ID
        // lParam: not used, 0
        // return: BOOL LOWORD, pressed if nonzero; HIWORD not used
        //

#define TB_ISBUTTONHIDDEN	(WM_USER + 12)	
        // wParam: UINT, button ID
        // lParam: not used, 0
        // return: BOOL LOWORD, hidden if nonzero; HIWORD not used
        //

#define TB_ISBUTTONINDETERMINATE	(WM_USER + 13)	
        // wParam: UINT, button ID
        // lParam: not used, 0
        // return: BOOL LOWORD, indeterminate if nonzero; HIWORD not used
        //

#define TB_SETSTATE             (WM_USER + 17)
        // wParam: UINT, button ID
        // lParam: UINT LOWORD, state bits; HIWORD not used, 0
        // return: not used
        //

#define TB_GETSTATE             (WM_USER + 18)
        // wParam: UINT, button ID
        // lParam: not used, 0
        // return: UINT LOWORD, state bits; HIWORD not used
        //

#define TB_ADDBITMAP		(WM_USER + 19)
        // wParam: UINT, number of button graphics in bitmap
        // lParam: one of:
        //         HINSTANCE LOWORD, module handle; UINT HIWORD, resource id
        //         HINSTANCE LOWORD, NULL; HBITMAP HIWORD, bitmap handle
        // return: one of:
        //         int LOWORD, index for first new button; HIWORD not used
        //         int LOWORD, -1 indicating error; HIWORD not used
        //

#define TB_ADDBUTTONS		(WM_USER + 20)
        // wParam: UINT, number of buttons to add
        // lParam: LPTBBUTTON, pointer to array of TBBUTTON structures
        // return: not used
        //

#define TB_INSERTBUTTON		(WM_USER + 21)
        // wParam: UINT, index for insertion (appended if index doesn't exist)
        // lParam: LPTBBUTTON, pointer to one TBBUTTON structure
        // return: not used
        //

#define TB_DELETEBUTTON		(WM_USER + 22)
        // wParam: UINT, index of button to delete
        // lParam: not used, 0
        // return: not used
        //

#define TB_GETBUTTON		(WM_USER + 23)
        // wParam: UINT, index of button to get
        // lParam: LPTBBUTTON, pointer to TBBUTTON buffer to receive button
        // return: not used
        //

#define TB_BUTTONCOUNT		(WM_USER + 24)
        // wParam: not used, 0
        // lParam: not used, 0
        // return: UINT LOWORD, number of buttons; HIWORD not used
        //

#define TB_COMMANDTOINDEX	(WM_USER + 25)
        // wParam: UINT, command id
        // lParam: not used, 0
        // return: UINT LOWORD, index of button (-1 if command not found);
        //         HIWORD not used
        //


#define TBN_BEGINDRAG	0x0201
#define TBN_ENDDRAG	     0x0203

#define SETWINDOWPOINTER(hwnd, name, p)	     SetWindowWord(hwnd, 0, (WORD)p)
#define GETWINDOWPOINTER(hwnd, name)	     ((name)GetWindowWord(hwnd, 0))
#define ALLOCWINDOWPOINTER(name, size)	     ((name)LocalAlloc(LPTR, size))
#define FREEWINDOWPOINTER(p)		          LocalFree((HLOCAL)p)

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef struct tagTBBMINFO {		/* info for recreating the bitmaps */
    int        nButtons;
    HINSTANCE  hInst;
    WORD       wID;
    HBITMAP    hbm;
} TBBMINFO, NEAR *PTBBMINFO;

typedef struct tagTBHDR {             /* instance data for toolbar window */
    int        nSysColorChanges;
    PTBBUTTON  pCaptureButton;
    HWND       hdlgCust;
    HWND       hwndCommand;
    int        nBitmaps;
    PTBBMINFO  pBitmaps;
    int        iNumButtons;
    TBBUTTON   Buttons[1];
} TBHDR, NEAR *PTBHDR;

/*---------------------------------------------------------------------------
** Local function prototypes
*/

LRESULT CALLBACK ToolbarWndProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);

static HWND WINAPI CreateToolbar(HWND hwnd, DWORD ws, WORD wID, int nBitmaps, HINSTANCE hBMInst,
                    WORD wBMID, LPCTBBUTTON lpButtons, int iNumButtons, unsigned short usWidth, unsigned short usHeight);
static int     ToolBarRegister(HINSTANCE hInstance);
static void    GetSysColors(PTBHDR pTBHdr);
static BOOL    CreateDitherBrush(BOOL bIgnoreCount);
static BOOL    FreeDitherBrush(void);
static HBITMAP WINAPI CreateMappedBitmap(HINSTANCE hInstance, int idBitmap );
static HBITMAP SelectBM(HDC hDC, PTBHDR pTBState, int nButton);
static void    DrawButton(HDC hdc, int x, int y, int dx, int dy,
                    PTBHDR pTBState, PTBBUTTON ptButton);
static int     TBHitTest(PTBHDR pTBState, int xPos, int yPos);
static int     PositionFromID(PTBHDR pTBState, int id);
static void    BuildButtonTemplates(void);
static void    TBInputStruct(PTBHDR pTBState, LPTBBUTTON pButtonInt, LPTBBUTTON pButtonExt);
static void    TBOutputStruct(PTBHDR pTBState, LPTBBUTTON pButtonInt, LPTBBUTTON pButtonExt);

/*---------------------------------------------------------------------------
** Data
*/

static HBRUSH hbrDither;
static DWORD  rgbFace;
static DWORD  rgbShadow;
static DWORD  rgbHilight;
static DWORD  rgbFrame;

static int nSelectedBM   = -1;
static HDC hdcGlyphs     = NULL;           // globals for fast drawing

/*
** hdcMono is the DC that holds a mono bitmap, hbmMono that is used to
** create highlights of button faces.
** 
** hbmDefault hold the default bitmap if there is one.
*/
static HDC     hdcMono        = NULL;
static HBITMAP hbmMono        = NULL;
static HBITMAP hbmDefault     = NULL;

static WORD wStateMasks[] = {
    TBSTATE_ENABLED,
    TBSTATE_CHECKED,
    TBSTATE_PRESSED,
    TBSTATE_HIDDEN,
    TBSTATE_INDETERMINATE
};

static int iDitherCount = 0;
static int iInitCount   = 0;

static unsigned short    usWindowWidth;
static unsigned short    usWindowHeight;

static HINSTANCE         hInstanceGlobal;

/*---------------------------------------------------------------------------
** Functions
*/

_MFN
ToolBar::ToolBar(
     HINSTANCE      hInst,              /* In  Instance handle             */
     HWND           hWnd,               /* In  Parent's handle             */
     DWORD          ws,                 /* In  Window styles               */
     WORD           wID,                /* In  Child window ID             */
     int            nBitmaps,           /* In  Number of bitmaps in image  */
     WORD           wBMID,              /* In  Resource ID for bitmap      */
     LPCTBBUTTON    lpButtons,          /* In  Buttons definition          */
     int            iNumButtons,        /* In  Number of buttons           */
     unsigned short usWidth,            /* In  Window width                */
     unsigned short usHeight            /* In  Window height               */
) {
     hInstance           = hInst;
     hInstanceGlobal     = Instance();
     ToolBarRegister(Instance());

     hWndToolBar = CreateToolbar(  hWnd, ws, wID, nBitmaps, hInst,
                                   wBMID, lpButtons, iNumButtons,
                                   usWidth, usHeight
                              );
}

_MFN
ToolBar::~ToolBar(
     void
) {
     ;
}

static int
ToolBarRegister(
     HINSTANCE hInstance
) {
     WNDCLASS wc;

     if (!GetClassInfo(hInstance, TOOLBAR_CLASS_NAME, &wc)) {
          wc.lpszClassName    = TOOLBAR_CLASS_NAME;
          wc.style	          = CS_DBLCLKS;
          wc.lpfnWndProc	     = (WNDPROC) ToolbarWndProc;
          wc.cbClsExtra	     = 0;
          wc.cbWndExtra       = sizeof(PTBHDR);
          wc.hInstance        = hInstance;
          wc.hIcon	          = NULL;
          wc.hCursor	     = LoadCursor(NULL, IDC_ARROW);
          wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1);
          wc.lpszMenuName	= NULL;

          return RegisterClass(&wc);
     }

     return 0;
}

static BOOL
InitGlobalObjects(
     void
) {
     hdcGlyphs = CreateCompatibleDC(NULL);
     if (!hdcGlyphs) {
          return FALSE;
     }

     hdcMono = CreateCompatibleDC(NULL);
     if (!hdcMono) {
          return FALSE;
     }

     hbmMono = CreateBitmap(DEFAULTBUTTONX, DEFAULTBUTTONY, 1, 1, NULL);
     if (!hbmMono) {
          return FALSE;
     }

     hbmDefault = (HBITMAP) SelectObject(hdcMono, hbmMono);
     iInitCount++;

     return TRUE;
}

static void
FreeGlobalObjects(
     void
) {
     --iInitCount;

     if (iInitCount) {
          return;
     }

     if (hdcMono) {
	     if (hbmDefault) {
	          SelectObject(hdcMono, hbmDefault);
          }
	     DeleteDC(hdcMono);
          hdcMono = NULL;
     }

     if (hdcGlyphs) {
	     DeleteDC(hdcGlyphs);
          hdcGlyphs = NULL;
     }

     if (hbmMono) {
	     DeleteObject(hbmMono);
          hbmMono = NULL;
     }
}

static HWND WINAPI
CreateToolbar(
     HWND           hwnd,
     DWORD          ws,
     WORD           wID,
     int            nBitmaps,
     HINSTANCE      hBMInst,
     WORD           wBMID,
     LPCTBBUTTON    lpButtons,
     int            iNumButtons,
     unsigned short usWidth,            /* In  Window width                */
     unsigned short usHeight            /* In  Window height               */
) {
    HWND hwndToolbar;

     usWindowWidth = usWidth;
     if (usHeight != 0) {
          usWindowHeight = usHeight;
     }
     else {
          usWindowHeight = DEFAULTBARHEIGHT;
     }

    hwndToolbar = CreateWindowEx(  0L,
                                   TOOLBAR_CLASS_NAME,
                                   NULL,
                                   WS_CHILD | ws,
                                   0, 0, (usWindowWidth != 0) ? usWindowWidth : 10, 10,
                                   hwnd,
                                   (HMENU) wID,
                                   hInstanceGlobal,
                                   NULL
                              );
     if (hwndToolbar) {
          SendMessage(hwndToolbar, TB_ADDBITMAP, nBitmaps, MAKELONG(hBMInst, wBMID));
          SendMessage(hwndToolbar, TB_ADDBUTTONS, iNumButtons, (LPARAM)lpButtons);
     }

     return hwndToolbar;
}

static void
PatB(
     HDC       hdc,
     int       x,
     int       y,
     int       dx,
     int       dy,
     DWORD     rgb
) {
     RECT    rc;

     rc.left   = x;
     rc.top    = y;
     rc.right  = x + dx;
     rc.bottom = y + dy;

     SetBkColor(hdc,rgb);
     ExtTextOut(hdc,0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
}

/*---------------------------------------------------------------------------
** Purpose:    Create a mono bitmap mask.
**
** Returns:    Nothing.
**
** Notes  :    1's where color == COLOR_BTNFACE || COLOR_HILIGHT
**             0's everywhere else
*/
static void
CreateMask(
     PTBHDR    pTBHdr,
     PTBBUTTON pTBButton,
     int       xoffset,
     int       yoffset,
     int       dx,
     int       dy
) {
     /*
     ** Initalize whole area with 1's
     */
     PatBlt(hdcMono, 0, 0, dx, dy, WHITENESS);

     /*
     ** Create mask based on color bitmap, convert this to 1's
     */
     SetBkColor(hdcGlyphs, rgbFace);
     BitBlt(   hdcMono, xoffset, yoffset, DEFAULTBITMAPX, DEFAULTBITMAPY,
               hdcGlyphs, pTBButton->iBitmap * DEFAULTBITMAPX, 0, SRCCOPY
          );

     /*
     ** Convert this to 1's 
     */
     SetBkColor(hdcGlyphs, rgbHilight);

     /*
     ** OR in the new 1's 
     */
     BitBlt(   hdcMono, xoffset, yoffset, DEFAULTBITMAPX, DEFAULTBITMAPY,
               hdcGlyphs, pTBButton->iBitmap * DEFAULTBITMAPX, 0, SRCPAINT
          );
}

/*---------------------------------------------------------------------------
** Purpose:    Given a button number, the corresponding bitmap is loaded
**             and selected in, and the Window origin set.
**
** Returns:    Returns NULL on Error, 1 if the necessary bitmap is already
**             selected, or the old bitmap otherwise.
**
** Notes  :    
*/
static HBITMAP
SelectBM(
     HDC       hDC,
     PTBHDR    pTBHdr,
     int       nButton
) {
     PTBBMINFO pTemp;
     HBITMAP   hRet;
     int       nBitmap, nTot;

     for (pTemp = pTBHdr->pBitmaps, nBitmap = 0, nTot = 0; ; ++pTemp, ++nBitmap) {
          if (nBitmap >= pTBHdr->nBitmaps) {
	          return(NULL);
          }

          if (nButton < nTot+pTemp->nButtons) {
	          break;
          }
          nTot += pTemp->nButtons;
     }

     /*
     ** Special case when the required bitmap is already selected
     */
     if (nBitmap == nSelectedBM) {
          return((HBITMAP) 1);
     }

     if (!pTemp->hbm || (hRet = (HBITMAP) SelectObject(hDC, pTemp->hbm)) == NULL) {
          if (pTemp->hbm) {
	          DeleteObject(pTemp->hbm);
          }

          if (pTemp->hInst) {
               pTemp->hbm = CreateMappedBitmap(pTemp->hInst, pTemp->wID);
          }
          else {
     	     pTemp->hbm = (HBITMAP) pTemp->wID;
          }

          if (!pTemp->hbm || (hRet = (HBITMAP) SelectObject(hDC, pTemp->hbm)) == NULL) {
	          return(NULL);
          }
     }

     nSelectedBM = nBitmap;
     SetWindowOrg(hDC, nTot * DEFAULTBITMAPX, 0);

     return(hRet);
}

static void 
DrawBlankButton(
     HDC  hdc,
     int  x,
     int  y,
     int  dx,
     int  dy,
     WORD state
) {
     /*
     ** Face color
     */
     PatB(hdc, x, y, dx, dy, rgbFace);

     if (state & TBSTATE_PRESSED) {
          PatB(hdc, x + 1, y, dx - 2, 1, rgbFrame);
          PatB(hdc, x + 1, y + dy - 1, dx - 2, 1, rgbFrame);
          PatB(hdc, x, y + 1, 1, dy - 2, rgbFrame);
          PatB(hdc, x + dx - 1, y +1, 1, dy - 2, rgbFrame);
          PatB(hdc, x + 1, y + 1, 1, dy-2, rgbShadow);
          PatB(hdc, x + 1, y + 1, dx-2, 1, rgbShadow);
     }
     else {
          PatB(hdc, x + 1, y, dx - 2, 1, rgbFrame);
          PatB(hdc, x + 1, y + dy - 1, dx - 2, 1, rgbFrame);
          PatB(hdc, x, y + 1, 1, dy - 2, rgbFrame);
          PatB(hdc, x + dx - 1, y + 1, 1, dy - 2, rgbFrame);
          dx -= 2;
          dy -= 2;
          PatB(hdc, x + 1, y + 1, 1, dy - 1, rgbHilight);
          PatB(hdc, x + 1, y + 1, dx - 1, 1, rgbHilight);
          PatB(hdc, x + dx, y + 1, 1, dy, rgbShadow);
          PatB(hdc, x + 1, y + dy, dx, 1, rgbShadow);
          PatB(hdc, x + dx - 1, y + 2, 1, dy - 2, rgbShadow);
          PatB(hdc, x + 2, y + dy - 1, dx - 2, 1, rgbShadow);
     }
}

static void 
DrawButton(
     HDC       hdc,
     int       x,
     int       y,
     int       dx,
     int       dy,
     PTBHDR    pTBHdr,
     PTBBUTTON ptButton
) {
     int       yOffset;
     HBRUSH    hbrOld, hbr;
     BOOL      bMaskCreated = FALSE;
     BYTE      state;
     int       xButton = 0;		// assume button is down
     int       dxFace, dyFace;
     int       xCenterOffset;

     dxFace = dx;
     dyFace = dy;

     /*
     ** Make local copy of state and do proper overriding 
     */
     state = ptButton->fsState;
     if (state & TBSTATE_INDETERMINATE) {
	     if (state & TBSTATE_PRESSED) {
	          state &= ~TBSTATE_INDETERMINATE;
          }
	     else if (state & TBSTATE_ENABLED) {
	          state = TBSTATE_INDETERMINATE; /*<TK >*//* Bug? |= ? */
          }
	     else {
	          state &= ~TBSTATE_INDETERMINATE;
          }
     }

     /*
     ** Get the proper button look - up or down. 
     */
     if (!(state & (TBSTATE_PRESSED | TBSTATE_CHECKED))) {
	     xButton = dx;	// use 'up' version of button
	     dxFace -= 2;	
	     dyFace -= 2;	// extents to ignore button highlight
     }

     DrawBlankButton(hdc, x, y, dx, dy, state);

     /*
     ** Move coordinates inside border and away from upper left highlight.
     ** The extents change accordingly.
     */
     x += 2;
     y += 2;
     dxFace -= 3;		
     dyFace -= 3;

     if (!SelectBM(hdcGlyphs, pTBHdr, ptButton->iBitmap)) {
     	return;
     }

     /*
     ** Calculate offset of face from (x,y). y is always from the top,
     ** so the offset is easy. x needs to be centered in face.
     */
     yOffset = 1;
     xCenterOffset = (dxFace - DEFAULTBITMAPX)/2;
     if (state & (TBSTATE_PRESSED | TBSTATE_CHECKED)) {
          /*
          ** Pressed state moves down and to the right
          ** (x moves automatically as face size grows)
          */
          yOffset++;
     }

     /*
     ** Now put on the face 
     */
     if (state & TBSTATE_ENABLED) {
          /*
          ** Regular version
          */
          BitBlt(   hdc, x+xCenterOffset, y + yOffset, DEFAULTBITMAPX, DEFAULTBITMAPY,
                    hdcGlyphs, ptButton->iBitmap * DEFAULTBITMAPX, 0, SRCCOPY
               );
     }
     else {
          /*
          ** Disabled version (or indeterminate)
          */
	     bMaskCreated = TRUE;
          CreateMask(pTBHdr, ptButton, xCenterOffset, yOffset, dxFace, dyFace);

	     SetTextColor(hdc, 0L);	          // 0's in mono -> 0 (for ROP)
	     SetBkColor(hdc, 0x00FFFFFF);       // 1's in mono -> 1

          /*
          ** Draw glyph's white understrike
          */
	     if (!(state & TBSTATE_INDETERMINATE)) {
	          hbr = CreateSolidBrush(rgbHilight);
	          if (hbr) {
	               hbrOld = (HBRUSH) SelectObject(hdc, hbr);
	               if (hbrOld) {
                         /*
                         ** Draw hilight color where we have 0's in the mask
                         */
                         BitBlt(hdc, x + 1, y + 1, dxFace, dyFace, hdcMono, 0, 0, 0x00B8074A);
	                    SelectObject(hdc, hbrOld);
	               }
	               DeleteObject(hbr);
	          }
	     }

          /*
          ** Gray out glyph
          */
	     hbr = CreateSolidBrush(rgbShadow);
	     if (hbr) {
	          hbrOld = (HBRUSH) SelectObject(hdc, hbr);
	          if (hbrOld) {
                    /*
                    ** Draw the shadow color where we have 0's in the mask
                    */
                    BitBlt(hdc, x, y, dxFace, dyFace, hdcMono, 0, 0, 0x00B8074A);
	               SelectObject(hdc, hbrOld);
	          }
	          DeleteObject(hbr);
	     }

	     if (state & TBSTATE_CHECKED) {
	          BitBlt(hdcMono, 1, 1, dxFace - 1, dyFace - 1, hdcMono, 0, 0, SRCAND);
	     }
     }

     if (state & (TBSTATE_CHECKED | TBSTATE_INDETERMINATE)) {
          hbrOld = (HBRUSH) SelectObject(hdc, hbrDither);
	     if (hbrOld) {
     	     if (!bMaskCreated) {
                    CreateMask(pTBHdr, ptButton, xCenterOffset, yOffset, dxFace, dyFace);
               }

	          SetTextColor(hdc, 0L);		// 0 -> 0
	          SetBkColor(hdc, 0x00FFFFFF);	// 1 -> 1

               /*
               ** Only draw the dither brush where the mask is 1's
               */
               BitBlt(hdc, x, y, dxFace, dyFace, hdcMono, 0, 0, 0x00E20746);

	          SelectObject(hdc, hbrOld);
	     }
     }
}

static void
ToolbarPaint(
     HWND      hWnd,
     PTBHDR    pTBHdr
) {
     RECT           rc;
     HDC            hdc;
     PAINTSTRUCT    ps;
     int            iButton, xButton, yButton;
     int            cButtons       = pTBHdr->iNumButtons;
     PTBBUTTON      pAllButtons    = pTBHdr->Buttons;
     HBITMAP        hbmOldGlyphs;
     int            iCacheWidth = 0;
     int            dx, dy;

     GetSysColors(pTBHdr);

     hdc = BeginPaint(hWnd, &ps);

     GetClientRect(hWnd, &rc);
     if (!rc.right) {
	     goto Error1;
     }

     if (usWindowWidth != 0) {
          rc.right = usWindowWidth;
     }

     dx = DEFAULTBUTTONX;
     dy = DEFAULTBUTTONY;

     /*
     ** Setup global stuff for fast painting 
     */
     nSelectedBM = -1;
     hbmOldGlyphs = SelectBM(hdcGlyphs, pTBHdr, 0);
     if (!hbmOldGlyphs) {
	     goto Error1;
     }

     yButton   = ((rc.bottom - rc.top) - dy) / 2;
     rc.top    = yButton;
     rc.bottom = yButton + dy;

     for (iButton = 0, xButton = FIRSTBUTTON; iButton < cButtons; iButton++) {
          PTBBUTTON ptbButton = &pAllButtons[iButton];

	     if (ptbButton->fsState & TBSTATE_HIDDEN) {
	          ;
          }
          else if (ptbButton->fsStyle & TBSTYLE_SEP) {
	          xButton += ptbButton->iBitmap;
          }
          else {
               rc.left = xButton;
               rc.right = xButton + dx;

	          if (RectVisible(hdc, &rc)) {
                    DrawButton(hdc, xButton, yButton, dx, dy, pTBHdr, ptbButton);
	          }
	          xButton += dx - 1;
          }
     }

     SelectObject(hdcGlyphs, hbmOldGlyphs);

Error1:
     EndPaint(hWnd, &ps);
}

static BOOL
GetItemRect(
     PTBHDR    pTBHdr,
     UINT      uButton,
     LPRECT    lpRect
) {
     UINT      iButton, xPos;
     PTBBUTTON pButton;

     if (uButton>=(UINT)pTBHdr->iNumButtons || (pTBHdr->Buttons[uButton].fsState&TBSTATE_HIDDEN)) {
          return(FALSE);
	}

     xPos = FIRSTBUTTON;

     for (iButton=0, pButton=pTBHdr->Buttons; iButton<uButton; ++iButton, ++pButton) {
          if (pButton->fsState & TBSTATE_HIDDEN) {
	          ;
          }
		else if (pButton->fsStyle & TBSTYLE_SEP) {
			xPos += pButton->iBitmap;
		}
		else {
               xPos += DEFAULTBUTTONX - 1;
		}
	}

	/* pButton should now point at the required button, and xPos should be
	** its left edge. Note that we already checked if the button was
	** hidden above.
	*/
	lpRect->left   = xPos;
	lpRect->right  = xPos + (pButton->fsStyle&TBSTYLE_SEP
                         ? pButton->iBitmap : DEFAULTBUTTONX);
     lpRect->top    = (usWindowHeight - DEFAULTBUTTONY) / 2;
     lpRect->bottom = lpRect->top + DEFAULTBUTTONY;

	return(TRUE);
}

static void
InvalidateButton(
     HWND      hwnd,
     PTBHDR    pTBHdr,
     PTBBUTTON pButtonToPaint
) {
	RECT rc;

     if (GetItemRect(pTBHdr, pButtonToPaint-pTBHdr->Buttons, &rc)) {
          InvalidateRect(hwnd, &rc, FALSE);
	}
}

static int
TBHitTest(
     PTBHDR    pTBHdr,
     int       xPos,
     int       yPos
) {
     int       iButton;
     int       cButtons = pTBHdr->iNumButtons;
     PTBBUTTON pButton;

     /*
     ** What we do is adjust the x and y to account for
     **    the x-offset to the first button. If x is less
     **    then this value then no button is pushed.
     */
     xPos -= FIRSTBUTTON;
     if (xPos < 0) {
          return(-1);
     }

     /*
     ** Account for the space above and below the buttons.
     */
     yPos -= ((usWindowHeight - DEFAULTBUTTONY) / 2);
     if (yPos < 0 || yPos >= DEFAULTBUTTONY) {
           return(-1);
     }

     /*
     ** xPos and yPos are now adjusted so that the first button
     ** would be at 0,0 on the toolbar. Next we are going to walk the
     ** butons and for each one will adjust xPos and yPos so that
     ** the next button is at 0,0. If xPos or yPos are less then 0
     ** no button is hit. If for any button xPos and yPos are within
     ** pTBHdr->nButWidth and pTBHdr->nButHeight then the given button
     ** is hit.
     */
     for (iButton=0, pButton=pTBHdr->Buttons; iButton < cButtons; ++iButton, ++pButton) {
          /*
          ** Ignore hidden buttons
          */
          if (pButton->fsState & TBSTATE_HIDDEN) {
               ;
          }
          else if (pButton->fsStyle & TBSTYLE_SEP) {
               /*
               ** If this is a seperator adjust by seperater width
               */
	          xPos -= pButton->iBitmap;
          }
          else {
               /*
               ** Ok this is a regular button adjust as such
               */
               xPos -= DEFAULTBUTTONX - 1;
          }

          if (xPos < 0) {
               if (pButton->fsStyle&TBSTYLE_SEP) {
	               break;
               }

	          return(iButton);
	     }
     }

     return(-1 - iButton);
}

static int
PositionFromID(
     PTBHDR    pTBHdr,
     int       id
) {
     int       i;
     int       cButtons    = pTBHdr->iNumButtons;
     PTBBUTTON pAllButtons = pTBHdr->Buttons;

     for (i = 0; i < cButtons; i++) {
          if (pAllButtons[i].idCommand == id) {
	          return i;		// position found
          }
     }

     return -1;		// ID not found!
}

// check a radio button by button index.
// the button matching idCommand was just pressed down.  this forces
// up all other buttons in the group.
// this does not work with buttons that are forced up with

static void
MakeGroupConsistant(
     HWND      hWnd,
     PTBHDR    pTBHdr,
     int       idCommand
) {
     int       i, iFirst, iLast, iButton;
     int       cButtons    = pTBHdr->iNumButtons;
     PTBBUTTON pAllButtons = pTBHdr->Buttons;

     iButton = PositionFromID(pTBHdr, idCommand);

     if (iButton < 0) {
          return;
     }

     /*
     ** Did the pressed button just go down? 
     */
     if (!(pAllButtons[iButton].fsState & TBSTATE_CHECKED)) {
          return;         // no, can't do anything
     }

     /*
     ** Find the limits of this radio group 
     */
     for (iFirst = iButton; (iFirst > 0) && (pAllButtons[iFirst].fsStyle & TBSTYLE_GROUP); iFirst--) {
          if (!(pAllButtons[iFirst].fsStyle & TBSTYLE_GROUP)) {
               iFirst++;
          }
     }

     cButtons--;
     for (iLast = iButton; (iLast < cButtons) && (pAllButtons[iLast].fsStyle & TBSTYLE_GROUP); iLast++) {
          ;
     }

     if (!(pAllButtons[iLast].fsStyle & TBSTYLE_GROUP)) {
          iLast--;
     }

     /*
     ** Search for the currently down button and pop it up 
     */
     for (i = iFirst; i <= iLast; i++) {
          if (i != iButton) {
               /*
               ** Is this button down?
               */
               if (pAllButtons[i].fsState & TBSTATE_CHECKED) {
	               pAllButtons[i].fsState &= ~TBSTATE_CHECKED;     // pop it up
                    InvalidateButton(hWnd, pTBHdr, &pAllButtons[i]);
                    break;          // only one button is down right?
               }
          }
     }
}

/*
** Adds a new bitmap to the list of BMs available for this toolbar.
** Returns the index of the first button in the bitmap or -1 if there
** was an error.
*/
static int
AddBitmap(
     PTBHDR    pTBHdr,
     int       nButtons,
     HINSTANCE hBMInst,
     WORD      wBMID
) {
     PTBBMINFO pTemp;
     int       nBM, nIndex;

     if (pTBHdr->pBitmaps) {
          /*
          ** Check if the bitmap has already been added
          */
          for (nBM=pTBHdr->nBitmaps, pTemp=pTBHdr->pBitmaps, nIndex=0; nBM>0; nBM--, ++pTemp) {
	          if (pTemp->hInst==hBMInst && pTemp->wID==wBMID) {
                    /*
                    ** We already have this bitmap, but have we "registered" all
	               ** the buttons in it?
	               */
	               if (pTemp->nButtons >= nButtons) {
		               return(nIndex);
                    }
	               if (nBM == 1) {
		               /*
                         ** If this is the last bitmap, we can easily increase the
		               ** number of buttons without messing anything up.
		               */
		               pTemp->nButtons = nButtons;
		               return(nIndex);
		          }
	          }

	          nIndex += pTemp->nButtons;
	     }

          pTemp = (PTBBMINFO) LocalReAlloc((HANDLE)pTBHdr->pBitmaps,
                    (pTBHdr->nBitmaps+1)*sizeof(TBBMINFO), LMEM_MOVEABLE);
          if (!pTemp) {
     	     return(-1);
          }
          pTBHdr->pBitmaps = pTemp;
     }
     else {
          pTBHdr->pBitmaps = (PTBBMINFO)LocalAlloc(LPTR, sizeof(TBBMINFO));
          if (!pTBHdr->pBitmaps) {
	          return(-1);
          }
     }

     pTemp = pTBHdr->pBitmaps + pTBHdr->nBitmaps;

     pTemp->hInst = hBMInst;
     pTemp->wID = wBMID;
     pTemp->nButtons = nButtons;
     pTemp->hbm = NULL;

     ++pTBHdr->nBitmaps;

     for (nButtons=0, pTemp--; pTemp>=pTBHdr->pBitmaps; pTemp--) {
          nButtons += pTemp->nButtons;
     }

     return(nButtons);
}

static BOOL
InsertButtons(
     HWND           hWnd,
     PTBHDR         pTBHdr,
     UINT           uWhere,
     UINT           uButtons,
     LPTBBUTTON     lpButtons
) {
     PTBBUTTON pIn, pOut;

     if (!pTBHdr) {
          return(FALSE);
     }

     pTBHdr = (PTBHDR)LocalReAlloc((HANDLE)pTBHdr, sizeof(TBHDR)-sizeof(TBBUTTON)
               + (pTBHdr->iNumButtons+uButtons)*sizeof(TBBUTTON), LMEM_MOVEABLE);
     if (!pTBHdr) {
          return(FALSE);
     }

     SETWINDOWPOINTER(hWnd, PTBHDR, pTBHdr);

     if (uWhere > (UINT)pTBHdr->iNumButtons) {
          uWhere = pTBHdr->iNumButtons;
     }

     for (pIn=pTBHdr->Buttons+pTBHdr->iNumButtons-1, pOut=pIn+uButtons,
        uWhere=(UINT)pTBHdr->iNumButtons-uWhere; uWhere>0;
        pIn--, pOut--, uWhere--) {
          *pOut = *pIn;
     }

     for (lpButtons=lpButtons+uButtons-1, pTBHdr->iNumButtons+=(int)uButtons;
       uButtons>0;
       pOut--, lpButtons--, uButtons--) {
          *pOut = *lpButtons;

          if ((pOut->fsStyle & TBSTYLE_SEP) && pOut->iBitmap <= 0) {
               pOut->iBitmap = BUTTONSEPERATOR;
          }
    }

     /*
     ** We need to completely redraw the toolbar at this point.
     */	      
     InvalidateRect(hWnd, NULL, TRUE);

     return(TRUE);
}

static BOOL
DeleteButton(
     HWND      hWnd,
     PTBHDR    pTBHdr,
     UINT      uIndex
) {
     PTBBUTTON pIn, pOut;

     if (uIndex >= (UINT)pTBHdr->iNumButtons) {
          return(FALSE);
     }

     pTBHdr->iNumButtons--;
     for (pOut = pTBHdr->Buttons + uIndex, pIn = pOut + 1;
          uIndex < (UINT) pTBHdr->iNumButtons; ++uIndex, ++pIn, ++pOut
     ) {
          *pOut = *pIn;
     }

     /*
     ** We need to completely redraw the toolbar at this point.
     */
     InvalidateRect(hWnd, NULL, TRUE);

     return(TRUE);
}

LRESULT CALLBACK _export
ToolbarWndProc(
     HWND      hWnd,
     UINT      wMsg,
     WPARAM    wParam,
     LPARAM    lParam
) {
     BOOL      fSameButton;
     PTBBUTTON ptbButton;
     PTBHDR    pTBHdr;
     int       iPos;
     BYTE      fsState;

     pTBHdr = GETWINDOWPOINTER(hWnd, PTBHDR);

     switch (wMsg) {
     case WM_CREATE:
	     #define lpcs ((LPCREATESTRUCT)lParam)

          if (!CreateDitherBrush(FALSE)) {
               return -1;
          }

	     if (!InitGlobalObjects()) {
               FreeGlobalObjects();
	          return -1;
          }

          /*
          ** Create the state data for this toolbar
          */
          pTBHdr = ALLOCWINDOWPOINTER(PTBHDR, sizeof(TBHDR)-sizeof(TBBUTTON));
          if (!pTBHdr) {
	          return -1;
          }

          /*
          ** The struct is initialized to all NULL when created.
          */
          pTBHdr->hwndCommand = lpcs->hwndParent;

          SETWINDOWPOINTER(hWnd, PTBHDR, pTBHdr);
	     break;

     case WM_DESTROY:
          if (pTBHdr) {
	          PTBBMINFO pTemp;
	          int       i;

               /*
               ** Free all the bitmaps before exiting
               */
               for (pTemp=pTBHdr->pBitmaps, i=pTBHdr->nBitmaps-1; i>=0; ++pTemp, i--) {
		          if (pTemp->hInst && pTemp->hbm) {
		               DeleteObject(pTemp->hbm);
                    }
	          }

               FREEWINDOWPOINTER(pTBHdr);
               SETWINDOWPOINTER(hWnd, PTBHDR, 0);
	     }
          FreeGlobalObjects();
          FreeDitherBrush();
          break;

     case WM_PAINT:
          ToolbarPaint(hWnd, pTBHdr);
	     break;

     case WM_SIZE:
          {
               RECT rcParent;
               HWND hwndParent;

               /*
               ** If there is no parent, then this is a top level window
               ** so don't size the toolbar
               */
	          hwndParent = GetParent(hWnd);
               if (hwndParent) {
                    GetClientRect(hwndParent, &rcParent);

                    if (usWindowWidth != 0) {
                         rcParent.right = usWindowWidth;
                         rcParent.right -= GetSystemMetrics(SM_CXBORDER);
                    }

                    SetWindowPos(  hWnd, NULL, 0, 0,
                                   rcParent.right+(GetSystemMetrics(SM_CXBORDER)*2),
                                   usWindowHeight,
                                   SWP_NOZORDER
                              );
               }
	          break;
          }

     case WM_COMMAND:
     case WM_DRAWITEM:
     case WM_MEASUREITEM:
     case WM_VKEYTOITEM:
     case WM_CHARTOITEM:
          SendMessage(pTBHdr->hwndCommand, wMsg, wParam, lParam);
          break;

     case WM_LBUTTONDOWN:
          iPos = TBHitTest(pTBHdr, LOWORD(lParam), HIWORD(lParam));
          if (iPos >= 0) {
               ptbButton = pTBHdr->Buttons + iPos;

               pTBHdr->pCaptureButton = ptbButton;
	          SetCapture(hWnd);

	          if (ptbButton->fsState & TBSTATE_ENABLED) {
		          ptbButton->fsState |= TBSTATE_PRESSED;
                    InvalidateButton(hWnd, pTBHdr, ptbButton);
		          UpdateWindow(hWnd);         // imedeate feedback
	          }

               SendMessage(pTBHdr->hwndCommand, WM_COMMAND, GETWINDOWID(hWnd), MAKELONG(pTBHdr->pCaptureButton->idCommand, TBN_BEGINDRAG));
	     }
	     break;

     case WM_MOUSEMOVE:
          if (pTBHdr->pCaptureButton!=NULL && (pTBHdr->pCaptureButton->fsState & TBSTATE_ENABLED)) {
               iPos = TBHitTest(pTBHdr, LOWORD(lParam), HIWORD(lParam));
	          fSameButton = (iPos>=0 && pTBHdr->pCaptureButton==pTBHdr->Buttons+iPos);
               if (fSameButton == !(pTBHdr->pCaptureButton->fsState & TBSTATE_PRESSED)) {
                    pTBHdr->pCaptureButton->fsState ^= TBSTATE_PRESSED;
                    InvalidateButton(hWnd, pTBHdr, pTBHdr->pCaptureButton);
	          }
	     }
	     break;

     case WM_LBUTTONUP:
          if (pTBHdr->pCaptureButton != NULL) {
	          int idCommand;

               idCommand = pTBHdr->pCaptureButton->idCommand;
	          ReleaseCapture();
               SendMessage(pTBHdr->hwndCommand, WM_COMMAND, GETWINDOWID(hWnd), MAKELONG(idCommand, TBN_ENDDRAG));

               iPos = TBHitTest(pTBHdr, LOWORD(lParam), HIWORD(lParam));
               if ((pTBHdr->pCaptureButton->fsState&TBSTATE_ENABLED) && iPos>=0
                  && (pTBHdr->pCaptureButton==pTBHdr->Buttons+iPos)
               ) {
                    pTBHdr->pCaptureButton->fsState &= ~TBSTATE_PRESSED;

                    if (pTBHdr->pCaptureButton->fsStyle & TBSTYLE_CHECK) {
                         if (pTBHdr->pCaptureButton->fsStyle & TBSTYLE_GROUP) {
                              /*
                              ** Group buttons already checked can't be forced
                              ** up by the user.
                              */
                              if (pTBHdr->pCaptureButton->fsState & TBSTATE_CHECKED) {
                                   pTBHdr->pCaptureButton = NULL;
			                    break;
                	          }

                              pTBHdr->pCaptureButton->fsState |= TBSTATE_CHECKED;
                              MakeGroupConsistant(hWnd, pTBHdr, idCommand);
		               }
                         else {
                              pTBHdr->pCaptureButton->fsState ^= TBSTATE_CHECKED;
		               }
		          }
                    InvalidateButton(hWnd, pTBHdr, pTBHdr->pCaptureButton);
                    pTBHdr->pCaptureButton = NULL;
                    SendMessage(pTBHdr->hwndCommand, WM_COMMAND, idCommand, 0L);
	          }
	          else {
                    pTBHdr->pCaptureButton = NULL;
	          }
          }
          break;

     case TB_SETSTATE:
          iPos = PositionFromID(pTBHdr, (int)wParam);
	     if (iPos < 0) {
	          return(FALSE);
          }
          ptbButton = pTBHdr->Buttons + iPos;

	     fsState = (BYTE)(LOWORD(lParam) ^ ptbButton->fsState);
          ptbButton->fsState = (BYTE)LOWORD(lParam);

	     if (fsState & TBSTATE_HIDDEN) {
	          InvalidateRect(hWnd, NULL, TRUE);
          }
	     else if (fsState) {
               InvalidateButton(hWnd, pTBHdr, ptbButton);
          }
          return(TRUE);

     case TB_GETSTATE:
          iPos = PositionFromID(pTBHdr, (int)wParam);
	     if (iPos < 0) {
	          return(-1L);
          }
          return(pTBHdr->Buttons[iPos].fsState);

     case TB_ENABLEBUTTON:
     case TB_CHECKBUTTON:
     case TB_PRESSBUTTON:
     case TB_HIDEBUTTON:
     case TB_INDETERMINATE:
          iPos = PositionFromID(pTBHdr, (int)wParam);
	     if (iPos < 0) {
	          return(FALSE);
          }
          ptbButton = &pTBHdr->Buttons[iPos];
          fsState   = ptbButton->fsState;

          if (LOWORD(lParam)) {
               ptbButton->fsState |= wStateMasks[wMsg - TB_ENABLEBUTTON];
          }
	     else {
               ptbButton->fsState &= ~wStateMasks[wMsg - TB_ENABLEBUTTON];
          }

          /*
          ** Did this actually change the state?
          */
          if (fsState != ptbButton->fsState) {
               /*
               ** Is this button a member of a group?
               */
               if ((wMsg == TB_CHECKBUTTON) && (ptbButton->fsStyle & TBSTYLE_GROUP)) {
                    MakeGroupConsistant(hWnd, pTBHdr, (int)wParam);
               }

	          if (wMsg == TB_HIDEBUTTON) {
		          InvalidateRect(hWnd, NULL, TRUE);
               }
	          else {
                    InvalidateButton(hWnd, pTBHdr, ptbButton);
               }
          }
          return(TRUE);

     case TB_ISBUTTONENABLED:
     case TB_ISBUTTONCHECKED:
     case TB_ISBUTTONPRESSED:
     case TB_ISBUTTONHIDDEN:
     case TB_ISBUTTONINDETERMINATE:
          iPos = PositionFromID(pTBHdr, (int)wParam);
	     if (iPos < 0) {
	          return(-1L);
          }
          return (LRESULT)pTBHdr->Buttons[iPos].fsState & wStateMasks[wMsg - TB_ISBUTTONENABLED];

     case TB_ADDBITMAP:
          return(AddBitmap(pTBHdr, wParam, (HINSTANCE)LOWORD(lParam), HIWORD(lParam)));

     case TB_ADDBUTTONS:
          return(InsertButtons(hWnd, pTBHdr, (UINT)-1, wParam, (LPTBBUTTON)lParam));

     case TB_INSERTBUTTON:
          return(InsertButtons(hWnd, pTBHdr, wParam, 1, (LPTBBUTTON)lParam));

     case TB_DELETEBUTTON:
          return(DeleteButton(hWnd, pTBHdr, wParam));

     case TB_GETBUTTON:
          if (wParam >= (UINT)pTBHdr->iNumButtons) {
	          return(FALSE);
          }
          *((LPTBBUTTON)lParam) = *(pTBHdr->Buttons+wParam);
	     return(TRUE);

     case TB_BUTTONCOUNT:
          return(pTBHdr->iNumButtons);

     case TB_COMMANDTOINDEX:
          return(PositionFromID(pTBHdr, (int)wParam));

     default:
	     return DefWindowProc(hWnd, wMsg, wParam, lParam);
     }

     return 0L;
}

static void
GetSysColors(
     PTBHDR    pTBHdr
) {
	static COLORREF rgbSaveFace    = 0xffffffffL,
	                rgbSaveShadow  = 0xffffffffL,
	                rgbSaveHilight = 0xffffffffL,
	                rgbSaveFrame   = 0xffffffffL;

	rgbFace    = GetSysColor(COLOR_BTNFACE);
	rgbShadow  = GetSysColor(COLOR_BTNSHADOW);
	rgbHilight = GetSysColor(COLOR_BTNHIGHLIGHT);
	rgbFrame   = GetSysColor(COLOR_WINDOWFRAME);

	if ( rgbSaveFace!=rgbFace          || rgbSaveShadow!=rgbShadow ||
          rgbSaveHilight!=rgbHilight    || rgbSaveFrame!=rgbFrame
     ) {
          int       i;
          PTBBMINFO pBitmap;

		rgbSaveFace    = rgbFace;
		rgbSaveShadow  = rgbShadow;
		rgbSaveHilight = rgbHilight;
          rgbSaveFrame   = rgbFrame;

          /*
          ** Update the brush for pushed-in buttons
          */
		CreateDitherBrush(TRUE);

          /*
          ** Reset all of the bitmaps if the sys colors have changed
          ** since the last time the bitmaps were created.
          */
          for (i=pTBHdr->nBitmaps-1, pBitmap=pTBHdr->pBitmaps; i>=0; i--, ++pBitmap) {
               if (pBitmap->hInst && pBitmap->hbm) {
                    DeleteObject(pBitmap->hbm);
                    pBitmap->hbm = NULL;
               }
          }
     }
}

static HBITMAP
CreateDitherBitmap(
     void
) {
    PBITMAPINFO     pbmi;
    HBITMAP         hbm;
    HDC             hdc;
    int             i;
    long            patGray[8];
    DWORD           rgb;

     pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 16));
     if (!pbmi) {
        return NULL;
     }

     pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
     pbmi->bmiHeader.biWidth = 8;
     pbmi->bmiHeader.biHeight = 8;
     pbmi->bmiHeader.biPlanes = 1;
     pbmi->bmiHeader.biBitCount = 1;
     pbmi->bmiHeader.biCompression = BI_RGB;

     rgb = GetSysColor(COLOR_BTNFACE);
     pbmi->bmiColors[0].rgbBlue  = GetBValue(rgb);
     pbmi->bmiColors[0].rgbGreen = GetGValue(rgb);
     pbmi->bmiColors[0].rgbRed   = GetRValue(rgb);
     pbmi->bmiColors[0].rgbReserved = 0;

     rgb = GetSysColor(COLOR_BTNHIGHLIGHT);
     pbmi->bmiColors[1].rgbBlue  = GetBValue(rgb);
     pbmi->bmiColors[1].rgbGreen = GetGValue(rgb);
     pbmi->bmiColors[1].rgbRed   = GetRValue(rgb);
     pbmi->bmiColors[1].rgbReserved = 0;

     /*
     ** Initialize the brushes 
     */
     for (i = 0; i < 8; i++) {
          if (i & 1) {
               patGray[i] = 0xAAAA5555L;   //  0x11114444L; // lighter gray
          }
          else {
               patGray[i] = 0x5555AAAAL;   //  0x11114444L; // lighter gray
          }
     }
     hdc = GetDC(NULL);
     hbm = CreateDIBitmap(hdc, &pbmi->bmiHeader, CBM_INIT, patGray, pbmi, DIB_RGB_COLORS);

     ReleaseDC(NULL, hdc);
     LocalFree((HANDLE)pbmi);

     return hbm;
}

static BOOL
CreateDitherBrush(
     BOOL bIgnoreCount
) {
	HBITMAP hbmGray;
	HBRUSH hbrSave;

	if (bIgnoreCount && !iDitherCount) {
          return TRUE;
	}

	if (iDitherCount>0 && !bIgnoreCount) {
		iDitherCount++;
		return TRUE;
	}

	hbmGray = CreateDitherBitmap();
	if (hbmGray) {
		hbrSave = hbrDither;
		hbrDither = CreatePatternBrush(hbmGray);
		DeleteObject(hbmGray);
		if (hbrDither) {
			if (hbrSave) {
				DeleteObject(hbrSave);
			}
			if (!bIgnoreCount) {
				iDitherCount = 1;
			}
			return TRUE;
		}
		else {
			hbrDither = hbrSave;
		}
	}

	return FALSE;
}

static BOOL
FreeDitherBrush(
     void
) {
     iDitherCount--;

     if (iDitherCount > 0) {
          return FALSE;
     }

     DeleteObject(hbrDither);
     hbrDither = NULL;

     return TRUE;
}

typedef struct tagCOLORMAP {
    COLORREF bgrfrom;
    COLORREF bgrto;
    COLORREF sysColor;
} COLORMAP;

/*
** These are the default colors used to map the dib colors
** to the current system colors
*/ 
#define BGR_BUTTONTEXT      (RGB(000,000,000))  // black
#define BGR_BUTTONSHADOW    (RGB(128,128,128))  // dark grey
#define BGR_BUTTONFACE      (RGB(192,192,192))  // bright grey
#define BGR_BUTTONHILIGHT   (RGB(255,255,255))  // white
#define BGR_BACKGROUNDSEL   (RGB(255,000,000))  // blue
#define BGR_BACKGROUND      (RGB(255,000,255))  // magenta
#define FlipColor(rgb)      (RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb)))

static HBITMAP WINAPI
CreateMappedBitmap(
     HINSTANCE hInstance,
     int       idBitmap
) {
     static COLORMAP ColorMap[] = {
          {BGR_BUTTONTEXT,    BGR_BUTTONTEXT,    COLOR_BTNTEXT},     // black
          {BGR_BUTTONSHADOW,  BGR_BUTTONSHADOW,  COLOR_BTNSHADOW},   // dark grey
          {BGR_BUTTONFACE,    BGR_BUTTONFACE,    COLOR_BTNFACE},     // bright grey
          {BGR_BUTTONHILIGHT, BGR_BUTTONHILIGHT, COLOR_BTNHIGHLIGHT},// white
          {BGR_BACKGROUNDSEL, BGR_BACKGROUNDSEL, COLOR_HIGHLIGHT},   // blue
          {BGR_BACKGROUND,    BGR_BACKGROUND,    COLOR_WINDOW}       // magenta
     };
     HDC			     hdc, hdcMem = NULL;
     HANDLE		     h;
     DWORD FAR		     *p;
     LPSTR 		     lpBits;
     HANDLE		     hRes;
     LPBITMAPINFOHEADER	lpBitmapInfo;
     HBITMAP		     hbm = NULL, hbmOld;
     int                 numcolors, i;
     int                 wid, hgt;

     #define NUM_MAPS (sizeof(ColorMap)/sizeof(COLORMAP))

     h = FindResource(hInstance, MAKEINTRESOURCE(idBitmap), RT_BITMAP);
     if (!h) {
          return NULL;
     }

     hRes = LoadResource(hInstance, (HRSRC) h);

     /*
     ** Lock the bitmap and get a pointer to the color table.
     */
     lpBitmapInfo = (LPBITMAPINFOHEADER)LockResource(hRes);
     if (!lpBitmapInfo) {
  	     return NULL;
     }

     /*
     ** So what are the new colors anyway ?
     */
     for (i=0; i < NUM_MAPS; i++) {
          ColorMap[i].bgrto = FlipColor(GetSysColor((int)ColorMap[i].sysColor));
     }

     p = (DWORD FAR *)(((LPSTR)lpBitmapInfo) + lpBitmapInfo->biSize);

     /*
     ** Replace button-face and button-shadow colors with the current values
     */
     numcolors = 16;

     while (numcolors-- > 0) {
          for (i = 0; i < NUM_MAPS; i++) {
               if (*p == ColorMap[i].bgrfrom) {
                    *p = ColorMap[i].bgrto;
	               break;
	          }
          }
          p++;
     }

     /*
     ** First skip over the header structure
     */
     lpBits = (LPSTR)(lpBitmapInfo + 1);

     /*
     ** Skip the color table entries, if any
     */
     lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);

     /*
     ** Create a color bitmap compatible with the display device
     */
     i = wid = (int)lpBitmapInfo->biWidth;
     hgt = (int)lpBitmapInfo->biHeight;
     hdc = GetDC(NULL);

     hdcMem = CreateCompatibleDC(hdc);
     if (hdcMem) {
          hbm = CreateDiscardableBitmap(hdc, i, hgt);
          if (hbm) {
               hbmOld = (HBITMAP) SelectObject(hdcMem, hbm);

               /*
               ** Set the main image
               */
               StretchDIBits(hdcMem, 0, 0, wid, hgt, 0, 0, wid, hgt, lpBits,
                         (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

               SelectObject(hdcMem, hbmOld);
          }
          DeleteObject(hdcMem);
     }

     ReleaseDC(NULL, hdc);

     UnlockResource(hRes);
     FreeResource(hRes);

     return hbm;
}

_MFN
ToolBarButton::ToolBarButton(
     int  id,
     HWND hWndToolBar
) {
     nButtonId                     = id;
     ToolBarButton::hWndToolBar    = hWndToolBar;
}

_MFN ToolBarButton::~ToolBarButton(
     void
) {
     ;
}

void _MFN
ToolBarButton::Enable(
     BOOL enable_it
) {
     SendMessage(Handle(), TB_ENABLEBUTTON, ResourceId(), MAKELONG(enable_it, 0));
}

void _MFN
ToolBarButton::Show(
     BOOL show_it
) {
     SendMessage(Handle(), TB_HIDEBUTTON, ResourceId(), MAKELONG(!show_it, 0));
}

void _MFN
ToolBarButton::Check(
     BOOL checked
) {
     SendMessage(Handle(), TB_CHECKBUTTON, ResourceId(), MAKELONG(checked, 0));
}

BOOL _MFN
ToolBarButton::IsChecked(
     void
) {
     return LOWORD(SendMessage(Handle(), TB_ISBUTTONCHECKED, ResourceId(), 0L));
}

/*---------------------------------------------------------------------------
** End of File
*/

