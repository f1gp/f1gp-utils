// GpLapdoc.h : interface of the CGplapwDoc class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _gplapdoc_h_
#define _gplapdoc_h_

#include <afxtempl.h>
#include "LapUtl.h"

class CGplapwDoc : public CDocument
{
protected: // create from serialization only
	CGplapwDoc();
	DECLARE_DYNCREATE(CGplapwDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGplapwDoc)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGplapwDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGplapwDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CList <ALL_RECORDS, ALL_RECORDS&> m_gplog;
};

#endif // _gplapdoc_h_
/////////////////////////////////////////////////////////////////////////////
