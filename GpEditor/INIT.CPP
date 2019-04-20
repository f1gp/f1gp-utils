/*
** File   : init.cpp
** Author : TK 
** Date   : 26/02/94
**
** $Header:   L:/GPEDITOR/VCS/INIT.CP_   1.4   24 Jun 1995 11:36:48   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Initialisation.
*/

/*---------------------------------------------------------------------------
** Includes
*/

/*lint -elib(???) */
#include <cppwin.hpp>
/*lint +elib(???) */
#include "GPeditor.hpp"
#include "GPeditor.h"
#pragma hdrstop
/*lint -elib(???) */
#include <ctype.h>
#include <io.h>
/*lint +elib(???) */
#include "palette.i"
#include "cntrlbar.hpp"
#include "drawarea.hpp"
#include "drawhelm.hpp"
#include "palette.hpp"
#include "teams.hpp"
#include "version.ipp"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#define DEFAULT_GP_EXE_FILENAME         "gp.exe"
#define DEFAULT_NAMES_FILENAME          "*.nam"
#define DEFAULT_DESIGN_FILENAME         "*.car"
#define DEFAULT_HELMET_FILENAME         "*.hel"
#define DEFAULT_GPE_FILENAME            "f1sa.gpe"
#define DEFAULT_COMMENT_FILENAME        "f1sa.txt"

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static BOOL IsRemovableMedia(char const *pathname);

/*---------------------------------------------------------------------------
** Data
*/

/*---------------------------------------------------------------------------
** Functions
*/

void
AppWindow::InitDefaultCarNumbers(
     void
) {
     unsigned short i;

     /*
     ** Initialise default car numbers.
     */
     for (i = 0; i < NUM_ELEMENTS(GP_EXE_car_numbers); i++) {
          GP_EXE_car_numbers[i].number = i + 1;
     }
}

void
AppWindow::InitDefaultNames(
     void
) {
     unsigned short i;

     /*
     ** Initialise default names.
     */
     memset(&GP_names, 0x00, sizeof(GP_names));
     for (i = 0; i < NUM_ELEMENTS(GP_names.drivers); i++) {
          sprintf(GP_names.drivers[i].driver, "%s #%u", GetIdt(IDT_DRIVER), i + 1);
     }
     for (i = 0; i < NUM_ELEMENTS(GP_names.teams); i++) {
          sprintf(GP_names.teams[i].team, "%s %u", GetIdt(IDT_TEAM), i + 1);
     }

     if (pControlBar) {
          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_LOAD_NAMES, 0, 0L);
          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_LOAD_DRIVER_NAMES, 0, 0L);
     }
}

void
AppWindow::InitDefaultFileNames(
     void
) {
     if ( !(   pProfile->Read(szGpExeFilename, sizeof(szGpExeFilename), APP_PROF_STR_EXE_FILENAME)       &&
               !IsRemovableMedia(szGpExeFilename)                                                        &&
               access(szGpExeFilename, 0) == 0
          )
     ) {
          strcpy(szGpExeFilename, DEFAULT_GP_EXE_FILENAME);
     }
     
     if ( !(   pProfile->Read(szNamesFilename, sizeof(szNamesFilename), APP_PROF_STR_NAMES_FILENAME)     && 
               !IsRemovableMedia(szNamesFilename)                                                        &&
               access(szNamesFilename, 0) == 0
          )
     ) {
          strcpy(szNamesFilename, DEFAULT_NAMES_FILENAME);
     }

     if ( !(   pProfile->Read(szDesignFilename, sizeof(szDesignFilename), APP_PROF_STR_CAR_FILENAME)     &&
               !IsRemovableMedia(szDesignFilename)                                                       &&
               access(szDesignFilename, 0) == 0
          )
     ) {
          strcpy(szDesignFilename, DEFAULT_DESIGN_FILENAME);
     }

     if ( !(   pProfile->Read(szHelmetFilename, sizeof(szHelmetFilename), APP_PROF_STR_HELMET_FILENAME)  &&
               !IsRemovableMedia(szHelmetFilename)                                                       &&
               access(szHelmetFilename, 0) == 0
          )
     ) {
          strcpy(szHelmetFilename, DEFAULT_HELMET_FILENAME);
     }

     if ( !(   pProfile->Read(szGpeFilename, sizeof(szGpeFilename), APP_PROF_STR_GPE_FILENAME) &&
               !IsRemovableMedia(szGpeFilename)                                                &&
               access(szGpeFilename, 0) == 0
          )
     ) {
          strcpy(szGpeFilename, DEFAULT_GPE_FILENAME);
     }

     if ( !(   pProfile->Read(szCommentFilename, sizeof(szCommentFilename), APP_PROF_STR_COMMENT_FILENAME) &&
               !IsRemovableMedia(szCommentFilename)                                                &&
               access(szCommentFilename, 0) == 0
          )
     ) {
          strcpy(szCommentFilename, DEFAULT_COMMENT_FILENAME);
     }
}

BOOL
AppWindow::InitResources(
     void
) {
     /*
     ** Default menu.
     */
     pResourceMenu = new ResourceMenu(Instance());
     if (pResourceMenu == NULL) {
          FatalError(IDT_OUT_OF_MEMORY);
          return FALSE;
     }

     /*
     ** Load bitmaps
     */
     pF1CarBitmap = new Bitmap(Instance(), APP_BITMAP_F1_CAR);
     if (pF1CarBitmap == NULL) {
          FatalError(IDT_UNABLE_TO_LOAD_BITMAP);
          return FALSE;
     }

     pF1CarSmallBitmap = new Bitmap(Instance(), APP_BITMAP_F1_CAR_SMALL);
     if (pF1CarSmallBitmap == NULL) {
          FatalError(IDT_UNABLE_TO_LOAD_BITMAP);
          return FALSE;
     }

     pHelmetBitmap = new Bitmap(Instance(), APP_BITMAP_HELMET);
     if (pHelmetBitmap == NULL) {
          FatalError(IDT_UNABLE_TO_LOAD_BITMAP);
          return FALSE;
     }

     return TRUE;
}

void
AppWindow::InitHelpSystem(
     void
) {
     OFSTRUCT of;

     /*
     ** See if the help file is in the same place as the .EXE, in case the
     ** silly user hasn't included the .EXE (and hence the .HLP) in the path.
     */
     help_active = FALSE;
     strcpy(help_filename, APP_HELPFILE);
     if (GetModuleFileName(Instance(), help_filename, sizeof(help_filename)) > 4) {
          strcpy(&help_filename[strlen(help_filename) - 3], "HLP");
          if (OpenFile(help_filename, &of, OF_EXIST) == -1) {
               /*
               ** Not found in same location as .EXE, so WINHELP will have
               ** to find it.
               */
               strcpy(help_filename, APP_HELPFILE);
          }
     }
}

BOOL
AppWindow::InitColourPalette(
     void
) {
     unsigned short i;

     /*
     ** Create colour palette.
     */
     LOGPALETTE far *plgPalette;     

     plgPalette = (LOGPALETTE *) LocalAlloc(LPTR, sizeof(LOGPALETTE) + NUM_COLOURS_IN_PALETTE * sizeof(PALETTEENTRY));
     if (plgPalette == NULL) {
          FatalError(IDT_OUT_OF_MEMORY);
          return FALSE;
     }

     plgPalette->palNumEntries = NUM_COLOURS_IN_PALETTE;
     plgPalette->palVersion    = 0x300;

     for (i = 0; i < NUM_COLOURS_IN_PALETTE; i++) {
          plgPalette->palPalEntry[i].peRed    = RGB_Palette[i][0] << 2;
          plgPalette->palPalEntry[i].peGreen  = RGB_Palette[i][1] << 2;
          plgPalette->palPalEntry[i].peBlue   = RGB_Palette[i][2] << 2;
          plgPalette->palPalEntry[i].peFlags  = NULL;
     }

     hPalette = CreatePalette(plgPalette);
     if (hPalette == NULL) {
          FatalError(IDT_UNABLE_TO_CREATE_PALETTE);
          return FALSE;
     }
     (void) LocalFree((HLOCAL) plgPalette);

     return TRUE;
}

BOOL
AppWindow::InitRegisterClasses(
     HINSTANCE hPrevInstance            /* In  Previous instance           */
) {
     /*
     ** Register all child window classes (first instance only).
     */
     if (!hPrevInstance) {
          WinClass  ControlBarClass(CONTROL_BAR_CLASS_NAME, Instance());
          WinClass  DrawingClass(DRAWING_CLASS_NAME, Instance());
          WinClass  HelmetClass(HELMET_CLASS_NAME, Instance());
          WinClass  PaletteClass(PALETTE_CLASS_NAME, Instance());
          WinClass  TeamsClass(TEAMS_CLASS_NAME, Instance());

          if ( !ControlBarClass.Register()   ||
               !DrawingClass.Register()      ||
               !HelmetClass.Register()       ||
               !PaletteClass.Register()      ||
               !TeamsClass.Register()
          ) {
               FatalError(IDT_UNABLE_TO_REGISTER_CLASS);
               return FALSE;
          }
     }

     return TRUE;
}

void
AppWindow::InitMenu(
     void
) {
     LANGUAGE  tmp_lang = (LANGUAGE) pProfile->Read(APP_PROF_STR_LANGUAGE);

     if (tmp_lang > LANG_NOT_SUPPORTED && tmp_lang <= LANG_LAST) {
          CurrentLanguage = tmp_lang;
     }
     else {
          CurrentLanguage = DetermineLanguage();
     }
     SelectNewMenu();
}

BOOL
AppWindow::InitCreateChildWindows(
     void     
) {
     unsigned short usClientHeight;
     unsigned short usControlBarHeight;

     /*
     ** Create child windows.
     */
     usClientHeight      = pF1CarSmallBitmap->Height() * (GP_EXE_NUM_TEAMS / 2);
     usControlBarHeight  = usClientHeight - PALETTE_BOX_TOTAL_HEIGHT - pF1CarBitmap->Height();

     pControlBar = new ControlBar(      Instance(), Handle(),
                                        pF1CarBitmap->Width(),
                                        usControlBarHeight,
                                        usClientHeight
                                   );
     if (pControlBar == NULL) {
          FatalError(IDT_OUT_OF_MEMORY);
          return FALSE;
     }

     pDrawingWindow = new DrawingWindow(Instance(), Handle());
     if (pDrawingWindow == NULL) {
          FatalError(IDT_OUT_OF_MEMORY);
          return FALSE;
     }

     pHelmetWindow = new HelmetWindow(Instance(), Handle());
     if (pHelmetWindow == NULL) {
          FatalError(IDT_OUT_OF_MEMORY);
          return FALSE;
     }

     pPaletteWindow = new PaletteWindow(     Instance(), Handle(),
                                             pF1CarBitmap->Width(),
                                             PALETTE_BOX_TOTAL_HEIGHT
                                        );
     if (pPaletteWindow == NULL) {
          FatalError(IDT_OUT_OF_MEMORY);
          return FALSE;
     }

     pTeamsWindow = new TeamsWindow(    Instance(), Handle(),
                                        TEAMS_WINDOW_WIDTH(pF1CarBitmap->Width()),
                                        pF1CarSmallBitmap->Height() * (GP_EXE_NUM_TEAMS / 2)
                                   );
     if (pTeamsWindow == NULL) {
          FatalError(IDT_OUT_OF_MEMORY);
          return FALSE;
     }

     /*
     ** Calculate maximum frame window size based on F1 car bitmap.
     */
     frame_width    = GetWindowWidth() - GetClientWidth();
     frame_width   += pF1CarBitmap->Width();
     frame_height   = GetWindowHeight() - GetClientHeight();
     frame_height  += usClientHeight;

     /*
     ** Restore window position. Ensures that origin is within the screen.
     */
     POINT     pt;

     pt.x = pProfile->Read(APP_PROF_STR_POSITION_X);
     pt.y = pProfile->Read(APP_PROF_STR_POSITION_Y);

     if ( pt.x > 0 && pt.x <= GetSystemMetrics(SM_CXSCREEN) &&
          pt.y > 0 && pt.y <= GetSystemMetrics(SM_CYSCREEN)
     ) {
          SetWindowCoords(pt.x - 1, pt.y - 1, frame_width, frame_height);
     }
     else {
          SetWindowWidth(frame_width);
          SetWindowHeight(frame_height);
     }

     /*
     ** Position and size all client windows.
     */
     pControlBar->SetWindowCoords(      0, 0,
                                        pF1CarBitmap->Width(),
                                        usControlBarHeight
                                   );
     pDrawingWindow->SetWindowCoords(   0,
                                        usControlBarHeight,
                                        pF1CarBitmap->Width(),
                                        pF1CarBitmap->Height()
                                   );
     pHelmetWindow->SetWindowCoords(    0,
                                        usControlBarHeight,
                                        pHelmetBitmap->Width(),
                                        pHelmetBitmap->Height()
                                   );
     pPaletteWindow->SetWindowCoords(   0,
                                        usClientHeight - PALETTE_BOX_TOTAL_HEIGHT,
                                        pF1CarBitmap->Width(),
                                        PALETTE_BOX_TOTAL_HEIGHT
                                   );
     pTeamsWindow->SetWindowCoords(     pF1CarBitmap->Width(),
                                        0,
                                        TEAMS_WINDOW_WIDTH(pF1CarBitmap->Width()),
                                        pF1CarSmallBitmap->Height() * (GP_EXE_NUM_TEAMS / 2)
                                   );

     /*
     ** Show all client windows.
     */
     pControlBar->Show();
     if (fCarEditorEnabled) {
          pDrawingWindow->Show();
     }
     else {
          pHelmetWindow->Show();
     }
     pPaletteWindow->Show();
     pTeamsWindow->Show();

     return TRUE;
}

/*---------------------------------------------------------------------------
** Purpose:    To determine if a path is on removeable media.
**
** Returns:    True if removeable, False if not.
**
** Notes  :    
*/
static BOOL
IsRemovableMedia(
     char const     *pathname           /* In  Pathname to check           */
) {
     BOOL removeable = FALSE;
        
     if (strlen(pathname) > 2 && pathname[1] == ':') {
          char c = pathname[0];

          if (islower(c)) {
               c = toupper(c);
          }

          removeable = (GetDriveType(c - 'A') == DRIVE_REMOVABLE) ? TRUE : FALSE;
     }

     return removeable;
}

/*---------------------------------------------------------------------------
** End of File
*/

