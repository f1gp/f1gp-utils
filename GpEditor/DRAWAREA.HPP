/*
** File   : drawarea.hpp
** Author : TK 
** Date   : 21/11/93
**
** $Header:   L:/GPEDITOR/VCS/DRAWAREA.HP_   1.3   01 Oct 1994 15:27:46   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
*/

#ifndef   drawarea_hpp
#define   drawarea_hpp

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

class DrawingWindow : public ChildWindow {
public:
     DrawingWindow(HINSTANCE hInst, HWND hWndParent);
     ~DrawingWindow();

private:
     void DestroyWindow();
     BOOL UnknownSysEventHandler(Event *event, long *r_val_p);
     BOOL Paint(void);
     void Mouse(EventMouse *event);
     void SetOurCursor(void);
     void FillCarRegion(unsigned short usCarRegionIndex, unsigned short usPaletteIndex);
     void DesignNew(void);
     BOOL DesignLoad(char *filename);
     BOOL DesignSave(char *filename);
     void LoadExeColours(GP_EXE_CAR_COLOURS *pNewGpExeCarColours);
     void SaveExeColours(GP_EXE_CAR_COLOURS *pNewGpExeCarColours);
     BOOL SelectTeam(unsigned short usNewTeamIndex);
     void DoDrag(WORD hDragInfo);

     Cursor              *pCursorSprayCan;
     unsigned short      usSelectedPaletteIndex;
     unsigned short      usCurrentTeamIndex;
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


#endif    /* drawarea_hpp */
/*---------------------------------------------------------------------------
** End of File
*/

