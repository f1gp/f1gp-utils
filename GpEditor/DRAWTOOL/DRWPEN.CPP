/*
** File   : drwpen.cpp
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
Pen::Pen(
     int       width,                   /* In  Pen width in pixels         */
     COLORREF  c,                       /* In  Pen colour                  */
     int       type                     /* In  Pen type/style              */
) {
     HPEN h;

     h = CreatePen(type, width, c);

     if (h == 0) {
          Debug.Error(__FILE__, __LINE__, "Creation of Pen tool failed.");
     }
     else {
          NewTool(h);
     }
}

/*---------------------------------------------------------------------------
** End of File
*/

