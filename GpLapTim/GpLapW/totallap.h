// totallap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTotalLapsDlg dialog

class CTotalLapsDlg : public CDialog
{
// Construction
public:
	CTotalLapsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTotalLapsDlg)
	enum { IDD = IDD_TOTAL_LAPS };
	CString	m_szFreePractice;
	CString	m_szPractice;
	CString	m_szPreRacePractice;
	CString	m_szQualifying;
	CString	m_szRace;
	CString	m_szTotalLaps;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTotalLapsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTotalLapsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
