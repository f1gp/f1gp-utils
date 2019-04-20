/*
** File   : cntrlbar.hpp
** Author : TK 
** Date   : 21/11/93
**
** $Header:   L:/GPEDITOR/VCS/CNTRLBAR.HP_   1.3   01 Oct 1994 15:28:00   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
*/

#ifndef   cntrlbar_hpp
#define   cntrlbar_hpp

/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

class ControlBar : public ChildWindow {
public:
     ControlBar(    HINSTANCE      hInst,
                    HWND           hWndParent,
                    unsigned short usWidth,
                    unsigned short usHeight,
                    unsigned short usComboBoxHeight
          );
     ~ControlBar();

     void UpdateToolBar();

private:
     BOOL UserEventHandler(Event *event);
     BOOL UnknownSysEventHandler(Event *event, long *r_val_p);
     BOOL Paint(void);

     void LoadNames(void);
     void LoadDriverNames(void);

     ComboBox  *pcbTeams;
     ComboBox  *pcbDrivers;
};

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/


#endif    /* cntrlbar_hpp */
/*---------------------------------------------------------------------------
** End of File
*/

