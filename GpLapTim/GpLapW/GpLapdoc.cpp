// GpLapdoc.cpp : implementation of the CGplapwDoc class
//

#include "stdafx.h"
#include "GpLapW.h"

#include "GpLapdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGplapwDoc

IMPLEMENT_DYNCREATE(CGplapwDoc, CDocument)

BEGIN_MESSAGE_MAP(CGplapwDoc, CDocument)
	//{{AFX_MSG_MAP(CGplapwDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGplapwDoc construction/destruction

CGplapwDoc::CGplapwDoc()
{
}

CGplapwDoc::~CGplapwDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGplapwDoc serialization

void CGplapwDoc::Serialize(CArchive& ar)
{
	ALL_RECORDS	rec;
	ASSERT(sizeof(rec) == 44);

	if (ar.IsStoring())
	{
		//
		// Save all log
		//
          POSITION  pos;

	     pos = m_gplog.GetHeadPosition();
	     while (pos) {
		     rec = m_gplog.GetNext(pos);
		     ar.Write(&rec, sizeof(rec));
		}
	}
	else
	{
		//
		// Load log
		//
		while (ar.Read(&rec, sizeof(rec)) == sizeof(rec)) {
			m_gplog.AddTail(rec);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGplapwDoc diagnostics

#ifdef _DEBUG
void CGplapwDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGplapwDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGplapwDoc commands
