// aboutbox.h : header file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef _AboutBox_h
#define _AboutBox_h

/////////////////////////////////////////////////////////////////////////////
// CBigIcon window

class CBigIcon : public CButton
{
// Attributes
public:

// Operations
public:
     void SizeToContent();

// Implementation
protected:
     virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

     //{{AFX_MSG(CBigIcon)
     afx_msg BOOL OnEraseBkgnd(CDC* pDC);
     //}}AFX_MSG
     DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSplashWnd dialog

class CSplashWnd : public CDialog
{
// Construction
public:
     BOOL Create(CWnd* pParent);

// Dialog Data
     //{{AFX_DATA(CSplashWnd)
     enum { IDD = IDD_SPLASH };
          // NOTE: the ClassWizard will add data members here
     //}}AFX_DATA

// Implementation
protected:
     virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

     CBigIcon m_icon; // self-draw button
     CFont m_font;   // light version of dialog font

     // Generated message map functions
     //{{AFX_MSG(CSplashWnd)
     virtual BOOL OnInitDialog();
     //}}AFX_MSG
     DECLARE_MESSAGE_MAP()

private:
	CBitmapButton	bitbtnPicture1;
};

#endif // _AboutBox_h
/////////////////////////////////////////////////////////////////////////////
