/*
** File   : drawtool.hpp
** Author : Trevor
** Date   : 21/08/91
**
**
*/

#ifndef   drawtool_hpp
#define   drawtool_hpp

/*---------------------------------------------------------------------------
** Includes
*/

#include <_cppwin_.hpp>

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

/*
** We don't include paintwin.hpp as we land up with a chicken & egg
** situation as it needs this file! So just say this class exists.
*/
class _EXPORT PaintWindow;


/*
** Drawing tool class.
*/
class _EXPORT DrawingTool {
public:
     _MFN DrawingTool();
     _MFN ~DrawingTool();

     HANDLE    _MFN Handle() const                     { return hTool; }

protected:
     friend    class _EXPORT PaintWindow;

     void      _MFN NewTool(HANDLE tool_handle);

     void      _MFN Selected();
     void      _MFN Deselected();

     BOOL      _MFN IsSelected() const                 { return selected; }

private:
     HANDLE    hTool;
     int       selected;
};


class _EXPORT Pen : public DrawingTool {
public:
     _MFN Pen(int width = 1, COLORREF c = RGB(0,0,0), int type = PS_SOLID);
};


class _EXPORT Brush : public DrawingTool {
     /*<TK>*//* It feels like this should be a base class for Solid and Patterned brushes */
protected:
     _MFN Brush() { ; }
};


class _EXPORT SolidBrush : public Brush {
public:
     _MFN SolidBrush(COLORREF c = RGB(0,0,0));
};


class _EXPORT Font : public DrawingTool {
public:
     _MFN Font::Font(    int       height,
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
                    );
};


class _EXPORT Bitmap : public DrawingTool {
public:
     _MFN Bitmap(HINSTANCE instance, LPSTR name);
     _MFN Bitmap(Bitmap _FAR *bitmap_to_clone);
     _MFN Bitmap(Bitmap _FAR *bitmap_to_clone, int new_width, int new_height);

     int            _MFN Type()         const     { return bm.bmType;      }
     int            _MFN Height()       const     { return bm.bmHeight;    }
     int            _MFN Width()        const     { return bm.bmWidth;     }
     int            _MFN WidthInBytes() const     { return bm.bmWidthBytes;}
     BYTE           _MFN Planes()       const     { return bm.bmPlanes;    }
     BYTE           _MFN BitsPerPixel() const     { return bm.bmBitsPixel; }
     void _FAR *    _MFN Bits()         const     { return ((void *) bm.bmBits); }

private:
     BITMAP    bm;
};

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/


#endif    /* drawtool_hpp */
/*---------------------------------------------------------------------------
** End of File
*/





