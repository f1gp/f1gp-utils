/*
** File   : loglstbx.cpp
** Author : TK
** Date   : 11/11/95
**
** Grand Log Listbox class.
*/

/*---------------------------------------------------------------------------
** Includes
*/

#include "stdafx.h"
#include "gplapw.h"
#include "gplapvw.h"
#include "loglstbx.h"
#include "laputl.h"

#ifdef _DEBUG
#undef THIS_FILE
static TCHAR BASED_CODE THIS_FILE[] = __FILE__;
#endif

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

BEGIN_MESSAGE_MAP(CLogListBox, CListBox)
	//{{AFX_MSG_MAP(CLogListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*---------------------------------------------------------------------------
** Functions
*/

CLogListBox::CLogListBox()
{
     nCurrentFontHeight  = 0;
     pviewParent         = NULL;
     fCircuit            = FALSE;
     nCircuit            = 0;

     ShowComputer();
}

CLogListBox::~CLogListBox()
{
}

void CLogListBox::SetParentsView(CGplapwView *pviewParentsView)
{
	ASSERT(pviewParentsView != NULL);
	pviewParent = pviewParentsView;
}

void CLogListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = DetermineItemHeight(lpMeasureItemStruct->itemHeight);
}

LONG CLogListBox::DetermineItemHeight(LONG lItemHeight)
{
	if (lItemHeight < BITMAP_HEIGHT) {
		lItemHeight = BITMAP_HEIGHT;
	}

	LOGFONT 	lf;
	CFont	*pfnt = GetFont();

	if (pfnt) {
		pfnt->GetObject(sizeof(lf), &lf);
	}
	else {
		//
		// Listbox is using default system font.
		//
		HFONT hf = (HFONT) ::GetStockObject(SYSTEM_FIXED_FONT);
		::GetObject(hf, sizeof(lf), &lf);
	}

	//
	// Font sizes can be negative, so get absolute value
	//
	lf.lfHeight = abs(lf.lfHeight);
	nCurrentFontHeight = lf.lfHeight;

	if (lItemHeight < lf.lfHeight) {
		lItemHeight = lf.lfHeight;
	}

	return lItemHeight;
}

void CLogListBox::RecalculateItemHeight() {
	SetItemHeight(0, DetermineItemHeight(0));
}

void CLogListBox::DrawItem(LPDRAWITEMSTRUCT lpDis)
{
	CDC				*pDC	= CDC::FromHandle(lpDis->hDC);
     COLORREF            fgc = GetSysColor(COLOR_WINDOWTEXT);
     COLORREF            bgc = GetSysColor(COLOR_WINDOW);
     LB_ENTRY            *le = (LB_ENTRY *) lpDis->itemData;
     LAP_RECORD          *lr = &le->u.lr;
     AUTH_RECORD         *ar = &le->u.ar;
     CRect               r;
     TCHAR               buff[132];

     if (lpDis->itemAction & ODA_DRAWENTIRE) {
		r = lpDis->rcItem;

          if (lpDis->itemState & ODS_DISABLED || lpDis->itemState & ODS_GRAYED) {
               fgc = GetSysColor(COLOR_GRAYTEXT);
          }

          UINT	b  = 0;
          UINT bm = 0;

          if (lr->lr_record_type == REC_TYPE_LAP) {
               if ((lr->lr_lap_time & TIME_MASK) == 0UL) {
                    if (le->fastest_race_lap) {
                         b  = IDR_BMP_FL;
                         bm = IDR_BMP_FL_MASK;
                    }
                    else if (le->fastest_qual_lap) {
                         b  = IDR_BMP_Q1;
                         bm = IDR_BMP_Q1_MASK;
                    }
                    else if (lr->lr_lap_time == lr->lr_best_time) {
                         b  = IDR_BMP_BL;
                         bm = IDR_BMP_BL_MASK;
                    }

                    if (IS_RACE(lr->lr_game_mode) && lr->lr_car_position == 1) {

                         if (fShowComputer) {
                              fgc = GetSysColor(COLOR_HIGHLIGHTTEXT);
                              bgc = GetSysColor(COLOR_HIGHLIGHT);
                         }

                         if (le->fastest_race_lap || le->fastest_qual_lap) {
                              b  = IDR_BMP_P1_FL;
                              bm = IDR_BMP_P1_MASK;
                         }
                         else if (lr->lr_lap_time == lr->lr_best_time) {
                              b  = IDR_BMP_P1_BL;
                              bm = IDR_BMP_P1_MASK;
                         }
                         else {
                              b   = IDR_BMP_P1;
                              bm  = IDR_BMP_P1_MASK;
                         }

                         if (lr->lr_lap_number == LapCircuitLaps(CIRCUIT(lr->lr_game_mode))) {
                              b   = IDR_BMP_RW;
                              bm  = IDR_BMP_RW_MASK;
                         }
                    }
               }

               if (fShowComputer && IS_PLAYER(lr->lr_car_number)) {
                    fgc = GetSysColor(COLOR_HIGHLIGHTTEXT);
                    bgc = GetSysColor(COLOR_HIGHLIGHT);
               }
          }
          else if (lr->lr_record_type == REC_TYPE_AUTH) {
               if ((ar->ar_seconds & 0x80) && (ar->ar_seconds & 0x20)) {
                    b  = IDR_BMP_AUTH_GRASS;
               }
               else {
                    b  = IDR_BMP_AUTH;
               }
               bm = IDR_BMP_AUTH_MASK;
          }
          else if (lr->lr_record_type == REC_TYPE_AUTH_RACE) {
               b  = IDR_BMP_AUTH_RACE;
               bm = IDR_BMP_AUTH_MASK;
          }

          if ( lr->lr_record_type == REC_TYPE_AUTH          ||
               lr->lr_record_type == REC_TYPE_AUTH_RACE     ||
               lr->lr_record_type == REC_TYPE_DATE          ||
               lr->lr_record_type == REC_TYPE_SETUP
          ) {
               fgc = GetSysColor(COLOR_GRAYTEXT);
          }

          if (lr->lr_record_type == REC_TYPE_PIT_IN) {
               if (fShowComputer && IS_PLAYER(le->u.pi.pi_car_number)) {
                    fgc = GetSysColor(COLOR_HIGHLIGHTTEXT);
                    bgc = GetSysColor(COLOR_HIGHLIGHT);
               }
          }

          if (lr->lr_record_type == REC_TYPE_PIT_OUT) {
               if (fShowComputer && IS_PLAYER(le->u.po.po_car_number)) {
                    fgc = GetSysColor(COLOR_HIGHLIGHTTEXT);
                    bgc = GetSysColor(COLOR_HIGHLIGHT);
               }
          }

          LbEntryToText(buff, le);

          CBrush brush(bgc);
          pDC->FillRect(&r, &brush);

          if (b) {
               DrawBitmap(pDC, b, bm, &lpDis->rcItem);
          }

		INT	centre_offset = 0;
		if (nCurrentFontHeight != 0) {
			INT	height;
			height = r.bottom - r.top;
			if (height > nCurrentFontHeight) {
				centre_offset = (height - nCurrentFontHeight) / 2;
			}
		}
		pDC->SetTextColor(fgc);
		pDC->SetBkColor(bgc);
		pDC->ExtTextOut(	BITMAP_WIDTH + BITMAP_SPACE,
						lpDis->rcItem.top + centre_offset,
						ETO_CLIPPED,
						&lpDis->rcItem,
						buff,
                              lstrlen(buff),
						NULL
					);

          if (lpDis->itemState & ODS_SELECTED) {
               r = lpDis->rcItem;
               r.left += BITMAP_WIDTH + (BITMAP_SPACE / 2);
               pDC->InvertRect(&r);
          }

          if (lpDis->itemState & ODS_FOCUS) {
               pDC->DrawFocusRect(&lpDis->rcItem);
          }
     }
     else {
          if (lpDis->itemAction & ODA_SELECT) {
               RECT r = lpDis->rcItem;

               r.left += BITMAP_WIDTH + (BITMAP_SPACE / 2);
               pDC->InvertRect(&r);
          }

          if (lpDis->itemAction & ODA_FOCUS) {
               pDC->DrawFocusRect(&lpDis->rcItem);
          }
     }

     if ((lpDis->itemAction & ODA_FOCUS) || !fCircuit) {
          if ( lr->lr_record_type == REC_TYPE_LAP           ||
               lr->lr_record_type == REC_TYPE_AUTH          ||
               lr->lr_record_type == REC_TYPE_AUTH_RACE     ||
               lr->lr_record_type == REC_TYPE_SETUP         ||
               lr->lr_record_type == REC_TYPE_PIT_IN        ||
               lr->lr_record_type == REC_TYPE_PIT_OUT
          ) {
               if (!fCircuit || nCircuit != CIRCUIT(lr->lr_game_mode)) {
                    fCircuit = TRUE;
                    nCircuit = CIRCUIT(lr->lr_game_mode);

				ASSERT(pviewParent != NULL);
				pviewParent->NewCircuit(nCircuit);
               }
          }
     }
}

void CLogListBox::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
     LB_ENTRY	*le = (LB_ENTRY *) lpDeleteItemStruct->itemData;
	delete le;
}

void CLogListBox::DrawBitmap(CDC *pDC, UINT idr_bm, UINT idr_bm_mask, RECT *prect) {
     CBitmap	bm;
     CBitmap   bm_mask;
	CBitmap	*pbmpOld;
	CDC		dcOurDC;
	BITMAP	bitmap_info;
	INT		height;
	INT		centre_offset = 0;

	VERIFY(bm.LoadBitmap(idr_bm));
	VERIFY(bm_mask.LoadBitmap(idr_bm_mask));

	VERIFY(dcOurDC.CreateCompatibleDC(pDC));

	bm_mask.GetObject(sizeof(bitmap_info), &bitmap_info);

	height = prect->bottom - prect->top;
	if (height > bitmap_info.bmHeight) {
		centre_offset = (height - bitmap_info.bmHeight) / 2;
	}

	pbmpOld = dcOurDC.SelectObject(&bm_mask);
     {
          pDC->BitBlt(	0, prect->top + centre_offset,
                    	bitmap_info.bmWidth,
                    	bitmap_info.bmHeight,
                    	&dcOurDC,
                    	0, 0,
                    	SRCAND
               	);
     }
	dcOurDC.SelectObject(pbmpOld);

	pbmpOld = dcOurDC.SelectObject(&bm);
     {
          pDC->BitBlt(	0, prect->top + centre_offset,
                    	bitmap_info.bmWidth,
                    	bitmap_info.bmHeight,
                    	&dcOurDC,
                    	0, 0,
                    	SRCPAINT
               );
     }
	dcOurDC.SelectObject(pbmpOld);
}

/*---------------------------------------------------------------------------
** End of File
*/
