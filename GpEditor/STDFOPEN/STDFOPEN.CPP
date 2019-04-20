/*
** File   : stdfopen.cpp
** Author : TK
** Date   : 12/02/94
**
** Interface to Windows 3.1 common file open dialog.
*/

/*---------------------------------------------------------------------------
** Includes
*/

#include <_cppwin_.hpp>
#include <stdlib.h>
#include <string.h>
#include <commdlg.h>
#include "stdfopen.hpp"
#include "version.ipp"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/


_MFN 
StdFileOpenDialog::StdFileOpenDialog(
     HINSTANCE      hInstance,
     HWND           hWndParent,
     LPSTR          szCaption,
     LPSTR          szBuff,
     unsigned short usBuffSize,
     LPSTR          szDefaultExtension,
     LPSTR          szFilterSpec
) {
     StdFileOpenDialog::hInstance            = hInstance;
     StdFileOpenDialog::hWndParent           = hWndParent;
     StdFileOpenDialog::szCaption            = szCaption;
     StdFileOpenDialog::szBuff               = szBuff;
     StdFileOpenDialog::usBuffSize           = usBuffSize;
     StdFileOpenDialog::szDefaultExtension   = szDefaultExtension;
     StdFileOpenDialog::szFilterSpec         = szFilterSpec;
}

_MFN 
StdFileOpenDialog::~StdFileOpenDialog(
     void
) {
     ;
}

/*---------------------------------------------------------------------------
** Purpose:    Process dialog.
**
** Returns:    Nothing
**
** Notes  :    
*/
short _MFN 
StdFileOpenDialog::ProcessMessages(
     void
) {
     OPENFILENAME   ofn;
     short          r;

     _fstrncpy(pathname, szBuff, sizeof(pathname));
     _splitpath(pathname, drive, dir, fname, ext);
     _fstrcpy(directory, drive);
     _fstrcat(directory, dir);
     _fstrcpy(filename, fname);
     _fstrcat(filename, ext);

     /*
     ** Must strip trailling slash off directory for Common dialog
     ** handler to select directory.
     */
     if (strlen(directory) > 0) {
          directory[strlen(directory) - 1] = '\0';
     }

     memset(&ofn, 0x00, sizeof(ofn));

     ofn.lStructSize          = sizeof(OPENFILENAME);
     ofn.hwndOwner            = hWndParent;
     ofn.lpstrFilter          = szFilterSpec;
     ofn.lpstrCustomFilter    = NULL;
     ofn.nMaxCustFilter       = 0;
     ofn.nFilterIndex         = 1;
     ofn.lpstrFile            = filename;
     ofn.nMaxFile             = sizeof(filename);
     ofn.lpstrInitialDir      = directory;
     ofn.lpstrFileTitle       = NULL;
     ofn.nMaxFileTitle        = 0;
     ofn.lpstrTitle           = szCaption;
     ofn.lpstrDefExt          = szDefaultExtension;
     ofn.Flags                = OFN_HIDEREADONLY;
     ofn.lpfnHook             = NULL;
     ofn.lCustData            = 0;
     ofn.lpTemplateName       = 0;

     r = GetOpenFileName((LPOPENFILENAME) &ofn);

     if (r) {
          _fstrncpy(szBuff, filename, usBuffSize);
     }

     return r;
}

/*---------------------------------------------------------------------------
** End of File
*/

