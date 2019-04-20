/*
** Change History: 
** 
** V1.0   First cut.
*/

#define VERSION     "V1.0 12/02/94"

#if defined(__DLL__)
#    if defined(_HIDDEN_DLL)
#         define TARGET  "toolbar.lih"
#    else
#         define TARGET  "toolbar.lid"
#    endif
#else
#    define TARGET  "toolbar.lib"
#endif

static const char sccsid[] = "@(#)" TARGET "\t" VERSION "\t" "(" __DATE__ "," __TIME__ ")";
