// totallap.cpp : implementation file
//

#include "stdafx.h"
#include "gplapw.h"
#include "totallap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTotalLapsDlg dialog


CTotalLapsDlg::CTotalLapsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTotalLapsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTotalLapsDlg)
	m_szFreePractice = _T("");
	m_szPractice = _T("");
	m_szPreRacePractice = _T("");
	m_szQualifying = _T("");
	m_szRace = _T("");
	m_szTotalLaps = _T("");
	//}}AFX_DATA_INIT
}


void CTotalLapsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTotalLapsDlg)
	DDX_Text(pDX, IDC_TL_FREE_PRACTICE, m_szFreePractice);
	DDX_Text(pDX, IDC_TL_PRACTICE, m_szPractice);
	DDX_Text(pDX, IDC_TL_PRE_RACE_PRACTICE, m_szPreRacePractice);
	DDX_Text(pDX, IDC_TL_QUALIFYING, m_szQualifying);
	DDX_Text(pDX, IDC_TL_RACE, m_szRace);
	DDX_Text(pDX, IDC_TL_TOTAL_LAPS, m_szTotalLaps);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTotalLapsDlg, CDialog)
	//{{AFX_MSG_MAP(CTotalLapsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTotalLapsDlg message handlers
