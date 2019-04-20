/*
** File   : drwdrw.cpp
** Author : TK
** Date   : 11/09/91
**
** 
*/

/*---------------------------------------------------------------------------
** Includes
*/

#include <windows.h>
#include <debug.hpp>
#include "drawtool.hpp"
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
DrawingTool::DrawingTool(
     void
) {
     hTool     = 0;
     selected  = FALSE;
}

_MFN
DrawingTool::~DrawingTool(
     void
) {
     if (IsSelected()) {
          Debug.Error(__FILE__, __LINE__, "Attempting to destroy a selected tool.");
     }
     else {
          DeleteObject(hTool);
     }
}

/*---------------------------------------------------------------------------
** End of File
*/

