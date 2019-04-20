/*
** File   : toolbar.hpp
** Author : TK 
** Date   : 13/02/94
**
** Tool Bar
*/

#ifndef   toolbar_hpp
#define   toolbar_hpp

/*---------------------------------------------------------------------------
** Includes
*/

#include <_cppwin_.hpp>
#include <cppwin.hpp>

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*
** Styles
*/
#define TBSTYLE_BUTTON          0x00
#define TBSTYLE_SEP             0x01
#define TBSTYLE_CHECK           0x02
#define TBSTYLE_GROUP           0x04
#define TBSTYLE_CHECKGROUP      (TBSTYLE_GROUP | TBSTYLE_CHECK)

/*
** States
*/
#define TBSTATE_CHECKED         0x01
#define TBSTATE_PRESSED         0x02
#define TBSTATE_ENABLED         0x04
#define TBSTATE_HIDDEN          0x08
#define TBSTATE_INDETERMINATE   0x10

/*---------------------------------------------------------------------------
** Typedefs
*/

/*
** Button Structure
*/
typedef struct tagTBBUTTON {
    int  iBitmap;        // 1 based index into bitmap of button's picture
    int  idCommand;      // WM_COMMAND menu ID that this button sends
    BYTE fsState;        // Button's state
    BYTE fsStyle;        // Button's style
} TBBUTTON;

typedef TBBUTTON NEAR* PTBBUTTON;
typedef TBBUTTON FAR* LPTBBUTTON;
typedef const TBBUTTON FAR* LPCTBBUTTON;

class _EXPORT ToolBarButton {
public:
     _MFN ToolBarButton(int id, HWND hWndToolBar);
     _MFN ~ToolBarButton();

     void _MFN Enable(BOOL enable_it = TRUE);
     void _MFN Disable()                               { Enable(FALSE);    }
     void _MFN Show(BOOL show_it = TRUE);
     void _MFN Hide()                                  { Show(FALSE);      }

     void _MFN Check(BOOL checked = TRUE);
     void _MFN Uncheck()                               { Check(FALSE);     }
     BOOL _MFN IsChecked();

     HWND _MFN Handle()       const                    { return hWndToolBar;}
     int  _MFN ResourceId()   const                    { return nButtonId;  }

private:
     HWND           hWndToolBar;
     int            nButtonId;
};

class ToolBar {
public:
     _MFN ToolBar(  HINSTANCE      hInst,
                    HWND           hWnd,
                    DWORD          ws,
                    WORD           wID,
                    int            nBitmaps,
                    WORD           wBMID,
                    LPCTBBUTTON    lpButtons,
                    int            iNumButtons,
                    unsigned short usWidth = 0,
                    unsigned short usHeight = 0
            );
     _MFN ~ToolBar();

     HWND      _MFN Handle()   const              { return hWndToolBar; }
     HINSTANCE _MFN Instance() const              { return hInstance; }
     HWND      _MFN Parent()   const              { return GetParent(Handle()); }

private:
     HINSTANCE      hInstance;
     HWND           hWndToolBar;
};

/*---------------------------------------------------------------------------
** Data
*/

extern HINSTANCE hInstanceGlobal;

/*---------------------------------------------------------------------------
** Functions
*/

#endif    /* toolbar_hpp */
/*---------------------------------------------------------------------------
** End of File
*/

