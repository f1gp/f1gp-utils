// GpLapvw.h : interface of the CGplapwView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _gplapvw_h_
#define _gplapvw_h_

#include "gplapdoc.h"
#include "loglstbx.h"

class CLogListBox;

class CGplapwView : public CView
{
protected: // create from serialization only
	CGplapwView();
	DECLARE_DYNCREATE(CGplapwView)

// Attributes
public:
	CGplapwDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGplapwView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
     virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGplapwView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void 		NewCircuit(INT nCircuit);

private:
	BOOL			IsFilter(LB_ENTRY *le);
	void			ForceUpdate(void);
	void			SwitchFont(LPLOGFONT pLogFont);
     void           ReportFileException(CString& pszFilename, CFileException *e);
				
	CStatic		m_wndLogTitleStatic;
	CLogListBox	m_wndLogListBox;
	CFont		m_font;
	UINT			m_uFontHeight;
	UINT			m_uTitleHeight;
     UINT           m_uPrintHeaderBlankLines;
     UINT           m_uPrintFooterBlankLines;
     UINT           m_uPrintHeaderLines;
     UINT           m_uPrintFooterLines;
     				
     BOOL 		fShowPractice;
     BOOL 		fShowQualifying;
     BOOL 		fShowRace;
     BOOL 		fShowComputer;
     BOOL 		fShowLeader;
     BOOL 		fShowPlayer;
     BOOL 		fShowAuth;
     BOOL 		fShowFastest;
     BOOL 		fShowBest;
     BOOL 		fShowAvgSpeed;
     BOOL 		fAvgSpeedMph;
     BOOL 		fElapsed;
     BOOL 		fShowSetup;
     BOOL 		fShowPit;
     BOOL 		fFilterDrivers;

	CStringList	m_strlistExcludedDrivers;
	CStringList	m_strlistIncludedDrivers;
	BOOL			fInitialiseDriverNames;

// Generated message map functions
protected:
	//{{AFX_MSG(CGplapwView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFontSelect();
	afx_msg void OnUpdateViewPractice(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewQualifying(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewRace(CCmdUI* pCmdUI);
	afx_msg void OnViewRace();
	afx_msg void OnViewPractice();
	afx_msg void OnViewQualifying();
	afx_msg void OnViewComputerscars();
	afx_msg void OnUpdateViewComputerscars(CCmdUI* pCmdUI);
	afx_msg void OnViewPlayerscar();
	afx_msg void OnUpdateViewPlayerscar(CCmdUI* pCmdUI);
	afx_msg void OnViewLeader();
	afx_msg void OnUpdateViewLeader(CCmdUI* pCmdUI);
	afx_msg void OnViewFastestlaps();
	afx_msg void OnUpdateViewFastestlaps(CCmdUI* pCmdUI);
	afx_msg void OnViewBestlaps();
	afx_msg void OnUpdateViewBestlaps(CCmdUI* pCmdUI);
	afx_msg void OnViewAuthenticationrecords();
	afx_msg void OnUpdateViewAuthenticationrecords(CCmdUI* pCmdUI);
	afx_msg void OnViewDisplayaveragespeeds();
	afx_msg void OnUpdateViewDisplayaveragespeeds(CCmdUI* pCmdUI);
	afx_msg void OnViewMphorkph();
	afx_msg void OnUpdateViewMphorkph(CCmdUI* pCmdUI);
	afx_msg void OnViewElapsedracetime();
	afx_msg void OnUpdateViewElapsedracetime(CCmdUI* pCmdUI);
	afx_msg void OnViewCarSetup();
	afx_msg void OnUpdateViewCarSetup(CCmdUI* pCmdUI);
	afx_msg void OnViewPitStops();
	afx_msg void OnUpdateViewPitStops(CCmdUI* pCmdUI);
	afx_msg void OnDriversDriverFilter();
	afx_msg void OnUpdateDriversDriverfilter(CCmdUI* pCmdUI);
	afx_msg void OnInfoTotalLaps();
	afx_msg void OnEditCopy();
	afx_msg void OnFontRestoreDefault();
	afx_msg void OnFileAuthenticationToClipboard();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileAuthenticationToTextFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GpLapvw.cpp
inline CGplapwDoc* CGplapwView::GetDocument()
   { return (CGplapwDoc*)m_pDocument; }
#endif

#endif // _gplapvw_h_
/////////////////////////////////////////////////////////////////////////////
