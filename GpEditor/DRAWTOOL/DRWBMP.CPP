/*
** File   : drwbmp.cpp
** Author : TK
** Date   : 04/11/91
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
Bitmap::Bitmap(
     HINSTANCE instance,
     LPSTR     name
) {
     HBITMAP   h;

     h = LoadBitmap(instance, name);

     if (h == 0) {
          Debug.Error(__FILE__, __LINE__, "Creation of Bitmap tool failed.");
     }
     else {
          GetObject(h, sizeof(bm), (LPSTR) &bm);
          NewTool(h);
     }
}

_MFN
Bitmap::Bitmap(
     Bitmap _FAR *bitmap_to_clone
) {
     HBITMAP   h;

     GetObject(bitmap_to_clone->Handle(), sizeof(bm), (LPSTR) &bm);
     h = CreateBitmapIndirect(&bm);
     NewTool(h);
}

_MFN
Bitmap::Bitmap(
     Bitmap _FAR    *bitmap_to_clone,
     int            new_width,
     int            new_height
) {
     HBITMAP   h;

     GetObject(bitmap_to_clone->Handle(), sizeof(bm), (LPSTR) &bm);
     h = CreateBitmap(new_width, new_height, bm.bmPlanes, bm.bmBitsPixel, NULL);
     GetObject(h, sizeof(bm), (LPSTR) &bm);
     NewTool(h);
}

/*---------------------------------------------------------------------------
** End of File
*/

