/*
** File   : execheck.hpp
** Author : Trevor
** Date   : 26/08/92
**
** $Header:   L:/GPEDITOR/VCS/EXECHECK.HP_   1.2   01 Oct 1994 15:27:50   tk  $
**
** Check .EXE file to determine if sum check is zero.
*/

#ifndef   execheck_hpp
#define   execheck_hpp

/*---------------------------------------------------------------------------
** Includes
*/

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
** Purpose:    Check module file to determine if sum check is zero.
**
** Returns:    True if OK, false if corrupted or can't be opened.
**
** Notes  :    File is opened in shared mode to avoid file sharing errors.
*/
BOOL
ExeCheckModule(
     HINSTANCE instance                 /* In  Module instance to check    */
);

/*---------------------------------------------------------------------------
** Purpose:    Check .EXE file to determine if sum check is zero.
**
** Returns:    True if OK, false if corrupted or can't be opened.
**
** Notes  :    File is opened in shared mode to avoid file sharing errors.
*/
BOOL
ExeCheckFilename(
     const char _FAR *filename          /* In  Filename to check           */
);

#endif    /* execheck_hpp */
/*---------------------------------------------------------------------------
** End of File
*/





