/*
** File   : stdfopen.hpp
** Author : TK/KB
** Date   : 25/08/92
**
** File open dialog.
*/

#ifndef   stdfopen_hpp
#define   stdfopen_hpp

/*---------------------------------------------------------------------------
** Includes
*/

#include <_cppwin_.hpp>

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/

/*---------------------------------------------------------------------------
** Purpose:    File open dialog.
**
** Notes  :    
*/
class _EXPORT StdFileOpenDialog {
public:
     _MFN StdFileOpenDialog(  HINSTANCE hInstance, HWND hWndParent,
                              LPSTR szCaption,
                              LPSTR szBuff, unsigned short usBuffSize,
                              LPSTR szDefaultExtension = NULL,
                              LPSTR szFilterSpec = NULL
                         );
     _MFN ~StdFileOpenDialog();

     short _MFN ProcessMessages();

     HWND _MFN Handle() const                          { return hWndParent; }

private: 
     HINSTANCE      hInstance;
     HWND           hWndParent;
     LPSTR          szCaption;
     LPSTR          szBuff;
     unsigned short usBuffSize;
     LPSTR          szDefaultExtension;
     LPSTR          szFilterSpec;

     char           drive[_MAX_DRIVE];
     char           dir[_MAX_DIR];
     char           fname[_MAX_FNAME];
     char           ext[_MAX_EXT];
     char           pathname[_MAX_PATH];
     char           directory[_MAX_PATH];
     char           filename[_MAX_PATH];
};

#endif    /* stdfopen_hpp */
/*---------------------------------------------------------------------------
** End of File
*/





