/*
** File   : drawhelm.hpp
** Author : TK 
** Date   :  8/01/94
**
** $Header:   L:/GPEDITOR/VCS/DRAWHELM.HP_   1.3   01 Oct 1994 15:27:48   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
*/

#ifndef   drawhelm_hpp
#define   drawhelm_hpp

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

class HelmetWindow : public ChildWindow {
public:
     HelmetWindow(HINSTANCE hInst, HWND hWndParent);
     ~HelmetWindow();

private:
     void DestroyWindow();
     BOOL UnknownSysEventHandler(Event *event, long *r_val_p);
     BOOL Paint(void);
     void Mouse(EventMouse *event);
     void SetOurCursor(void);
     void FillHelmetRegion(unsigned short usHelmetRegionIndex, unsigned short usPaletteIndex);
     void DesignNew(void);
     BOOL DesignLoad(char *filename);
     BOOL DesignSave(char *filename);
     void LoadExeColours(GP_EXE_HELMET_COLOURS *pNewGpExeHelmetColours);
     void SaveExeColours(GP_EXE_HELMET_COLOURS *pNewGpExeHelmetColours);
     BOOL SelectDriver(unsigned short usNewDriverIndex);
     BOOL IsLinkedHelmet(unsigned short index);
     void DoDrag(WORD hDragInfo);

     Cursor              *pCursorSprayCan;
     unsigned short      usSelectedPaletteIndex;
     unsigned short      usCurrentDriverIndex;
     char                msg_buff[128];
     BOOL                fInRegion;
     BOOL                fImageChanged;
};

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/


#endif    /* drawhelm_hpp */
/*---------------------------------------------------------------------------
** End of File
*/

