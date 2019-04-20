/*
** File   : drwbrush.cpp
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
SolidBrush::SolidBrush(
     COLORREF  c                        /* In  Brush colour                */
) {
     HBRUSH    h;

     h = CreateSolidBrush(c);

     if (h == 0) {
          Debug.Error(__FILE__, __LINE__, "Creation of Solid Brush tool failed.");
     }
     else {
          NewTool(h);
     }
}

/*---------------------------------------------------------------------------
** End of File
*/

