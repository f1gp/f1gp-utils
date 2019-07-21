// GpLapvw.cpp : implementation of the CGplapwView class
//

#include "stdafx.h"
#include "GpLapW.h"

#include "GpLapdoc.h"
#include "GpLapvw.h"

#include "LapUtl.h"
#include "WaitCur.h"
#include "DrivFilt.h"
#include "TotalLap.h"

#ifdef _DEBUG
#undef THIS_FILE
static TCHAR BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGplapwView

IMPLEMENT_DYNCREATE(CGplapwView, CView)

BEGIN_MESSAGE_MAP(CGplapwView, CView)
	//{{AFX_MSG_MAP(CGplapwView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FONT_SELECT, OnFontSelect)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PRACTICE, OnUpdateViewPractice)
	ON_UPDATE_COMMAND_UI(ID_VIEW_QUALIFYING, OnUpdateViewQualifying)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RACE, OnUpdateViewRace)
	ON_COMMAND(ID_VIEW_RACE, OnViewRace)
	ON_COMMAND(ID_VIEW_PRACTICE, OnViewPractice)
	ON_COMMAND(ID_VIEW_QUALIFYING, OnViewQualifying)
	ON_COMMAND(ID_VIEW_COMPUTERSCARS, OnViewComputerscars)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMPUTERSCARS, OnUpdateViewComputerscars)
	ON_COMMAND(ID_VIEW_PLAYERSCAR, OnViewPlayerscar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PLAYERSCAR, OnUpdateViewPlayerscar)
	ON_COMMAND(ID_VIEW_LEADER, OnViewLeader)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LEADER, OnUpdateViewLeader)
	ON_COMMAND(ID_VIEW_FASTESTLAPS, OnViewFastestlaps)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FASTESTLAPS, OnUpdateViewFastestlaps)
	ON_COMMAND(ID_VIEW_BESTLAPS, OnViewBestlaps)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BESTLAPS, OnUpdateViewBestlaps)
	ON_COMMAND(ID_VIEW_AUTHENTICATIONRECORDS, OnViewAuthenticationrecords)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AUTHENTICATIONRECORDS, OnUpdateViewAuthenticationrecords)
	ON_COMMAND(ID_VIEW_DISPLAYAVERAGESPEEDS, OnViewDisplayaveragespeeds)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAYAVERAGESPEEDS, OnUpdateViewDisplayaveragespeeds)
	ON_COMMAND(ID_VIEW_MPHORKPH, OnViewMphorkph)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MPHORKPH, OnUpdateViewMphorkph)
	ON_COMMAND(ID_VIEW_ELAPSEDRACETIME, OnViewElapsedracetime)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ELAPSEDRACETIME, OnUpdateViewElapsedracetime)
	ON_COMMAND(ID_VIEW_CARSETUP, OnViewCarSetup)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CARSETUP, OnUpdateViewCarSetup)
	ON_COMMAND(ID_VIEW_PITSTOPS, OnViewPitStops)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PITSTOPS, OnUpdateViewPitStops)
	ON_COMMAND(ID_DRIVERS_DRIVERFILTER, OnDriversDriverFilter)
	ON_UPDATE_COMMAND_UI(ID_DRIVERS_DRIVERFILTER, OnUpdateDriversDriverfilter)
	ON_COMMAND(ID_INFO_TOTALLAPS, OnInfoTotalLaps)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_FONT_RESTOREDEFAULT, OnFontRestoreDefault)
	ON_COMMAND(ID_FILE_AUTHENTICATION_TOCLIPBOARD, OnFileAuthenticationToClipboard)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_AUTHENTICATION_TOTEXTFILE, OnFileAuthenticationToTextFile)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGplapwView construction/destruction

//
// Avoid 0 for registry as we can't determine the presence of an entry.
//
#define INI_TRUE	2
#define INI_FALSE	1


static TCHAR BASED_CODE szSectionKey[]		= _T("Settings");
static TCHAR BASED_CODE szFontNameKey[]		= _T("Font Name");
static TCHAR BASED_CODE szFontHeightKey[]	= _T("Font Height");
static TCHAR BASED_CODE szPracticeKey[]		= _T("Practice");
static TCHAR BASED_CODE szQualifyingKey[]	= _T("Qualifying");
static TCHAR BASED_CODE szRaceKey[]		= _T("Race");
static TCHAR BASED_CODE szComputerKey[]		= _T("Computer");
static TCHAR BASED_CODE szLeaderKey[]		= _T("Leader");
static TCHAR BASED_CODE szPlayerKey[]		= _T("Player");
static TCHAR BASED_CODE szAuthKey[]		= _T("Auth");
static TCHAR BASED_CODE szFastestKey[]		= _T("Fastest");
static TCHAR BASED_CODE szBestKey[]		= _T("Best");
static TCHAR BASED_CODE szAvgSpeedKey[]		= _T("Avg Speed");
static TCHAR BASED_CODE szAvgSpeedMphKey[]	= _T("Avg Speed MPH");
static TCHAR BASED_CODE szElapsedKey[]		= _T("Elasped");
static TCHAR BASED_CODE szCarSetupKey[]		= _T("Car Setup");
static TCHAR BASED_CODE szPitKey[]			= _T("Pit");

CGplapwView::CGplapwView()
{
	m_uFontHeight 	= 0;
	m_uTitleHeight	= 0;

     m_uPrintHeaderBlankLines = 5;
     m_uPrintFooterBlankLines = 5;
     m_uPrintHeaderLines      = 1 + 1 + 2;
     m_uPrintFooterLines      = 1 + 1;

	//
	// Force driver names to be added of first scan of log.
	//
	fInitialiseDriverNames	 = TRUE;

	// from INI file
     fShowPractice		= (AfxGetApp()->GetProfileInt(szSectionKey, szPracticeKey,		INI_TRUE)		== INI_TRUE);
     fShowQualifying	= (AfxGetApp()->GetProfileInt(szSectionKey, szQualifyingKey,	INI_TRUE)		== INI_TRUE);
     fShowRace			= (AfxGetApp()->GetProfileInt(szSectionKey, szRaceKey,			INI_TRUE)		== INI_TRUE);
     fShowComputer		= (AfxGetApp()->GetProfileInt(szSectionKey, szComputerKey,		INI_TRUE)		== INI_TRUE);
     fShowLeader		= (AfxGetApp()->GetProfileInt(szSectionKey, szLeaderKey,		INI_FALSE)	== INI_TRUE);
     fShowPlayer		= (AfxGetApp()->GetProfileInt(szSectionKey, szPlayerKey,		INI_TRUE)		== INI_TRUE);
     fShowAuth			= (AfxGetApp()->GetProfileInt(szSectionKey, szAuthKey,			INI_TRUE)		== INI_TRUE);
     fShowFastest		= (AfxGetApp()->GetProfileInt(szSectionKey, szFastestKey,		INI_FALSE)	== INI_TRUE);
     fShowBest			= (AfxGetApp()->GetProfileInt(szSectionKey, szBestKey,			INI_FALSE)	== INI_TRUE);
     fShowAvgSpeed		= (AfxGetApp()->GetProfileInt(szSectionKey, szAvgSpeedKey,		INI_FALSE)	== INI_TRUE);
     fAvgSpeedMph		= (AfxGetApp()->GetProfileInt(szSectionKey, szAvgSpeedMphKey,	INI_TRUE)		== INI_TRUE);
     fElapsed			= (AfxGetApp()->GetProfileInt(szSectionKey, szElapsedKey,		INI_FALSE)	== INI_TRUE);
     fShowSetup		= (AfxGetApp()->GetProfileInt(szSectionKey, szCarSetupKey,		INI_TRUE)		== INI_TRUE);
     fShowPit			= (AfxGetApp()->GetProfileInt(szSectionKey, szPitKey,			INI_TRUE)		== INI_TRUE);
     fFilterDrivers		= FALSE;

     LapSetAvgSpeed(fShowAvgSpeed, fAvgSpeedMph);
     LapSetElapsedMode(fElapsed);
}

CGplapwView::~CGplapwView()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGplapwView drawing

void CGplapwView::OnInitialUpdate()
{
	CGplapwDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CGplapwView printing

BOOL CGplapwView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CGplapwView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CView::OnPrepareDC(pDC, pInfo);
     if (pDC->IsPrinting()) {
          UINT      uLinesPerPage;

          uLinesPerPage  = (pInfo->m_rectDraw.bottom - pInfo->m_rectDraw.top) / abs(m_uFontHeight);
          uLinesPerPage  -= m_uPrintHeaderBlankLines;
          uLinesPerPage  -= m_uPrintFooterBlankLines;
          uLinesPerPage  -= m_uPrintHeaderLines;
          uLinesPerPage  -= m_uPrintFooterLines;

          if (((pInfo->m_nCurPage - 1) * uLinesPerPage) < (unsigned) m_wndLogListBox.GetCount()) {
               pInfo->m_bContinuePrinting = TRUE;
          }
          else {
               pInfo->m_bContinuePrinting = FALSE;
          }
     }
}

void CGplapwView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CGplapwView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CGplapwView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	LB_ENTRY	*le;
     TCHAR     szBuff[132];
     UINT      uLineNum;
     UINT      uListBoxCount;
     UINT      uLinesPerPage;
     UINT      uStartIndex;
     UINT      uEndIndex;
     UINT      uPrintIndex;
     UINT      uLeftOrigin;
     UINT      uTitleWidth;
     INT       iCircuit = -1;
     CFont     *pfntOldFont;

    	CWaitCursor wait;

     uLinesPerPage  = (pInfo->m_rectDraw.bottom - pInfo->m_rectDraw.top) / abs(m_uFontHeight);
     uLinesPerPage  -= m_uPrintHeaderBlankLines;
     uLinesPerPage  -= m_uPrintFooterBlankLines;
     uLinesPerPage  -= m_uPrintHeaderLines;
     uLinesPerPage  -= m_uPrintFooterLines;

     uStartIndex    = uLinesPerPage * (pInfo->m_nCurPage - 1);
     uEndIndex      = uStartIndex + uLinesPerPage;
     uLineNum       = 0;

     pfntOldFont = pDC->SelectObject(&m_font);
     {
     	uListBoxCount = m_wndLogListBox.GetCount();
          for (uPrintIndex = uStartIndex; uPrintIndex < uEndIndex && uPrintIndex < uListBoxCount; uPrintIndex++) {
     		le = (LB_ENTRY *) m_wndLogListBox.GetItemData(uPrintIndex);
               if ( le->u.lr.lr_record_type == REC_TYPE_LAP       ||
                    le->u.lr.lr_record_type == REC_TYPE_AUTH      ||
                    le->u.lr.lr_record_type == REC_TYPE_AUTH_RACE ||
                    le->u.lr.lr_record_type == REC_TYPE_PIT_IN    ||
                    le->u.lr.lr_record_type == REC_TYPE_PIT_OUT
               ) {
                    if (iCircuit != CIRCUIT(le->u.lr.lr_game_mode)) {
                         iCircuit = CIRCUIT(le->u.lr.lr_game_mode);
                         break;
                    }
               }
          }

          // TODO: Left margin

          uTitleWidth = pDC->GetTextExtent(LapTitleUnderlineText(), strlen(LapTitleUnderlineText())).cx;
          if (uTitleWidth < (unsigned) (pInfo->m_rectDraw.right - pInfo->m_rectDraw.left)) {
               uLeftOrigin = ((pInfo->m_rectDraw.right - pInfo->m_rectDraw.left) - uTitleWidth) / 2;
          }
          else {
               uLeftOrigin = 0;
          }
          uLineNum = m_uPrintHeaderBlankLines;

          if (iCircuit != -1) {
               lstrcpy(szBuff, LapCircuitText(iCircuit));
               pDC->SetTextAlign(TA_LEFT);
          	pDC->TextOut(  pInfo->m_rectDraw.left + uLeftOrigin,
          	               uLineNum * abs(m_uFontHeight),
          	               szBuff,
          	               lstrlen(szBuff));
          }
          wsprintf(szBuff, _T("[%s]"), GetDocument()->GetPathName());
          pDC->SetTextAlign(TA_RIGHT);
     	pDC->TextOut(  pInfo->m_rectDraw.right - uLeftOrigin,
     	               uLineNum * abs(m_uFontHeight),
     	               szBuff,
     	               lstrlen(szBuff));

          pDC->SetTextAlign(TA_LEFT);
          uLineNum++;
          uLineNum++;
     	pDC->TextOut(  pInfo->m_rectDraw.left + uLeftOrigin,
     	               uLineNum * abs(m_uFontHeight),
     	               LapTitleText(),
     	               lstrlen(LapTitleText()));
          uLineNum++;
     	pDC->TextOut(  pInfo->m_rectDraw.left + uLeftOrigin,
     	               uLineNum * abs(m_uFontHeight),
     	               LapTitleUnderlineText(),
     	               lstrlen(LapTitleUnderlineText()));

          uLineNum = m_uPrintHeaderBlankLines + m_uPrintHeaderLines + uLinesPerPage + 1;
          wsprintf(szBuff, _T("%u of %u"), pInfo->m_nCurPage, (uListBoxCount / uLinesPerPage) + 1);
          pDC->SetTextAlign(TA_CENTER);
     	pDC->TextOut(  (pInfo->m_rectDraw.right - pInfo->m_rectDraw.left) / 2,
     	               uLineNum * abs(m_uFontHeight),
     	               szBuff,
     	               lstrlen(szBuff));

          pDC->SetTextAlign(TA_LEFT);
          uLineNum = m_uPrintHeaderBlankLines + m_uPrintHeaderLines;
     	uListBoxCount = m_wndLogListBox.GetCount();
          for (uPrintIndex = uStartIndex; uPrintIndex < uEndIndex && uPrintIndex < uListBoxCount; uPrintIndex++) {
     		le = (LB_ENTRY *) m_wndLogListBox.GetItemData(uPrintIndex);

               LbEntryToText(szBuff, le);
          	pDC->TextOut(  pInfo->m_rectDraw.left + uLeftOrigin,
          	               uLineNum++ * abs(m_uFontHeight),
          	               szBuff,
          	               lstrlen(szBuff));
          }

          // TODO: Test border mark
          pDC->MoveTo(pInfo->m_rectDraw.left  + 1, pInfo->m_rectDraw.top    + 1);
          pDC->LineTo(pInfo->m_rectDraw.right - 1, pInfo->m_rectDraw.top    + 1);
          pDC->LineTo(pInfo->m_rectDraw.right - 1, pInfo->m_rectDraw.bottom - 1);
          pDC->LineTo(pInfo->m_rectDraw.left  + 1, pInfo->m_rectDraw.bottom - 1);
          pDC->LineTo(pInfo->m_rectDraw.left  + 1, pInfo->m_rectDraw.top    + 1);
     }
     pDC->SelectObject(pfntOldFont);
}

/////////////////////////////////////////////////////////////////////////////
// CGplapwView diagnostics

#ifdef _DEBUG
void CGplapwView::AssertValid() const
{
	CView::AssertValid();
}

void CGplapwView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGplapwDoc* CGplapwView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGplapwDoc)));
	return (CGplapwDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGplapwView message handlers


int CGplapwView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	LOGFONT lf;
	m_font.CreateStockObject(SYSTEM_FIXED_FONT);
	m_font.GetObject(sizeof(LOGFONT), &lf);

	CString	strFontName;
	strFontName = AfxGetApp()->GetProfileString(szSectionKey, szFontNameKey);
	if (!strFontName.IsEmpty()) {
		memset(&lf, 0x00, sizeof(lf));

		lf.lfPitchAndFamily = FIXED_PITCH;
		strcpy(lf.lfFaceName, strFontName); // TODO: Unicode? HELP!

		m_uFontHeight = AfxGetApp()->GetProfileInt(szSectionKey, szFontHeightKey, 0);
     	if (m_uFontHeight != 0) {
			lf.lfHeight = m_uFontHeight;
		}
		m_font.DeleteObject();
		m_font.CreateFontIndirect(&lf);
	}

	m_uFontHeight 	= abs(lf.lfHeight);
	m_uTitleHeight	= m_uFontHeight + 2;

	VERIFY(	m_wndLogTitleStatic.Create(	LapTitleText(),
									WS_CHILD | WS_VISIBLE,
									CRect(0, 0, 0, 0),
									this
								));
	m_wndLogTitleStatic.SetFont(&m_font, FALSE);

	m_wndLogListBox.SetParentsView(this);
	VERIFY(	m_wndLogListBox.Create(	WS_CHILD 				|
								WS_BORDER				|
								WS_VSCROLL 			|
								WS_VISIBLE 			|
								LBS_NOINTEGRALHEIGHT 	|
								LBS_DISABLENOSCROLL		|
								LBS_OWNERDRAWFIXED		|
								LBS_NOTIFY,
								CRect(0, 0, 0, 0),
								this,
								0
							));
	m_wndLogListBox.SetFont(&m_font, FALSE);
	m_wndLogListBox.RecalculateItemHeight();

	return 0;
}

void CGplapwView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_wndLogTitleStatic.MoveWindow(BITMAP_WIDTH + BITMAP_SPACE, 0, cx, m_uTitleHeight);
	m_wndLogListBox.MoveWindow(0, m_uTitleHeight, cx, cy - m_uTitleHeight);
}

void CGplapwView::OnFontSelect()
{
	//
	// Determine current font.
	//
	LOGFONT lf;
	m_font.GetObject(sizeof(LOGFONT), &lf);

	//
	// Put up dialog with current font selected.
	//
     CFontDialog dlg(&lf, CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT);
     if (dlg.DoModal() == IDOK) {
		SwitchFont(&lf);
     }
}

void CGplapwView::OnFontRestoreDefault()
{
	CFont	fntTemp;
	LOGFONT	lf;

	fntTemp.CreateStockObject(SYSTEM_FIXED_FONT);
	fntTemp.GetObject(sizeof(LOGFONT), &lf);
	SwitchFont(&lf);
}

void CGplapwView::SwitchFont(LPLOGFONT pLogFont)
{
    	CWaitCursor wait;
     m_font.DeleteObject();
	if (m_font.CreateFontIndirect(pLogFont)) {
		m_uFontHeight 	= abs(pLogFont->lfHeight);
		m_uTitleHeight	= m_uFontHeight + 2;

          SetFont(&m_font);
		m_wndLogTitleStatic.SetFont(&m_font);
		m_wndLogListBox.SetFont(&m_font);
		m_wndLogListBox.RecalculateItemHeight();

		//
		// Resize window.
		//
		CRect r;
		GetClientRect(r);
		m_wndLogTitleStatic.MoveWindow(BITMAP_WIDTH + BITMAP_SPACE, 0, r.right, m_uTitleHeight);
		m_wndLogListBox.MoveWindow(0, m_uTitleHeight, r.right, r.bottom - m_uTitleHeight);

		//
		// Save selected font in registry.
		//
		LOGFONT lf;
		m_font.GetObject(sizeof(LOGFONT), &lf);
		AfxGetApp()->WriteProfileString(szSectionKey, szFontNameKey, lf.lfFaceName); // TODO: Unicode? HELP!
		AfxGetApp()->WriteProfileInt(szSectionKey, szFontHeightKey, lf.lfHeight);
     }
}

void CGplapwView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CGplapwDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	ALL_RECORDS	rec;
	POSITION		pos;
     ULONG          fastest_race_lap    = 0UL;
     ULONG          leaders_race_time   = 0UL;
     BYTE           leaders_lap         = 0;
     ULONG          fastest_qual_lap    = 0UL;

    	CWaitCursor wait;

	//
	// Ensure list box is empty
	//
	m_wndLogListBox.ResetContent();
	m_wndLogListBox.ShowComputer(fShowComputer);

	//
	// Kludge to get around statics in LapUtl.cpp
	//
     LapSetAvgSpeed(fShowAvgSpeed, fAvgSpeedMph);
     LapSetElapsedMode(fElapsed);

	//
	// Walk list of log entries...
	//
	pos = pDoc->m_gplog.GetHeadPosition();
	while (pos) {
		rec = pDoc->m_gplog.GetNext(pos);

		if (fInitialiseDriverNames) {
			//
			// Add unique drivers name to driver filter lists.
			//
			if (rec.lr.lr_record_type == REC_TYPE_LAP) {
				CString	strCurrentDriverName(rec.lr.lr_driver);

				if (m_strlistIncludedDrivers.Find(strCurrentDriverName) == NULL) {
					m_strlistIncludedDrivers.AddTail(strCurrentDriverName);
				}
			}
		}

	  	//
		// Convert log entry to list box entry
		//
		LB_ENTRY	lbe;
	  	if (	LapEntryToLbEntry(	&lbe,
							&rec.lr,
	     					&fastest_race_lap,
	     					&leaders_race_time,
	     					&leaders_lap,
	     					&fastest_qual_lap
						)
		) {
			//
			// Only display record if filter settings are appropriate
			//
			if (IsFilter(&lbe)) {
				LB_ENTRY	*plbe = new LB_ENTRY(lbe);
				m_wndLogListBox.AddString((const TCHAR *) plbe);
			}
		}
	}

	fInitialiseDriverNames = FALSE;
}

void CGplapwView::ForceUpdate(void)
{
	CGplapwDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->UpdateAllViews(NULL); //shouldn't really use this as we haven't actually changed the document.
}

void CGplapwView::OnViewPractice()
{
	fShowPractice = !fShowPractice;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szPracticeKey, fShowPractice ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewPractice(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowPractice);
}

void CGplapwView::OnViewQualifying()
{
	fShowQualifying = !fShowQualifying;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szQualifyingKey, fShowQualifying ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewQualifying(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowQualifying);
}

void CGplapwView::OnViewRace()
{
	fShowRace = !fShowRace;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szRaceKey, fShowRace ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewRace(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowRace);
}

void CGplapwView::OnViewComputerscars()
{
	fShowComputer = !fShowComputer;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szComputerKey, fShowComputer ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewComputerscars(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowComputer);
}

void CGplapwView::OnViewPlayerscar()
{
	fShowPlayer = !fShowPlayer;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szPlayerKey, fShowPlayer ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewPlayerscar(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowPlayer);
}

void CGplapwView::OnViewLeader()
{
	fShowLeader = !fShowLeader;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szLeaderKey, fShowLeader ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewLeader(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowLeader);
}

void CGplapwView::OnViewFastestlaps()
{
	fShowFastest = !fShowFastest;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szFastestKey, fShowFastest ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewFastestlaps(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowFastest);
}

void CGplapwView::OnViewBestlaps()
{
	fShowBest = !fShowBest;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szBestKey, fShowBest ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewBestlaps(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowBest);
}

void CGplapwView::OnViewAuthenticationrecords()
{
	fShowAuth = !fShowAuth;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szAuthKey, fShowAuth ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewAuthenticationrecords(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowAuth);
}

void CGplapwView::OnViewDisplayaveragespeeds()
{
	fShowAvgSpeed = !fShowAvgSpeed;
     LapSetAvgSpeed(fShowAvgSpeed, fAvgSpeedMph);
	m_wndLogTitleStatic.SetWindowText(LapTitleText());
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szAvgSpeedKey, fShowAvgSpeed ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewDisplayaveragespeeds(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowAvgSpeed);
}

void CGplapwView::OnViewMphorkph()
{
	fAvgSpeedMph = !fAvgSpeedMph;
     LapSetAvgSpeed(fShowAvgSpeed, fAvgSpeedMph);
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szAvgSpeedMphKey, fAvgSpeedMph ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewMphorkph(CCmdUI* pCmdUI)
{
     pCmdUI->Enable(fShowAvgSpeed);
     pCmdUI->SetCheck(fAvgSpeedMph);
}

void CGplapwView::OnViewElapsedracetime()
{
	fElapsed = !fElapsed;
     LapSetElapsedMode(fElapsed);
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szElapsedKey, fElapsed ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewElapsedracetime(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fElapsed);
}

void CGplapwView::OnViewCarSetup()
{
	fShowSetup = !fShowSetup;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szCarSetupKey, fShowSetup ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewCarSetup(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowSetup);
}

void CGplapwView::OnViewPitStops()
{
	fShowPit = !fShowPit;
	ForceUpdate();
	VERIFY(AfxGetApp()->WriteProfileInt(szSectionKey, szPitKey, fShowPit ? INI_TRUE : INI_FALSE));
}

void CGplapwView::OnUpdateViewPitStops(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(fShowPit);
}

void CGplapwView::OnDraw(CDC* /*pDC*/)
{
	m_wndLogTitleStatic.Invalidate();
	m_wndLogListBox.Invalidate();
}

void CGplapwView::OnDriversDriverFilter()
{
	CDriverFilterDialog	dlg;
	dlg.m_pExcludedList = &m_strlistExcludedDrivers;
	dlg.m_pIncludedList = &m_strlistIncludedDrivers;

	if (dlg.DoModal() == IDOK) {
		fFilterDrivers = (m_strlistExcludedDrivers.GetCount() != 0);
		ForceUpdate();
	}
}

void CGplapwView::OnUpdateDriversDriverfilter(CCmdUI* pCmdUI)
{
     pCmdUI->SetCheck(m_strlistExcludedDrivers.GetCount() != 0);
}

void CGplapwView::OnInfoTotalLaps()
{
	UINT	practice_laps            = 0;
	UINT	free_practice_laps       = 0;
	UINT	qualifying_laps          = 0;
	UINT	pre_race_practice_laps   = 0;
	UINT	race_laps                = 0;
	UINT	total_laps               = 0;

	CGplapwDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

     ALL_RECORDS	rec;
	POSITION		pos;

	pos = pDoc->m_gplog.GetHeadPosition();
	while (pos) {
		rec = pDoc->m_gplog.GetNext(pos);

	     if ( rec.lr.lr_record_type == REC_TYPE_LAP &&
               IS_PLAYER(rec.lr.lr_car_number)
          ) {
               switch (GAME_MODE(rec.lr.lr_game_mode)) {
               case GM_PRACTICE:
                    ++practice_laps;
                    break;

               case GM_FREE_PRACTICE:
                    ++free_practice_laps;
                    break;

               case GM_QUALIFYING:
                    ++qualifying_laps;
                    break;

               case GM_PRE_RACE_PRACTICE:
                    ++pre_race_practice_laps;
                    break;

               case GM_RACE:
                    ++race_laps;
                    break;
               }
          }
          else if ( rec.pi.pi_record_type          	== REC_TYPE_PIT_IN  &&
                    GAME_MODE(rec.pi.pi_game_mode)	== GM_RACE          &&
                    IS_PLAYER(rec.pi.pi_car_number)
          ) {
               ++race_laps;
          }
	}
	total_laps =	practice_laps + free_practice_laps +
                    qualifying_laps + pre_race_practice_laps + race_laps;


	CTotalLapsDlg	dlg;

	dlg.m_szPractice.		Format(_T("%u"), practice_laps);
	dlg.m_szFreePractice.	Format(_T("%u"), free_practice_laps);
	dlg.m_szQualifying.		Format(_T("%u"), qualifying_laps);
	dlg.m_szPreRacePractice.	Format(_T("%u"), pre_race_practice_laps);
	dlg.m_szRace.			Format(_T("%u"), race_laps);
	dlg.m_szTotalLaps.		Format(_T("%u"), total_laps);
	dlg.DoModal();
}

BOOL CGplapwView::IsFilter(
     LB_ENTRY  *le                      /* In  List box entry              */
) {
     LAP_RECORD const    *lr            = &le->u.lr;
     BOOL                fAdd           = FALSE;
     BOOL                fDoComputer    = FALSE;
     BOOL                fDoPlayer      = FALSE;

	 fDoComputer    = fShowComputer && !IS_PLAYER(lr->lr_car_number);
	 fDoPlayer      = fShowPlayer   && IS_PLAYER(lr->lr_car_number);

     if (lr->lr_record_type == REC_TYPE_LAP || lr->lr_record_type == REC_TYPE_SPLIT) {

          if (IS_RACE(lr->lr_game_mode) && fShowRace) {
               if (fShowLeader && lr->lr_car_position == 1) {
                    fAdd = TRUE;
               }

               if ( (fShowFastest && le->fastest_race_lap) ||
                    (fShowBest    && lr->lr_lap_time == lr->lr_best_time)
               ) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }

               if (!fShowFastest && !fShowBest && (fDoComputer || fDoPlayer)) {
                    fAdd = TRUE;
               }
          }

          if (IS_QUALIFYING(lr->lr_game_mode) && fShowQualifying) {
               if (fShowLeader && lr->lr_qual_position == 1) {
                    fAdd = TRUE;
               }

               if ( (fShowFastest && le->fastest_qual_lap) ||
                    (fShowBest && lr->lr_lap_time == lr->lr_best_time)
               ) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }

               if (!fShowFastest && !fShowBest && (fDoComputer || fDoPlayer)) {
                    fAdd = TRUE;
               }
          }

          if ( (IS_PRACTICE(lr->lr_game_mode)          && fShowPractice)   ||
               (IS_FREE_PRACTICE(lr->lr_game_mode)     && fShowPractice)   ||
               (IS_PRE_RACE_PRACTICE(lr->lr_game_mode) && fShowPractice)
          ) {
               if ( (fShowFastest && le->fastest_qual_lap) ||
                    (fShowBest && lr->lr_lap_time == lr->lr_best_time)
               ) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }

               if (!fShowFastest && !fShowBest && (fDoComputer || fDoPlayer)) {
                    fAdd = TRUE;
               }
          }
     }
     else if (lr->lr_record_type == REC_TYPE_AUTH || lr->lr_record_type == REC_TYPE_AUTH_RACE) {
          if ( (IS_RACE(lr->lr_game_mode)              && fShowRace)       ||
               (IS_QUALIFYING(lr->lr_game_mode)        && fShowQualifying) ||
               (IS_PRACTICE(lr->lr_game_mode)          && fShowPractice)   ||
               (IS_FREE_PRACTICE(lr->lr_game_mode)     && fShowPractice)   ||
               (IS_PRE_RACE_PRACTICE(lr->lr_game_mode) && fShowPractice)
          ) {
               fAdd = fShowAuth;
          }
     }
     else if (lr->lr_record_type == REC_TYPE_SETUP) {
          if (fShowSetup) {
               if ( (IS_RACE(lr->lr_game_mode)              && fShowRace)       ||
                    (IS_QUALIFYING(lr->lr_game_mode)        && fShowQualifying) ||
                    (IS_PRACTICE(lr->lr_game_mode)          && fShowPractice)   ||
                    (IS_FREE_PRACTICE(lr->lr_game_mode)     && fShowPractice)   ||
                    (IS_PRE_RACE_PRACTICE(lr->lr_game_mode) && fShowPractice)
               ) {
				    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }
          }
     }
     else if (lr->lr_record_type == REC_TYPE_PIT_IN || lr->lr_record_type == REC_TYPE_PIT_OUT) {
          if (fShowPit && !fShowFastest && !fShowBest) {
               fDoComputer    = fShowComputer && !IS_PLAYER(lr->lr_car_number);
               fDoPlayer      = fShowPlayer   && IS_PLAYER(lr->lr_car_number);

               if (IS_RACE(lr->lr_game_mode) && fShowRace) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }

               if (IS_QUALIFYING(lr->lr_game_mode) && fShowQualifying) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }

               if ( (IS_PRACTICE(lr->lr_game_mode)          && fShowPractice)   ||
                    (IS_FREE_PRACTICE(lr->lr_game_mode)     && fShowPractice)   ||
                    (IS_PRE_RACE_PRACTICE(lr->lr_game_mode) && fShowPractice)
               ) {
                    fAdd = (fAdd || fDoComputer || fDoPlayer);
               }
          }
     }
     else {
          fAdd = TRUE;
     }

     if (fAdd && fFilterDrivers) {
          fAdd = FALSE;
          if (le->u.lr.lr_record_type == REC_TYPE_LAP) {
               if (m_strlistIncludedDrivers.Find((TCHAR *) le->u.lr.lr_driver)) {
                    fAdd = TRUE;
               }
          }
          else if (le->u.lr.lr_record_type == REC_TYPE_PIT_IN) {
               if (m_strlistIncludedDrivers.Find((TCHAR *) le->u.pi.pi_driver)) {
                    fAdd = TRUE;
               }
          }
          else if (le->u.lr.lr_record_type == REC_TYPE_PIT_OUT) {
               if (m_strlistIncludedDrivers.Find((TCHAR *) le->u.po.po_driver)) {
                    fAdd = TRUE;
               }
          }
          else {
               fAdd = TRUE;
          }
     }

     return fAdd;
}

void CGplapwView::OnEditCopy()
{
	LB_ENTRY	*le;
     TCHAR     buff[132];
     ULONG	len       = 0L;
     INT      	circuit   = -1;
	INT		count;
	INT		i;

    	CWaitCursor wait;

     /*
     ** Determine how much memory we'll need.
     */
     len += 1 + lstrlen(LapTitleText()) + 2;
     len += 1 + lstrlen(LapTitleUnderlineText()) + 2;

	count = m_wndLogListBox.GetCount();
	for (i = 0; i != LB_ERR && i < count; i++) {
		le = (LB_ENTRY *) m_wndLogListBox.GetItemData(i);

          if (IsFilter(le)) {
               if ( le->u.lr.lr_record_type == REC_TYPE_LAP       ||
                    le->u.lr.lr_record_type == REC_TYPE_AUTH      ||
                    le->u.lr.lr_record_type == REC_TYPE_AUTH_RACE ||
                    le->u.lr.lr_record_type == REC_TYPE_PIT_IN    ||
                    le->u.lr.lr_record_type == REC_TYPE_PIT_OUT
               ) {
                    if (circuit != CIRCUIT(le->u.lr.lr_game_mode)) {
                         circuit = CIRCUIT(le->u.lr.lr_game_mode);
                         len += 1 + lstrlen(LapCircuitText(circuit)) + 2;
                    }
               }

               LbEntryToText(buff, le);
               len += 1 + lstrlen(buff) + 2;
          }
    	}
     ++len;

	//
	// We are limited to 64kb for moveable global memory.
	//
     if (len < 0xfff0UL) {
          HGLOBAL   hMem = GlobalAlloc(GHND, SIZEOF_C(len));
          TCHAR     *szBuff;

          if (hMem) {
               szBuff = (TCHAR *) GlobalLock(hMem);
               lstrcpy(szBuff, _T(" "));
               lstrcat(szBuff, LapTitleText());
               lstrcat(szBuff, _T("\r\n"));
               lstrcat(szBuff, _T(" "));
               lstrcat(szBuff, LapTitleUnderlineText());
               lstrcat(szBuff, _T("\r\n"));

               circuit = -1;
			count = m_wndLogListBox.GetCount();
			for (i = 0; i != LB_ERR && i < count; i++) {
				le = (LB_ENTRY *) m_wndLogListBox.GetItemData(i);
                    if (IsFilter(le)) {
                         if ( le->u.lr.lr_record_type == REC_TYPE_LAP       ||
                              le->u.lr.lr_record_type == REC_TYPE_AUTH      ||
                              le->u.lr.lr_record_type == REC_TYPE_AUTH_RACE ||
                              le->u.lr.lr_record_type == REC_TYPE_PIT_IN    ||
                              le->u.lr.lr_record_type == REC_TYPE_PIT_OUT
                         ) {
                              if (circuit != CIRCUIT(le->u.lr.lr_game_mode)) {
                                   circuit = CIRCUIT(le->u.lr.lr_game_mode);
                                   lstrcat(szBuff, _T(" "));
                                   lstrcat(szBuff, LapCircuitText(circuit));
                                   lstrcat(szBuff, _T("\r\n"));
                              }
                         }

                         LbEntryToText(buff, le);
                         lstrcat(szBuff, _T(" "));
                         lstrcat(szBuff, buff);
                         lstrcat(szBuff, _T("\r\n"));
                    }
               }

               GlobalUnlock(hMem);

               OpenClipboard();
               EmptyClipboard();

               SetClipboardData(CF_TEXT, hMem);
               CloseClipboard();
          }
          else {
			CString	strMsg;
			VERIFY(strMsg.LoadString(IDS_COPY_NO_MEM));
               MessageBox((LPCTSTR) strMsg, AfxGetAppName(), MB_ICONEXCLAMATION);
          }
     }
     else {
		CString	strMsg;
		VERIFY(strMsg.LoadString(IDS_COPY_TOO_MUCH_DATA));
          MessageBox((LPCTSTR) strMsg, AfxGetAppName(), MB_ICONEXCLAMATION);
     }
}

void CGplapwView::NewCircuit(INT nCircuit) {
	CString	strTitle;
	CString	strFilename = GetDocument()->GetPathName();

	//
	// Strip pathname and extension in W95
	//
#ifdef UNICODE
	_wsplitpath((LPCTSTR) strFilename, NULL, NULL, strFilename.GetBuffer(FILENAME_MAX), NULL);
#else
	_splitpath((LPCTSTR) strFilename, NULL, NULL, strFilename.GetBuffer(FILENAME_MAX), NULL);
#endif
	strFilename.ReleaseBuffer();

	strTitle.Format("%s [%s]", (LPCTSTR) strFilename, LapCircuitText(nCircuit));
	GetDocument()->SetTitle(strTitle);
}

void CGplapwView::OnFileAuthenticationToTextFile()
{
     LB_ENTRY  *lbe;
     BOOL      fEntrySelected = FALSE;
     BOOL      fAppend        = FALSE;
     INT     	iSelection;

     iSelection = m_wndLogListBox.GetCurSel();
     if (iSelection != LB_ERR) {
		lbe = (LB_ENTRY *) m_wndLogListBox.GetItemData(iSelection);
          if (lbe && (lbe->u.ar.ar_record_type == REC_TYPE_AUTH || lbe->u.ar.ar_record_type == REC_TYPE_AUTH_RACE)) {
               fEntrySelected = TRUE;
          }
     }

     if (!fEntrySelected) {
		CString	strMsg;
		VERIFY(strMsg.LoadString(IDS_QUERY_WRITE_ALL_AUTH));
		if (MessageBox((LPCTSTR) strMsg, AfxGetAppName(), MB_ICONQUESTION | MB_YESNO) != IDYES) {
               return;
          }
     }

	CString	strFilter;
	VERIFY(strFilter.LoadString(IDS_SAVE_TXT_FILE_TEMPLATE));

     CFileDialog    dlgFileSaveAs( FALSE,
                                   _T(".txt"),
                                   _T("authlap.txt"),
                                   OFN_HIDEREADONLY,
                                   strFilter
                              );
     if (!dlgFileSaveAs.DoModal()) {
          return;
     }

     CString   strPathName;
     strPathName = dlgFileSaveAs.GetPathName();

     CFileStatus    file_status;
     if (CFile::GetStatus(strPathName, file_status)) {
          //
          // File already exists, append to it?
          //
		CString	strMsg;
		VERIFY(strMsg.LoadString(IDS_APPEND_TO_FILE_QUERY));
		if (MessageBox((LPCTSTR) strMsg, AfxGetAppName(), MB_ICONQUESTION | MB_YESNO) == IDYES) {
               fAppend = TRUE;
          }
     }

    	CWaitCursor    wait;
     TCHAR          szBuff[80 * 5];

     CFileException fe;
     CFile          file;
     try {
     	if (!file.Open(     strPathName,
     	                    fAppend ? (    CFile::modeReadWrite     |
     	                                   CFile::shareExclusive)
                                      : (    CFile::modeCreate        |
     	                                   CFile::modeReadWrite     |
     	                                   CFile::shareExclusive),
     	                    &fe)
          ) {
               ReportFileException(strPathName, &fe);
     		return;
          }
          else {
               if (fAppend) {
                    file.SeekToEnd();
               }

               if (fEntrySelected) {
                    LapCopyAuthRecord(szBuff, lbe);
                    file.Write(szBuff, lstrlen(szBuff));
               }
               else {
                    int  iCount;
                    int  i;

               	iCount = m_wndLogListBox.GetCount();
               	for (i = 0; i != LB_ERR && i < iCount; i++) {
               		lbe = (LB_ENTRY *) m_wndLogListBox.GetItemData(i);

                         if ( lbe->u.lr.lr_record_type == REC_TYPE_AUTH      ||
                              lbe->u.lr.lr_record_type == REC_TYPE_AUTH_RACE
                         ) {
                              LapCopyAuthRecord(szBuff, lbe);
                              file.Write(szBuff, lstrlen(szBuff));
                         }
                    }
               }
               file.Close();
          }
     }
	catch (CFileException *e)
	{
          ReportFileException(strPathName, e);
		return;
	}
}

void CGplapwView::OnFileAuthenticationToClipboard()
{
     LB_ENTRY  *le;
     LONG      len             = 80 * 5;
     BOOL      write_specified = FALSE;
     INT     	iSelection;

     iSelection = m_wndLogListBox.GetCurSel();
     if (iSelection != LB_ERR) {
		le = (LB_ENTRY *) m_wndLogListBox.GetItemData(iSelection);
          if (le && (le->u.ar.ar_record_type == REC_TYPE_AUTH || le->u.ar.ar_record_type == REC_TYPE_AUTH_RACE)) {
               write_specified = TRUE;
          }
     }

     if (!write_specified) {
		CString	strMsg;
		VERIFY(strMsg.LoadString(IDS_AUTH_NO_SELECTED_ENTRY));
		MessageBox((LPCTSTR) strMsg, AfxGetAppName(), MB_ICONINFORMATION);
     }
     else {
	    	CWaitCursor wait;

          HGLOBAL   hMem = GlobalAlloc(GHND, SIZEOF_C(len));
          TCHAR     *szBuff;

          if (hMem) {
               szBuff = (TCHAR *) GlobalLock(hMem);

               LapCopyAuthRecord(szBuff, le);

               GlobalUnlock(hMem);

               OpenClipboard();
               EmptyClipboard();

               SetClipboardData(CF_TEXT, hMem);
               CloseClipboard();
          }
          else {
			CString	strMsg;
			VERIFY(strMsg.LoadString(IDS_COPY_NO_MEM));
               MessageBox((LPCTSTR) strMsg, AfxGetAppName(), MB_ICONEXCLAMATION);
          }
     }
}

BOOL CGplapwView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//
	// Trap a double click of our log listbox.
	//
	if (HIWORD(wParam) == LBN_DBLCLK && (HWND) lParam == m_wndLogListBox.m_hWnd) {
		INT	iSelection = m_wndLogListBox.GetCurSel();
		if (iSelection != LB_ERR) {
			LB_ENTRY *lbe;

			lbe = (LB_ENTRY *) m_wndLogListBox.GetItemData(iSelection);
			ASSERT(lbe != NULL);
			if (	lbe->u.ar.ar_record_type == REC_TYPE_AUTH		||
				lbe->u.ar.ar_record_type == REC_TYPE_AUTH_RACE
			) {
				OnFileAuthenticationToClipboard();
			}
			else {
				CString	strMsg;
				VERIFY(strMsg.LoadString(IDS_TIP_DBLCLICK_AUTH_ENTRIES));
				MessageBox((LPCTSTR) strMsg, AfxGetAppName(), MB_ICONINFORMATION);
			}
		}
		return TRUE;
	}
	else {
		return CView::OnCommand(wParam, lParam);
	}
}

void CGplapwView::OnFileSaveAs()
{
	CString	strFilter;
	VERIFY(strFilter.LoadString(IDS_SAVE_FILES_TEMPLATE));

     CFileDialog    dlgFileSaveAs( FALSE,
                                   NULL,
                                   NULL,
                                   OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                                   strFilter
                              );
     if (!dlgFileSaveAs.DoModal()) {
          return;
     }

     CString   strPathName;
     CString   strExt;
     strPathName = dlgFileSaveAs.GetPathName();

#ifdef UNICODE
	_wsplitpath((LPCTSTR) strPathName, NULL, NULL, NULL, strExt.GetBuffer(FILENAME_MAX));
#else
	_splitpath((LPCTSTR) strPathName, NULL, NULL, NULL, strExt.GetBuffer(FILENAME_MAX));
#endif
	strExt.ReleaseBuffer();

     BOOL fText = FALSE;
     strExt.MakeLower();
     if (strExt == CString(_T(".txt"))) {
          fText = TRUE;
     }
     else if (strExt != CString(_T(".log"))) {
		CString	strMsg;
		VERIFY(strMsg.LoadString(IDS_SAVE_AS_LOG_QUERY));
          switch (MessageBox(strMsg, AfxGetAppName(), MB_ICONQUESTION | MB_YESNOCANCEL)) {
          case IDYES:
               fText = FALSE;
               break;

          case IDNO:
               fText = TRUE;
               break;

          default:
          case IDCANCEL:
               return;
          }
     }

    	CWaitCursor wait;

     CFileException fe;
     CFile          file;
     try {
     	if (!file.Open(     strPathName,
     	                    CFile::modeCreate        |
     	                    CFile::modeReadWrite     |
     	                    CFile::shareExclusive,
     	                    &fe)
          ) {
               ReportFileException(strPathName, &fe);
     		return;
          }
          else {
               LB_ENTRY  *lbe;
               CString   strBuff;
               int       iCount;
               int       i;
               int       circuit;

               if (fText) {
                    strBuff = _T(" ");
                    strBuff += LapTitleText();
                    strBuff += _T("\r\n");
                    strBuff += _T(" ");
                    strBuff += LapTitleUnderlineText();
                    strBuff += _T("\r\n");
                    file.Write((LPCTSTR) strBuff, strBuff.GetLength());
               }

               circuit = -1;
          	iCount = m_wndLogListBox.GetCount();
          	for (i = 0; i != LB_ERR && i < iCount; i++) {
          		lbe = (LB_ENTRY *) m_wndLogListBox.GetItemData(i);

                    if (IsFilter(lbe)) {
                         if (!fText) {
                              file.Write(&lbe->u, sizeof(lbe->u));
                         }
                         else {
                              TCHAR     buff[132];

                              strBuff = _T("");
                              if ( lbe->u.lr.lr_record_type == REC_TYPE_LAP       ||
                                   lbe->u.lr.lr_record_type == REC_TYPE_AUTH      ||
                                   lbe->u.lr.lr_record_type == REC_TYPE_AUTH_RACE ||
                                   lbe->u.lr.lr_record_type == REC_TYPE_PIT_IN    ||
                                   lbe->u.lr.lr_record_type == REC_TYPE_PIT_OUT
                              ) {
                                   if (circuit != CIRCUIT(lbe->u.lr.lr_game_mode)) {
                                        circuit = CIRCUIT(lbe->u.lr.lr_game_mode);
                                        strBuff += _T(" ");
                                        strBuff += LapCircuitText(circuit);
                                        strBuff += _T("\r\n");
                                   }
                              }

                              LbEntryToText(buff, lbe);
                              strBuff += _T(" ");
                              strBuff += buff;
                              strBuff += _T("\r\n");
                              file.Write((LPCTSTR) strBuff, strBuff.GetLength());
                         }
                    }
               }
               file.Close();
          }
     }
	catch (CFileException *e)
	{
          ReportFileException(strPathName, e);
		return;
	}
}

void CGplapwView::ReportFileException(CString& strFilename, CFileException *e)
{
     TRACE2("Reporting file I/O exception for %s with lOsError = $%lX.\n", strFilename, e->m_lOsError);

     UINT nIDP = AFX_IDP_FAILED_IO_ERROR_WRITE;
     switch (e->m_cause) {
     case CFileException::fileNotFound:
     case CFileException::badPath:
          nIDP = AFX_IDP_FAILED_INVALID_PATH;
          break;
     case CFileException::diskFull:
          nIDP = AFX_IDP_FAILED_DISK_FULL;
          break;
     case CFileException::accessDenied:
          nIDP = AFX_IDP_FAILED_ACCESS_WRITE;
          break;

     case CFileException::badSeek:
     case CFileException::generic:
     case CFileException::tooManyOpenFiles:
     case CFileException::invalidFile:
     case CFileException::hardIO:
     case CFileException::directoryFull:
          nIDP = AFX_IDP_FAILED_IO_ERROR_WRITE;
          break;
     }

     CString prompt;
     AfxFormatString1(prompt, nIDP, (LPCTSTR) strFilename);
     AfxMessageBox(prompt, MB_ICONEXCLAMATION, nIDP);
}
