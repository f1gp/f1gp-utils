// GpLapW.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GpLapW.h"

#include "mainfrm.h"
#include "GpLapFrm.h"
#include "GpLapdoc.h"
#include "GpLapvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGpLapWApp

BEGIN_MESSAGE_MAP(CGpLapWApp, CWinApp)
	//{{AFX_MSG_MAP(CGpLapWApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
//	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGpLapWApp construction

CGpLapWApp::CGpLapWApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGpLapWApp object

CGpLapWApp theApp;

static TCHAR BASED_CODE szRegistryKey[] 	= _T("Grand Prix Log Viewer");

/////////////////////////////////////////////////////////////////////////////
// CGpLapWApp initialization

BOOL CGpLapWApp::InitInstance()
{
	Enable3dControls();

     //
     // Use the registry rather than an INI file.
     //
	SetRegistryKey(szRegistryKey);
	LoadStdProfileSettings();

     //
	// Register the application's document templates.  Document templates
	// serve as the connection between documents, frame windows and views.
     //
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(   IDR_GPLAPWTYPE,
                                             RUNTIME_CLASS(CGplapwDoc),
                                             RUNTIME_CLASS(CGpLapFrame),
                                             RUNTIME_CLASS(CGplapwView));
	AddDocTemplate(pDocTemplate);

     //
     // Register our document extension types (note must be done after
     // the above AddDocTemplate().
     //
     RegisterShellFileTypes();

     //
     // Now register with the shell that it can use the open invoke method.
     //
     EnableShellOpen();

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

     if (m_nCmdShow != SW_HIDE) {
          if (!m_pMainWnd->IsIconic() && m_splash.Create(m_pMainWnd)) {
               m_splash.ShowWindow(SW_SHOW);
               m_splash.UpdateWindow();
          }
          m_dwSplashTime = ::GetCurrentTime();
     }

     //
     // Note we don't use m_lpCmdLine as it doesn't support Unicode.
     //
     LPTSTR    pszCmdLine = GetCommandLine();

     while (*pszCmdLine && !isspace(*pszCmdLine)) {    // TODO: isspace UNICODE?
          ++pszCmdLine;
     }

     while (*pszCmdLine && isspace(*pszCmdLine)) {    // TODO: isspace UNICODE?
          ++pszCmdLine;
     }

     if (lstrcmpi(pszCmdLine, _T("/u")) == 0 || lstrcmpi(pszCmdLine, _T("-u")) == 0) {
          //
          // Uninstall
          //
          // TODO: Uninstall registry stuff.
     }
     else if (lstrcmpi(pszCmdLine, _T("")) != 0) {
          //
          // Filename?
          //
          OpenDocumentFile(pszCmdLine);
     }

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGpLapWApp commands


BOOL CGpLapWApp::OnIdle(LONG lCount)
{
	BOOL bResult = CWinApp::OnIdle(lCount);

	if (m_splash.m_hWnd != NULL) {
          if (::GetCurrentTime() - m_dwSplashTime > 2000) {
               // timeout expired, destroy the splash window
               m_splash.DestroyWindow();
               m_pMainWnd->UpdateWindow();
          }
          else {
			// check again later...
			bResult = TRUE;
          }
     }

	return bResult;
}

// App command to run the dialog
void CGpLapWApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
