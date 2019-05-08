// GpLapW.h : main header file for the GPLAPW application
//

#ifndef _gplapw_h_
#define _gplapw_h_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "AboutBox.h"

//
// Number of bytes required for a character, depends if Unicode is selected.
//
#define   SIZEOF_C(n)         (sizeof(TCHAR) * (n))

/////////////////////////////////////////////////////////////////////////////
// CGpLapWApp:
// See GpLapW.cpp for the implementation of this class
//

class CGpLapWApp : public CWinApp
{
public:
	CGpLapWApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGpLapWApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGpLapWApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
     CSplashWnd	m_splash;
     DWORD		m_dwSplashTime;
};

#endif // _gplapw_h_
/////////////////////////////////////////////////////////////////////////////
