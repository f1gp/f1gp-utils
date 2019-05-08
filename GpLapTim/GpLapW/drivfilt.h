// drivfilt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDriverFilterDialog dialog

class CDriverFilterDialog : public CDialog
{
// Construction
public:
	CDriverFilterDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	CStringList	*m_pExcludedList;
	CStringList	*m_pIncludedList;

	//{{AFX_DATA(CDriverFilterDialog)
	enum { IDD = IDD_DRIVER_FILTER };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriverFilterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
     CListBox& ExcludedList()	{return *(CListBox*)GetDlgItem(IDC_DF_LIST_EXCLUDED); }
     CListBox& IncludedList()	{return *(CListBox*)GetDlgItem(IDC_DF_LIST_INCLUDED); }

     CButton& ExcludeButton()	{return *(CButton*)GetDlgItem(IDC_DF_EXCLUDE); }
     CButton& IncludeButton()	{return *(CButton*)GetDlgItem(IDC_DF_INCLUDE); }
     CButton& NoneButton()	{return *(CButton*)GetDlgItem(IDC_DF_NONE); }
     CButton& AllButton()	{return *(CButton*)GetDlgItem(IDC_DF_ALL); }
     CButton& OkButton()		{return *(CButton*)GetDlgItem(IDOK); }

	void UpdateButtons();

	// Generated message map functions
	//{{AFX_MSG(CDriverFilterDialog)
	afx_msg void OnDfAll();
	afx_msg void OnDfNone();
	afx_msg void OnDfExclude();
	afx_msg void OnDfInclude();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkDfListIncluded();
	afx_msg void OnDblclkDfListExcluded();
	virtual void OnOK();
	afx_msg void OnSelchangeDfListExcluded();
	afx_msg void OnSelchangeDfListIncluded();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
