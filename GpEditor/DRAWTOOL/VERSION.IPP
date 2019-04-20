/*
** Change History: 
** 
** V1.1   First cut.
** V1.2   Added new Bitmap tool.
** V1.3   Bitmaps can now be cloned.
** V1.4   Bits() now cast for C++ V3.0
** V2.0   Added DLL support.
** V3.0   Added W3.1 STRICT types.
*/

#define VERSION     "V3.0 23/03/93"

#if defined(__DLL__)
#    if defined(_HIDDEN_DLL)
#         define TARGET  "drawtool.lih"
#    else
#         define TARGET  "drawtool.lid"
#    endif
#else
#    define TARGET  "drawtool.lib"
#endif

static const char sccsid[] = "@(#)" TARGET "\t" VERSION "\t" "(" __DATE__ "," __TIME__ ")" " Beta";

