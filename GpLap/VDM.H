/*
** File   : vdm.h
** Author : TK
** Date   : 20/09/93
**
** $Header:   L:/gplaptim/gplap/vcs/vdm.h__   1.1   26 Jul 1994 23:18:30   tk  $
**
** Routines to co-operate with Window or OS/2 VDMs.
*/

#ifndef   vdm_h
#define   vdm_h

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

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
** Purpose:    To determine is we are running under Window 3.x
**
** Returns:    0000h if running real mode or standard mode Windows.
**
** Notes  :    See VDM_is_windows_VDM() for enhanced mode detection.
*/
short far
VDM_is_windows(
     void
);

/*---------------------------------------------------------------------------
** Purpose:    To determine if we are running under an enhanced Windows VDM.
**
** Returns:    ??00h if Enhanced Windows 3.x not running.
**             ??80h if Enhanced Windows 3.x not running.
**             ??01h if Windows/386 2.x running.
**             ??FFh if Windows/386 2.x running.
**             ??vvH Windows major version number >= 3
**             vv??H Windows minor version number.
**
** Notes  :    Mask the top byte before performing the test.
**             See VDM_is_windows() for real/standard mode detection.
*/
short far
VDM_is_windows_VDM(
     void
);


/*---------------------------------------------------------------------------
** Purpose:    To release the current time slice when application is idle.
**
** Returns:    00h if call is supported, 80h if not supported.
**
** Notes  :    This function also works for OS/2 2.x. 
*/
unsigned char far
VDM_release_time_slice(
     void
);

#ifdef __cplusplus
};
#endif

#endif    /* vdm_h */
/*---------------------------------------------------------------------------
** End of File
*/

