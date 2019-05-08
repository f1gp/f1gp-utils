// drivfilt.cpp : implementation file
//

#include "stdafx.h"
#include "gplapw.h"
#include "drivfilt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDriverFilterDialog dialog


CDriverFilterDialog::CDriverFilterDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDriverFilterDialog::IDD, pParent)
{
	m_pExcludedList = NULL;
	m_pIncludedList = NULL;

	//{{AFX_DATA_INIT(CDriverFilterDialog)
	//}}AFX_DATA_INIT
}


void CDriverFilterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDriverFilterDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDriverFilterDialog, CDialog)
	//{{AFX_MSG_MAP(CDriverFilterDialog)
	ON_BN_CLICKED(IDC_DF_ALL, OnDfAll)
	ON_BN_CLICKED(IDC_DF_NONE, OnDfNone)
	ON_BN_CLICKED(IDC_DF_EXCLUDE, OnDfExclude)
	ON_BN_CLICKED(IDC_DF_INCLUDE, OnDfInclude)
	ON_LBN_DBLCLK(IDC_DF_LIST_INCLUDED, OnDblclkDfListIncluded)
	ON_LBN_DBLCLK(IDC_DF_LIST_EXCLUDED, OnDblclkDfListExcluded)
	ON_LBN_SELCHANGE(IDC_DF_LIST_EXCLUDED, OnSelchangeDfListExcluded)
	ON_LBN_SELCHANGE(IDC_DF_LIST_INCLUDED, OnSelchangeDfListIncluded)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDriverFilterDialog message handlers

BOOL CDriverFilterDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	POSITION	pos;	

	ASSERT(m_pExcludedList != NULL);
	if (m_pExcludedList != NULL) {
		for (pos = m_pExcludedList->GetHeadPosition(); pos != NULL; ) {
			ExcludedList().AddString(m_pExcludedList->GetNext(pos));
		}
	}
		
	ASSERT(m_pIncludedList != NULL);
	if (m_pIncludedList != NULL) {
		for (pos = m_pIncludedList->GetHeadPosition(); pos != NULL; ) {
			IncludedList().AddString(m_pIncludedList->GetNext(pos));
		}
	}

	UpdateButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDriverFilterDialog::OnDfAll() 
{	
	INT	i;

	for (i = 0; i < ExcludedList().GetCount(); i++) {
		CString	szName;
		
		ExcludedList().GetText(i, szName);
 		IncludedList().AddString(szName);
	}
	ExcludedList().ResetContent();
	UpdateButtons();
}

void CDriverFilterDialog::OnDfNone() 
{
	INT	i;

	for (i = 0; i < IncludedList().GetCount(); i++) {
		CString	szName;
		
		IncludedList().GetText(i, szName);
 		ExcludedList().AddString(szName);
	}
	IncludedList().ResetContent();
	UpdateButtons();
}

void CDriverFilterDialog::OnDfExclude() 
{
	INT	i;

	i = IncludedList().GetCurSel();
	if (i != LB_ERR) {
		CString	szName;
		
		IncludedList().GetText(i, szName);
		IncludedList().DeleteString(i);
 		ExcludedList().AddString(szName);
		UpdateButtons();
	}
}

void CDriverFilterDialog::OnDfInclude() 
{
	INT	i;

	i = ExcludedList().GetCurSel();
	if (i != LB_ERR) {
		CString	szName;
		
		ExcludedList().GetText(i, szName);
		ExcludedList().DeleteString(i);
 		IncludedList().AddString(szName);
		UpdateButtons();
	}
}

void CDriverFilterDialog::OnDblclkDfListIncluded() 
{
	OnDfExclude();
}

void CDriverFilterDialog::OnDblclkDfListExcluded() 
{
	OnDfInclude();
}

void CDriverFilterDialog::OnSelchangeDfListExcluded() 
{
	UpdateButtons();
}

void CDriverFilterDialog::OnSelchangeDfListIncluded() 
{
	UpdateButtons();
}

void CDriverFilterDialog::OnOK() 
{
	m_pExcludedList->RemoveAll();
	m_pIncludedList->RemoveAll();

	INT		i;
	CString	szName;

	for (i = 0; i < ExcludedList().GetCount(); i++) {
		ExcludedList().GetText(i, szName);
		m_pExcludedList->AddTail(szName);
	}

	for (i = 0; i < IncludedList().GetCount(); i++) {
		IncludedList().GetText(i, szName);
		m_pIncludedList->AddTail(szName);
	}
		
	CDialog::OnOK();
}

void CDriverFilterDialog::UpdateButtons()
{
	ExcludeButton().EnableWindow(IncludedList().GetCurSel() != LB_ERR);
	IncludeButton().EnableWindow(ExcludedList().GetCurSel() != LB_ERR);

	NoneButton().EnableWindow(IncludedList().GetCount() != 0);
	AllButton().EnableWindow(ExcludedList().GetCount() != 0);

	OkButton().EnableWindow(IncludedList().GetCount() != 0);
}
