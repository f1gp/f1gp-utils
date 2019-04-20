/*
** File   : execheck.cpp
** Author : Trevor
** Date   : 27/08/92
**
** $Header:   L:/GPEDITOR/VCS/EXECHECK.CP_   1.3   01 Oct 1994 15:27:50   tk  $
**
** Check .EXE file to determine if sum check is zero.
*/

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <cppwin.hpp>
/*lint +elib(???) */
#include "execheck.hpp"

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
) {
     char progname[128];
     BOOL ok = FALSE;
     
     if ( GetModuleFileName(instance, progname, sizeof(progname)) == 0 ||
          !ExeCheckFilename(progname)
     ) {
          char buff[128];
          char *p = strrchr(progname, '\\');

          (void) sprintf(buff, "Warning: The file called %Fs has been corrupted or tampered with. Unable to continue, terminating application.", (LPSTR) ++p);
          MsgBoxError mbe("Fatal Error", buff);
          (void) mbe.ProcessMessages();
     }
     else {
          ok = TRUE;
     }

     return ok;
}


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
) {
     unsigned char  disk_buff[512];
     unsigned char  check_a = 0xff;
     unsigned char  check_b = 0xff;
     unsigned char  *p;
     unsigned int   count;
     int            fd;
     BOOL           ok = FALSE;
     
     if ((fd = _lopen(filename, OF_SHARE_DENY_WRITE)) != -1) {
          while ((count = _lread(fd, disk_buff, sizeof(disk_buff))) > 0) {
               for (p = disk_buff; count--; ) {
                    check_a += *p++;
                    check_b += check_a;
               }
          }
          (void) _lclose(fd);

          if (check_a == 0 && check_b == 0) {
               ok = TRUE;
          }
     }

     return ok;
}

/*---------------------------------------------------------------------------
** End of File
*/

