/*
** Change History: 
** 
** V1.0   First cut.
** V2.0   Added W3.1 STRICT types.
** V2.1   Added Facility to restore the original drive if the user selected
**        a floppy
** V3.0   Windows V3.1 Common File Open Dialog Now Used.
*/

#define VERSION     "V3.0 12/02/94"

#if defined(__DLL__)
#    if defined(_HIDDEN_DLL)
#         define TARGET  "stdfopen.lih"
#    else
#         define TARGET  "stdfopen.lid"
#    endif
#else
#    define TARGET  "stdfopen.lib"
#endif

static const char sccsid[] = "@(#)" TARGET "\t" VERSION "\t" "(" __DATE__ "," __TIME__ ")";
