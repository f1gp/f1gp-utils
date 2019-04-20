/*
** File   : drwfont.cpp
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
Font::Font(    int       height,
               int       width,
               int       escapement,
               int       orientation,
               int       weight,
               BYTE      italic,
               BYTE      underline,
               BYTE      strikeout,
               BYTE      charset,
               BYTE      out_precision,
               BYTE      clip_precision,
               BYTE      quality,
               BYTE      pitch_and_family,
               LPSTR     typeface
) {
     HFONT     h;

     h = CreateFont(     height,
                         width,
                         escapement,
                         orientation,
                         weight,
                         italic,
                         underline,
                         strikeout,
                         charset,
                         out_precision,
                         clip_precision,
                         quality,
                         pitch_and_family,
                         typeface
                    );

     if (h == 0) {
          Debug.Error(__FILE__, __LINE__, "Creation of Font tool failed.");
     }
     else {
          NewTool(h);
     }
}

/*---------------------------------------------------------------------------
** End of File
*/

