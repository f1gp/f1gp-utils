// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "GpLapW.h"

#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Registry keys.

static TCHAR BASED_CODE 	szSectionKey[]		= _T("Settings");
static TCHAR BASED_CODE 	szPosLeftKey[]		= _T("Pos Left");
static TCHAR BASED_CODE 	szPosRightKey[]	= _T("Pos Right");
static TCHAR BASED_CODE 	szPosTopKey[]		= _T("Pos Top");
static TCHAR BASED_CODE 	szPosBottomKey[]	= _T("Pos Bottom");

static TCHAR BASED_CODE  szToolBarKey[]		= _T("Tool Bar");
static TCHAR BASED_CODE  szStatusBarKey[]	= _T("Status Bar");

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars
	
// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_OPEN,
	ID_FILE_SAVE_AS,
		ID_SEPARATOR,
	ID_FILE_PRINT,
		ID_SEPARATOR,
	ID_EDIT_COPY,
		ID_SEPARATOR,
	ID_VIEW_PRACTICE,
	ID_VIEW_QUALIFYING,
	ID_VIEW_RACE,
		ID_SEPARATOR,
	ID_VIEW_COMPUTERSCARS,
	ID_VIEW_PLAYERSCAR,
	ID_VIEW_LEADER,
		ID_SEPARATOR,
	ID_VIEW_FASTESTLAPS,
	ID_VIEW_BESTLAPS,
	ID_VIEW_AUTHENTICATIONRECORDS,
		ID_SEPARATOR,
	ID_VIEW_ELAPSEDRACETIME,
		ID_SEPARATOR,
	ID_VIEW_CARSETUP,
	ID_VIEW_PITSTOPS,
		ID_SEPARATOR,
	ID_DRIVERS_DRIVERFILTER,
	ID_INFO_TOTALLAPS,
		ID_SEPARATOR,
	ID_APP_ABOUT,
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
		
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		  sizeof(buttons)/sizeof(UINT)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	//
	// Setup dockable toolbar
	//
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	//
	// Enable tool tips
	//
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

     BOOL fVisible;
     fVisible = AfxGetApp()->GetProfileInt(szSectionKey, szToolBarKey, TRUE); 
     m_wndToolBar.ShowWindow(fVisible ? SW_SHOW : SW_HIDE);       

     fVisible = AfxGetApp()->GetProfileInt(szSectionKey, szStatusBarKey, TRUE); 
     m_wndStatusBar.ShowWindow(fVisible ? SW_SHOW : SW_HIDE);       

	RestorePreviousWindowPostion();

	return 0;
}

BOOL CMainFrame::DestroyWindow() 
{
	SaveCurrentWindowPostion();	
	return CMDIFrameWnd::DestroyWindow();
}

void CMainFrame::OnClose() 
{
     AfxGetApp()->WriteProfileInt(szSectionKey, szToolBarKey, m_wndToolBar.IsWindowVisible()); 
     AfxGetApp()->WriteProfileInt(szSectionKey, szStatusBarKey, m_wndStatusBar.IsWindowVisible()); 
	
	CMDIFrameWnd::OnClose();
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::RestorePreviousWindowPostion()
{
	WINDOWPLACEMENT	wpPos;

	//
	// Get current window settings and restore our normal window positions.
	// Note we set the style to hidden when the perform the update.
	//
	GetWindowPlacement(&wpPos);

	wpPos.rcNormalPosition.left	= AfxGetApp()->GetProfileInt(szSectionKey, szPosLeftKey, 	wpPos.rcNormalPosition.left);
	wpPos.rcNormalPosition.right	= AfxGetApp()->GetProfileInt(szSectionKey, szPosRightKey, 	wpPos.rcNormalPosition.right);
	wpPos.rcNormalPosition.top	= AfxGetApp()->GetProfileInt(szSectionKey, szPosTopKey, 	wpPos.rcNormalPosition.top);
	wpPos.rcNormalPosition.bottom	= AfxGetApp()->GetProfileInt(szSectionKey, szPosBottomKey,	wpPos.rcNormalPosition.bottom);
	wpPos.showCmd			     = SW_HIDE;

	SetWindowPlacement(&wpPos);
}

void	CMainFrame::SaveCurrentWindowPostion()
{
	WINDOWPLACEMENT	wpPos;

	GetWindowPlacement(&wpPos);

	AfxGetApp()->WriteProfileInt(szSectionKey, szPosLeftKey, 	wpPos.rcNormalPosition.left);
	AfxGetApp()->WriteProfileInt(szSectionKey, szPosRightKey,	wpPos.rcNormalPosition.right);
	AfxGetApp()->WriteProfileInt(szSectionKey, szPosTopKey,	wpPos.rcNormalPosition.top);
	AfxGetApp()->WriteProfileInt(szSectionKey, szPosBottomKey,	wpPos.rcNormalPosition.bottom);
}

