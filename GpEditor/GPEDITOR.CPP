/*
** File   : GPeditor.cpp
** Author : TK
** Date   : 15/08/91
**
** $Header:   L:/GPEDITOR/VCS/GPEDITOR.CP_   1.10   24 Jun 1995 11:36:30   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
**
** To Do:
**
** Keyboard intercept for Combo Box.
** Tab between control bar controls.
** Accelerators.
**
** Image dirty support.
** Load should check for dirty image.
** Check for dirty image before exiting.
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
#include "execheck.hpp"
#include "cntrlbar.hpp"
#include "drawarea.hpp"
#include "drawhelm.hpp"
#include "palette.hpp"
#include "teams.hpp"
#include "version.ipp"

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*lint -esym(528,sccsid,sccsid_debug) */

#if DEBUG
static const char sccsid_debug[] = "@(#)" __FILE__ "\tDebug Enabled!";
#endif
static const char sccsid[] = "@(#)GPeditor.exe"  "\t" VERSION
                              "\t(" __DATE__ "," __TIME__ ")";

#define EDITOR_CAR       1
#define EDITOR_HELMET    2

/*---------------------------------------------------------------------------
** Typedefs
*/

/*---------------------------------------------------------------------------
** Local function prototypes
*/

/*---------------------------------------------------------------------------
** Data
*/

WinClass  *app_class;
AppWindow *app_window;

GP_NAMES            GP_names;
GP_EXE_CAR_NUMBER   GP_EXE_car_numbers[GP_EXE_NUM_DRIVERS];
unsigned char       GP_EXE_driving_aids[5];

BOOL                fGpExeIsFound;
BOOL                fNamesLoaded;

HPALETTE            hPalette;
Bitmap              *pF1CarBitmap;
Bitmap              *pF1CarSmallBitmap;
Bitmap              *pHelmetBitmap;

HINSTANCE           hStringsInstance;
HINSTANCE           hErrorInstance;

BOOL                help_active;
char                help_filename[128];

LANGUAGE            CurrentLanguage = LANG_ENGLISH;

char                command_line_param1[128];
char                command_line_param2[128];

char                szGpExeFilename[128];
char                szNamesFilename[128];
char                szDesignFilename[128];
char                szHelmetFilename[128];
char                szGpeFilename[128];
char                szCommentFilename[128];

BOOL                No3D = FALSE;

/*---------------------------------------------------------------------------
** Functions
*/

#pragma argsused

/*---------------------------------------------------------------------------
** Purpose:    Windows main entry point.
**
** Returns:    Return value of last application message processed.
**
** Notes  :    
*/
int PASCAL
WinMain(
     HINSTANCE hInstance,               /* In  This instance               */
     HINSTANCE hPrevInstance,           /* In  Previous instance           */
     LPSTR     lpCmdLine,               /* In  Invocation command line     */
     int       nCmdShow                 /* In  Show state (yes/no/icon)    */
) {
     Cursor    cursor(IDC_WAIT);
     char      buff[128];

     cursor.Select();
     {
          hErrorInstance      = hInstance;
          hStringsInstance    = hInstance;

          /*
          ** Determine current language ASAP so any error messages are
          ** generated in the correct language.
          */
          ProfileString ps(APP_INIFILE_KEYWORD, APP_INIFILE);
          LANGUAGE  tmp_lang = (LANGUAGE) ps.Read(APP_PROF_STR_LANGUAGE);

          if (tmp_lang > LANG_NOT_SUPPORTED && tmp_lang <= LANG_LAST) {
               CurrentLanguage = tmp_lang;
          }

          No3D = ps.Read(APP_PROF_STR_3D);
          if (!No3D) {
               (void) UtlLoad3D();
               UtlCtl3dRegister(hInstance);
               UtlCtl3dAutoSubclass(hInstance);
          }

          char      *p;

          command_line_param1[0] = '\0';
          command_line_param2[0] = '\0';

          if (*lpCmdLine != '\0') {
               for (p = command_line_param1; *lpCmdLine && *lpCmdLine != ' ';) {
                    *p++ = *lpCmdLine++;
               }
               *p++ = '\0';

               while (*lpCmdLine && *lpCmdLine == ' ') {
                    ++lpCmdLine;
               }

               for (p = command_line_param2; *lpCmdLine && *lpCmdLine != ' ';) {
                    *p++ = *lpCmdLine++;
               }
               *p++ = '\0';
          }

          /*
          ** Enable debugging.
          */
#if DEBUG
          Debug.Enable();
          Debug.SetApplicationName(GetAppName());
#endif

          /*
          ** Detect any tampering or corruption of the .EXE
          */
#if !DEBUG
          if (!ExeCheckModule(hInstance)) {
               return 0;
          }
#endif

          /*
          ** Only create class if not previously initialised.
          */
          if (!hPrevInstance) {
               app_class = new WinClass((LPSTR) APP_CLASS_NAME, hInstance);
               if (app_class == 0) {
                    FatalError(IDT_OUT_OF_MEMORY);
                    return 0;
               }
               else {
                    /*
                    ** Non-default class specifics.
                    */
                    app_class->Cursor(LoadCursor(NULL, IDC_ARROW));
                    app_class->Icon(LoadIcon(hInstance, APP_ICON_NAME));
               }

               /*
               ** Register the newly created class.
               */
               if (!app_class->Register()) {
                    delete app_class;
                    FatalError(IDT_UNABLE_TO_REGISTER_CLASS);
                    return 0;
               }

               /*
               ** We can now destroy the class info as it has been registered.
               */
               delete app_class;
          }

          /*
          ** Warn user that a mouse is required.
          */
          if (GetSystemMetrics(SM_MOUSEPRESENT) == FALSE) {
               strcpy(buff, GetAppName());
               MsgBoxWarning mb(buff, GetIdt(IDT_YOU_NEED_A_MOUSE));
               (void) mb.ProcessMessages();
          }

          /*
          ** Create application window, process messages, and finally
          ** delete window when it has exited.
          */
          app_window = new AppWindow(   hInstance,
                                        hPrevInstance,
                                        APP_CLASS_NAME,
                                        GetAppName(),
                                        WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW
                                   );
          ASSERT(app_window != NULL);
          app_window->Enable();
          app_window->Show(nCmdShow);
     }
     cursor.Deselect();

     int r = app_window->ProcessMessages();

     cursor.Select();
     {
          delete app_window;
     }
     cursor.Deselect();

     if (!No3D) {
          UtlCtl3dUnregister(hInstance);
          UtlUnload3D();
     }

     return r;
}

/*---------------------------------------------------------------------------
** Purpose:    Constructor for main application window.
**
** Returns:    Nothing.
**
** Notes  :    
*/
AppWindow::AppWindow(
     HINSTANCE hInstance,               /* In  This instance               */
     HINSTANCE hPrevInstance,           /* In  Previous instance           */
     LPSTR     class_name,              /* In  Application class name      */
     LPSTR     app_name,                /* In  Application name            */
     DWORD     window_style             /* In  Window style                */
) : MainWindow(hInstance, class_name, app_name, window_style) {
     /*
     ** Ensure all pointers used by destructor are NULL in case we take an
     ** early bath!
     */
     pResourceMenu       = NULL;
     pControlBar         = NULL;
     pDrawingWindow      = NULL;
     pHelmetWindow       = NULL;
     pPaletteWindow      = NULL;
     pTeamsWindow        = NULL;
     pF1CarBitmap        = NULL;
     pF1CarSmallBitmap   = NULL;
     pHelmetBitmap       = NULL;
     hPalette            = NULL;

     lGameExeColourFileOffset           = 0L;
     lGameExeDrivingAidsFileOffset      = 0L;
     lGameExeAiCarGripFileOffset        = 0L;
     lGameExePlayersCarPerfFileOffset   = 0L;

     /*
     ** Setup common private profile string.
     */
     pProfile  = new ProfileString(APP_INIFILE_KEYWORD, APP_INIFILE);
     if (pProfile == NULL) {
          FatalError(IDT_OUT_OF_MEMORY);
          return;
     }

     /*
     ** Setup default states and names.
     */
     fGpExeIsFound       = FALSE;
     fNamesLoaded        = FALSE;
     fCarEditorEnabled   = (pProfile->Read(APP_PROF_STR_EDITOR) == EDITOR_HELMET) ? FALSE : TRUE;
     fDesignLoaded       = FALSE;
     fHelmetLoaded       = FALSE;

     if (!InitResources()) {
          return;
     }

     if (!InitColourPalette()) {
          return;
     }

     InitMenu();
     InitDefaultFileNames();
     InitDefaultCarNumbers();
     InitDefaultNames();
     InitHelpSystem();

     if (!InitRegisterClasses(hPrevInstance)) {
          return;
     }

     if (!InitCreateChildWindows()) {
          return;
     }

     /*
     ** Just application name as caption to start with.
     */
     UpdateCaption("");

     /*
     ** Attempt to remove V1.0's WIN.INI information.
     */
     ProfileString  ps_win_ini(APP_INIFILE_KEYWORD);
     (void) ps_win_ini.DeleteAll();

     /*
     ** Show Credits very first time we're ever run & allow language
     ** selection.
     */
     if (pProfile->Read(APP_PROF_STR_CREDITS) != VERSION_WORD) {
          (void) pProfile->Write(APP_PROF_STR_CREDITS, VERSION_WORD);
          (void) SendMessage(Handle(), WM_COMMAND, IDM_ABOUT, 0L);
          (void) SendMessage(Handle(), WM_COMMAND, IDM_LANGUAGE, 0L);
     }
     SelectNewMenu();

     if (command_line_param1[0] != '\0') {
          (void) SendMessage(Handle(), WM_COMMAND, IDM_FILE_OPEN_GAME_EXE, 0L);
     }
}

/*---------------------------------------------------------------------------
** Purpose:    Destructor for main application window.
**
** Returns:    Nothing.
**
** Notes  :    Destroys all objects.
*/
AppWindow::~AppWindow(
     void
) {
     if (pControlBar) {
          delete pControlBar;
          pControlBar = NULL;
     }

     if (pDrawingWindow) {
          delete pDrawingWindow;
          pDrawingWindow = NULL;
     }

     if (pHelmetWindow) {
          delete pHelmetWindow;
          pHelmetWindow = NULL;
     }

     if (pPaletteWindow) {
          delete pPaletteWindow;
          pPaletteWindow = NULL;
     }

     if (pTeamsWindow) {
          delete pTeamsWindow;
          pTeamsWindow = NULL;
     }

     (void) DeleteObject(hPalette);
     hPalette = NULL;

     if (pF1CarBitmap) {
          delete pF1CarBitmap;
          pF1CarBitmap = NULL;
     }

     if (pF1CarSmallBitmap) {
          delete pF1CarSmallBitmap;
          pF1CarSmallBitmap = NULL;
     }

     if (pHelmetBitmap) {
          delete pHelmetBitmap;
          pHelmetBitmap = NULL;
     }

     if (pResourceMenu) {
          delete pResourceMenu;
          pResourceMenu = NULL;
     }

     if (pProfile) {
          delete pProfile;
          pProfile = NULL;
     }
}

/*---------------------------------------------------------------------------
** Purpose:    Called when the application window is being destroyed.
**
** Returns:    Nothing.
**
** Notes  :    Saves current position of window in WIN.INI.
*/
void
AppWindow::Destroy(
     void
) {
     /*
     ** Save window position (not if iconic).
     */
     if (!IsIconic(Handle())) {
          RECT rect;

          GetWindowCoords(&rect);

          (void) pProfile->Write(APP_PROF_STR_POSITION_X, rect.left + 1);
          (void) pProfile->Write(APP_PROF_STR_POSITION_Y, rect.top + 1);
     }

     if (help_active) {
          Cursor cursor(IDC_WAIT);

          cursor.Select();
          {
               if (WinHelp(Handle(), help_filename, HELP_QUIT, 0L)) {
                    help_active = FALSE;
               }
          }
          cursor.Deselect();
     }

     /*
     ** Select no menu.
     */
     (void) SetMenu(Handle(), NULL);
}

/*---------------------------------------------------------------------------
** Purpose:    Update the menu options available based on the current state.
**
** Returns:    Nothing.
**
** Notes  :    
*/
void
AppWindow::UpdateMenu(
     void
) {
     class Menu m(Handle());

     m.Enable(IDM_FILE_UPDATE_GAME_EXE, fGpExeIsFound);
     m.Enable(IDM_FILE_OPEN_NAMES, fGpExeIsFound);
     m.Enable(IDM_FILE_SAVE_NAMES, fGpExeIsFound);
     m.Enable(IDM_FILE_IMPORT_GPE, fGpExeIsFound);
     m.Enable(IDM_FILE_EXPORT_GPE, fGpExeIsFound);
     m.Enable(IDM_PERFORMANCE_CAR, fGpExeIsFound);
     m.Enable(IDM_PERFORMANCE_DRIVER, fGpExeIsFound);
     m.Enable(IDM_PERFORMANCE_DRIVING_AIDS, fGpExeIsFound && lGameExeDrivingAidsFileOffset != 0L);
     m.Enable(IDM_PERFORMANCE_CAR_NUMBERS, fGpExeIsFound);
     m.Enable(IDM_NAMES, fGpExeIsFound);
     m.Enable(IDM_AI_CAR_GRIP, fGpExeIsFound && lGameExeAiCarGripFileOffset != 0L);
     m.Enable(IDM_PLAYERS_CAR_PERFORMANCE, fGpExeIsFound && lGameExePlayersCarPerfFileOffset != 0L);

     m.Check(IDM_CAR_EDITOR, fCarEditorEnabled);
     m.Check(IDM_HELMET_EDITOR, !fCarEditorEnabled);

     if (pControlBar) {
          pControlBar->UpdateToolBar();
     }
}

/*---------------------------------------------------------------------------
** Purpose:    Called when user chooses "Exit" menu option (not "Close").
**
** Returns:    Nothing.
**
** Notes  :    Terminates application.
*/
void
AppWindow::Exit(
     void
) {
     DestroyWindow();
}

/*---------------------------------------------------------------------------
** Purpose:    Update the application's caption.
**
** Returns:    Nothing.
**
** Notes  :    Appends the specified caption string to the application name.
*/
void
AppWindow::UpdateCaption(
     char *szCaption                    /* In  Extra caption string        */
) {
     static char    caption[128];
     static BOOL    caption_loaded = FALSE;
     char           *p;

     if (szCaption != NULL) {
          if (strlen(szCaption) != 0) {
               if ( (p = strrchr(szCaption, '\\')) != 0    ||
                    (p = strrchr(szCaption, '/')) == 0
               ) {
                    strcpy(tmp_buff, ++p);
               }
               else {
                    strcpy(tmp_buff, szCaption);
               }
               strlwr(tmp_buff);
               strcpy(caption, tmp_buff);
               caption_loaded = TRUE;
          }
          else {
               caption_loaded = FALSE;
          }
     }

     if (caption_loaded) {
          sprintf(msg_buff, "%s - [%s]", GetAppName(), caption);
     }
     else {
#if DEBUG
          sprintf(msg_buff, "%s <%s %s %s>", GetAppName(), VERSION, __DATE__, __TIME__);
#else
          sprintf(msg_buff, "%s", GetAppName());
          SetCaption(GetAppName());
#endif
     }
     SetCaption(msg_buff);
}

/*---------------------------------------------------------------------------
** Purpose:    Switch between car and helmet editor.
**
** Returns:    Nothing.
**
** Notes  :    
*/
void
AppWindow::SwitchEditor(
     void
) {
     if (fCarEditorEnabled) {
          pHelmetWindow->Hide();
          pDrawingWindow->Show();
          pDrawingWindow->RepaintWindow();
     }
     else {
          pDrawingWindow->Hide();
          pHelmetWindow->Show();
          pHelmetWindow->RepaintWindow();
     }
     UpdateMenu();

     if (!fGpExeIsFound) {
          if (fCarEditorEnabled) {
               if (fDesignLoaded) {
                    UpdateCaption(szDesignFilename);
               }
               else {
                    UpdateCaption("");
               }
          }
          else {
               if (fHelmetLoaded) {
                    UpdateCaption(szHelmetFilename);
               }
               else {
                    UpdateCaption("");
               }
          }
     }
     else {
          (void) SendMessage(pControlBar->Handle(), IDE_CBAR_SHOW_DRIVERS, !fCarEditorEnabled, 0L);
     }

     (void) pProfile->Write(APP_PROF_STR_EDITOR, (fCarEditorEnabled) ? EDITOR_CAR : EDITOR_HELMET);
}

/*---------------------------------------------------------------------------
** Purpose:    Determine the Windows environments current language.
**
** Returns:    The matched supported language if found, otherise unsupported.
**
** Notes  :    Uses WIN.INI [intl] sLanguage entry.
*/
LANGUAGE
AppWindow::DetermineLanguage(
     void
) {
     ProfileString ps("intl");

     if (ps.Read(msg_buff, sizeof(msg_buff), "sLanguage")) {
          strlwr(msg_buff);

          if (strcmp(msg_buff, "eng") == 0 || strcmp(msg_buff, "enu") == 0) {
               return LANG_ENGLISH;
          }

          if (strcmp(msg_buff, "fra") == 0 || strcmp(msg_buff, "frc") == 0) {
               return LANG_FRENCH;
          }

          if (strcmp(msg_buff, "deu") == 0) {
               return LANG_GERMAN;
          }
     }

     return LANG_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------
** Purpose:    Select the specified language specific menu.
**
** Returns:    Nothing.
**
** Notes  :    
*/
void
AppWindow::SelectNewMenu(
     void
) {
     char *p;

     switch (CurrentLanguage) {
     case LANG_NOT_SUPPORTED:
     default:
     case LANG_ENGLISH:
          p = "ENGLISH";
          break;

     case LANG_FRENCH:
          p = "FRENCH";
          break;

     case LANG_GERMAN:
          p = "GERMAN";
          break;
     }
     sprintf(msg_buff, "%s%s", APP_MENU_NAME, p);

     (void) SetMenu(Handle(), NULL);
     if (pResourceMenu->IsMenuLoaded()) {
          pResourceMenu->UnloadMenu();
     }
     (void) pResourceMenu->LoadMenu(msg_buff);
     (void) SetMenu(Handle(), pResourceMenu->Handle());
     UpdateMenu();

     /*
     ** Repaint control bar as Windows seems to get confused about the
     ** area it needs to repaint as the client size shrinks when no menu
     ** is present.
     */
     if (pControlBar) {
          pControlBar->RepaintWindow();
     }
}

/*---------------------------------------------------------------------------
** End of File
*/
