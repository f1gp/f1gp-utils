/*
** File   : palette.hpp
** Author : TK 
** Date   : 21/11/93
**
** $Header:   L:/GPEDITOR/VCS/PALETTE.HP_   1.2   01 Oct 1994 15:27:56   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
*/

#ifndef   palette_hpp
#define   palette_hpp

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

class PaletteWindow : public ChildWindow {
public:
     PaletteWindow( HINSTANCE hInst, HWND hWndParent,
                    unsigned short usWidth, unsigned short usHeight);
     ~PaletteWindow();

private:
     BOOL UnknownSysEventHandler(Event *event, long *r_val_p);
     BOOL Paint(void);
     void Mouse(EventMouse *event);

     Cursor    *pCursorPalette;
     Bitmap    *pClientBitmap;
};

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/


#endif    /* palette_hpp */
/*---------------------------------------------------------------------------
** End of File
*/

