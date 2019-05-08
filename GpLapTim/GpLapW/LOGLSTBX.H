/*
** File   : loglstbx.h
** Author : TK
** Date   : 11/11/95
**
** $Header$
**
**
*/

#ifndef _loglstbx_h_
#define _loglstbx_h_

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

//
// Listbox bitmap size.
//
#define BITMAP_HEIGHT    16
#define BITMAP_WIDTH     16
#define BITMAP_SPACE     2

/*---------------------------------------------------------------------------
** Typedefs
*/

/////////////////////////////////////////////////////////////////////////////
// CLogListBox window

class CGplapwView;

class CLogListBox : public CListBox
{
// Construction
public:
	CLogListBox();
	void	SetParentsView(CGplapwView *pviewParentsView);

// Attributes
public:

// Operations
public:
	void	ShowComputer(BOOL fShow = TRUE)	{ fShowComputer = fShow; }
	void RecalculateItemHeight();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogListBox)
	public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLogListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLogListBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	void 		DrawBitmap(CDC *pDC, UINT idr_bm, UINT idr_bm_mask, RECT *prect);
	LONG 		DetermineItemHeight(LONG lItemHeight = 0);

	INT			nCurrentFontHeight;
	BOOL			fShowComputer;
	BOOL			fCircuit;
	INT			nCircuit;
	CGplapwView	*pviewParent;
};

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/


#endif    // _loglstbx_h_
/*---------------------------------------------------------------------------
** End of File
*/
