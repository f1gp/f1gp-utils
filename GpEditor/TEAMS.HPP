/*
** File   : teams.hpp
** Author : TK 
** Date   : 11/12/93
**
** $Header:   L:/GPEDITOR/VCS/TEAMS.HP_   1.3   01 Oct 1994 15:27:50   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
*/

#ifndef   teams_hpp
#define   teams_hpp

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define TEAMS_SCALING    7

#define TEAMS_NUM_X      2
#define TEAMS_NUM_Y      (GP_EXE_NUM_TEAMS / TEAMS_NUM_X)

#define TEAMS_WINDOW_WIDTH(car_width)        (((car_width) / TEAMS_SCALING) * TEAMS_NUM_X)

/*---------------------------------------------------------------------------
** Typedefs
*/

class TeamsWindow : public ChildWindow {
public:
     TeamsWindow(HINSTANCE hInst, HWND hWndParent,
                    unsigned short usWidth, unsigned short usHeight);
     ~TeamsWindow();
     void UpdateMemoryImage(void);

private:
     void DestroyWindow(void);
     BOOL UnknownSysEventHandler(Event *event, long *r_val_p);
     BOOL Paint(void);
     void FrameActiveTeam(unsigned short usNewActiveTeam);
     void FrameTeam(PaintWindow *pPW, unsigned short usTeamIndex, BOOL fActive);
     void Mouse(EventMouse *event);
     void SetOurCursor(void);
     void DoDrag(WORD hDragInfo);

     Bitmap              *pClientBitmap;
     Cursor              *pCursorTeamCar;
     BOOL                fInBitmap;
     unsigned short      usClientWidth;
     unsigned short      usClientHeight;
     unsigned short      usMiniCarWidth;
     unsigned short      usMiniCarHeight;
     unsigned short      usSelectedTeam;
};

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/


#endif    /* teams_hpp */
/*---------------------------------------------------------------------------
** End of File
*/

