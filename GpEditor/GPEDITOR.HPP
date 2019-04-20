/*
** File   : GPeditor.hpp
** Author : TK
** Date   : 15/08/91
**
** $Header:   L:/GPEDITOR/VCS/GPEDITOR.HP_   1.6   01 Oct 1994 15:27:52   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
*/

#ifndef   GPeditor_hpp
#define   GPeditor_hpp

/*lint -esym(1509,MainWindow,DialogWindow) Ignore non-virtual destructor        */
/*lint -esym(1512,MainWindow,DialogWindow,Window,ChildWindow,PaintWindow,Brush,DrawingTool) */
/*lint -e713 -e732 -e737 Ignore signed/unsigned                                 */
/*lint -e1712 Ignore no default constructor                                     */
/*lint -esym(534,memcpy,memset,strcpy,strcat,strlwr,sprintf) Ignore return value*/
/*lint -e537 Ignore repeated include files                                      */
/*---------------------------------------------------------------------------
** Includes
*/

/*---------------------------------------------------------------------------
** Defines and Macros
*/

#if DEBUG
#define ASSERT(x)   {if (!(x)) { Debug.Error(__FILE__, __LINE__, "Assertion Failed"); }}
#else
#define ASSERT(x)
#endif

#define NUM_ELEMENTS(x)                 (sizeof(x) / sizeof(x[0]))

#define APP_CLASS_NAME                  "GPeditor" "_WinClass"
#define APP_HELPFILE                    "GPeditor.hlp"
#define APP_INIFILE                     "GPeditor.ini"
#define APP_INIFILE_KEYWORD             "Grand Prix Editor"

#define APP_PROF_STR_POSITION_X         "X"
#define APP_PROF_STR_POSITION_Y         "Y"
#define APP_PROF_STR_EXE_FILENAME       "Game"
#define APP_PROF_STR_NAMES_FILENAME     "Names"
#define APP_PROF_STR_CAR_FILENAME       "Car"
#define APP_PROF_STR_HELMET_FILENAME    "Helmet"
#define APP_PROF_STR_COMMENT_FILENAME   "Comment"
#define APP_PROF_STR_EDITOR             "Editor"
#define APP_PROF_STR_GPE_FILENAME       "GPE"
#define APP_PROF_STR_CREDITS            "Credits"
#define APP_PROF_STR_LANGUAGE           "Language"
#define APP_PROF_STR_3D                 "No3D"

#define CONTROL_BAR_CLASS_NAME          "GPedtrCB" "_WinClass"
#define DRAWING_CLASS_NAME              "GPedtrDW" "_WinClass"
#define HELMET_CLASS_NAME               "GPedtrHE" "_WinClass"
#define PALETTE_CLASS_NAME              "GPedtrPL" "_WinClass"
#define TEAMS_CLASS_NAME                "GPedtrTM" "_WinClass"

#define CHILD_ID_CONTROL_BAR            1
#define CHILD_ID_DRAWING                2
#define CHILD_ID_PALETTE                3
#define CHILD_ID_TEAMS                  4
#define CHILD_ID_HELMET                 5

#define APP_MENU_NAME                   "MENU_"
#define APP_ICON_NAME                   "ICON"
#define APP_DIALOG_LANGUAGE             "DLG_LANGUAGE"
#define APP_DIALOG_ABOUT                "DLG_ABOUT"
#define APP_DIALOG_CAR_PERFORMANCE      "DLG_CAR_PERFORMANCE"
#define APP_DIALOG_DRIVER_PERFORMANCE   "DLG_DRIVER_PERFORMANCE"
#define APP_DIALOG_DRIVINGS_AIDS        "DLG_DRIVING_AIDS"
#define APP_DIALOG_CAR_NUMBERS          "DLG_CAR_NUMBERS"
#define APP_DIALOG_GPE_IMP_COMMENT      "DLG_GPE_IMP_COMMENT"
#define APP_DIALOG_GPE_EXP_COMMENT      "DLG_GPE_EXP_COMMENT"
#define APP_DIALOG_NAMES                "DLG_NAMES"
#define APP_DIALOG_AI_CAR_GRIP          "DLG_AI_CAR_GRIP"
#define APP_DIALOG_PLAYERS_CAR_PERF     "DLG_PLAYERS_CAR_PERF"
#define APP_BITMAP_F1_CAR               "BITMAP_F1CAR"
#define APP_BITMAP_F1_CAR_SMALL         "BITMAP_F1CAR_SMALL"
#define APP_BITMAP_HELMET               "BITMAP_HELMET"
#define APP_CURSOR_PALETTE              "CURSOR_PALETTE"
#define APP_CURSOR_SPRAYCAN             "CURSOR_SPRAYCAN"
#define APP_CURSOR_TEAMCAR              "CURSOR_TEAMCAR"

#define GP_EXE_NUM_TEAMS                20
#define GP_EXE_NUM_DRIVERS              (GP_EXE_NUM_TEAMS * 2)

#define NUM_COLOURS_IN_PALETTE          256
#define PALETTE_BOX_ITEM_HEIGHT         15
#define PALETTE_BOX_ITEMS_PER_LINE      32
#define PALETTE_BOX_TOTAL_HEIGHT        ((NUM_COLOURS_IN_PALETTE / PALETTE_BOX_ITEMS_PER_LINE) * PALETTE_BOX_ITEM_HEIGHT)

#define NUM_CAR_REGIONS                 16
#define NUM_HELMET_REGIONS              16

/*---------------------------------------------------------------------------
** Typedefs
*/

enum LANGUAGE {
     LANG_NOT_SUPPORTED  = 0,
     LANG_ENGLISH        = 1,
     LANG_FRENCH         = 2,
     LANG_GERMAN         = 3,
     LANG_LAST           = LANG_GERMAN
};

typedef struct {
     unsigned short power;
} GP_EXE_CAR_POWER;

typedef struct {
     unsigned char  number;
} GP_EXE_CAR_NUMBER;

typedef struct {
     unsigned char  qualifying;
} GP_EXE_DRIVER_QUALIFYING;

typedef struct {
     unsigned char  race;
} GP_EXE_DRIVER_RACE;

typedef struct {
     char           pling;
     unsigned char  colours[15];
} GP_EXE_CAR_COLOURS;

typedef struct {
     unsigned char  driver01[16];
     unsigned char  driver02[16];
     unsigned char  driver03[16];
     unsigned char  driver04[16];
     unsigned char  driver05[16];
     unsigned char  driver06[16];
     unsigned char  driver07[16];
     unsigned char  driver08[16];
     unsigned char  driver09[16];
     unsigned char  driver10[16];
     unsigned char  driver11[16];
     unsigned char  driver12[16];
     unsigned char  driver13[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver14[16];  
     unsigned char  driver15[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver16[16];
     unsigned char  driver17[16];
     unsigned char  driver18[16];
     unsigned char  driver19[16];
     unsigned char  driver20[16];
     unsigned char  driver21[16];
     unsigned char  driver22[16];
     unsigned char  driver23[16];
     unsigned char  driver24[16];
     unsigned char  driver25[16];
     unsigned char  driver26[16];
     unsigned char  driver27[16];
     unsigned char  driver28[16];
     unsigned char  driver29[16];
     unsigned char  driver30[16];
     unsigned char  driver31[16];
     unsigned char  driver32[16];
     unsigned char  driver33[16];
     unsigned char  driver34[16];
     unsigned char  driver35[16];
     unsigned char  driver36[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver37[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver38[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver39[14];  /* Packed record, only use first 6 cols */
     unsigned char  driver40[14];  /* Packed record, only use first 6 cols */
} GP_EXE_HELMET_COLOURS;

typedef struct {
     char           driver[24];
} GP_NAME_DRIVER;

typedef struct {
     char           team[13];
} GP_NAME_TEAM;

typedef struct {
     char           engine[13];
} GP_NAME_ENGINE;

typedef struct {
     GP_NAME_DRIVER drivers[GP_EXE_NUM_DRIVERS];
     GP_NAME_TEAM   teams[GP_EXE_NUM_TEAMS];
     GP_NAME_ENGINE engines[GP_EXE_NUM_TEAMS];
     unsigned short crc1;
     unsigned short crc2;
} GP_NAMES;

class ControlBar;
class DrawingWindow;
class HelmetWindow;
class PaletteWindow;
class TeamsWindow;

/*lint -esym(1712,AppWindow) Ignore no default constructor */

class AppWindow : public MainWindow {
public:
     AppWindow(HINSTANCE instance, HINSTANCE prev_instance, LPSTR class_name, LPSTR app_name, DWORD window_style = 0);
     ~AppWindow();

private:
     void Destroy();
     BOOL UserEventHandler(Event *event);
     BOOL UnknownSysEventHandler(Event *event, long _FAR *r_val_p);

     void UpdateMenu(void);

     void InitDefaultCarNumbers(void);
     void InitDefaultNames(void);
     void InitDefaultFileNames(void);
     BOOL InitResources(void);
     void InitHelpSystem(void);
     BOOL InitColourPalette(void);
     BOOL InitRegisterClasses(HINSTANCE hPrevInstance);
     void InitMenu(void);
     BOOL InitCreateChildWindows(void);

     void Language(void);
     void Exit(void);
     BOOL FileOpenGameExe(char *filename = NULL);
     BOOL IsGameExe(FILE *fp);
     BOOL IsGameExeColoursPresent(void);
     BOOL ReadPerformance(FILE *fp);
     BOOL ReadHelmetColours(FILE *fp);
     BOOL ReadDrivingAids(FILE *fp);
     BOOL ReadCarNumbers(FILE *fp);
     BOOL ReadAiCarGrip(FILE *fp);
     BOOL ReadPlayersCarPerf(FILE *fp);

     BOOL FileUpdateGameExe(void);
     BOOL FileOpenNames(char *filename = NULL);
     BOOL FileSaveNames();
     BOOL ImportGpe(void);
     BOOL ExportGpe(void);
     BOOL GpeReadHeader(FILE *fp);
     BOOL GpeRead(FILE *fp);
     BOOL GpeWrite(FILE *fp);
     void DesignNew(void);
     BOOL DesignLoad(void);
     BOOL DesignSave(void);
     void HelmetNew(void);
     BOOL HelmetLoad(void);
     BOOL HelmetSave(void);
     void UpdateCaption(char *szCaption);
     void SwitchEditor(void);
     LANGUAGE DetermineLanguage(void);
     void SelectNewMenu(void);

     void CarPerformance(void);
     void DriverPerformance(void);
     void DrivingAids(void);
     void CarNumbers(void);
     void AiCarGrip(void);
     void PlayersCarPerformance(void);
     void Names(void);
     void About(void);

     ResourceMenu   *pResourceMenu;
     ControlBar     *pControlBar;
     DrawingWindow  *pDrawingWindow;
     HelmetWindow   *pHelmetWindow;
     PaletteWindow  *pPaletteWindow;
     TeamsWindow    *pTeamsWindow;

     ProfileString  *pProfile;

     int            frame_height;
     int            frame_width;
     BOOL           fImageIsDirty;

     BOOL           fCarEditorEnabled;
     BOOL           fDesignLoaded;
     BOOL           fHelmetLoaded;

     long           lGameExeColourFileOffset;
     long           lGameExeDrivingAidsFileOffset;
     long           lGameExeAiCarGripFileOffset;
     long           lGameExePlayersCarPerfFileOffset;

     char           game_exe_version[32];

     char           msg_buff[128];
     char           tmp_buff[128];
};

typedef struct {
     unsigned char  usRegionColour[NUM_CAR_REGIONS];
} TEAM_COLOURS;

/*---------------------------------------------------------------------------
** Data
*/

extern WinClass     *app_class;
extern AppWindow    *app_window;

extern HPALETTE     hPalette;
extern Bitmap       *pF1CarBitmap;
extern Bitmap       *pF1CarSmallBitmap;
extern Bitmap       *pHelmetBitmap;

extern BOOL         fGpExeIsFound;
extern BOOL         fNamesLoaded;

extern TEAM_COLOURS                team_colours[GP_EXE_NUM_TEAMS];

extern GP_NAMES                    GP_names;
extern GP_EXE_CAR_COLOURS          GP_EXE_car_colours[GP_EXE_NUM_TEAMS];
extern GP_EXE_CAR_POWER            GP_EXE_car_power[GP_EXE_NUM_TEAMS];
extern GP_EXE_CAR_NUMBER           GP_EXE_car_numbers[GP_EXE_NUM_DRIVERS];
extern GP_EXE_DRIVER_QUALIFYING    GP_EXE_driver_qualifying[GP_EXE_NUM_DRIVERS];
extern GP_EXE_DRIVER_RACE          GP_EXE_driver_race[GP_EXE_NUM_DRIVERS];
extern GP_EXE_HELMET_COLOURS       GP_EXE_helmet_colours;
extern unsigned char               GP_EXE_driving_aids[5];
extern unsigned short              GP_EXE_AiCarGrip;
extern unsigned short              GP_EXE_PlayersCarPerf;

extern HINSTANCE    hStringsInstance;
extern HINSTANCE    hErrorInstance;

extern BOOL         help_active;
extern char         help_filename[128];

extern LANGUAGE     CurrentLanguage;

extern char         command_line_param1[128];
extern char         command_line_param2[128];

extern char         szGpExeFilename[128];
extern char         szNamesFilename[128];
extern char         szDesignFilename[128];
extern char         szHelmetFilename[128];
extern char         szGpeFilename[128];
extern char         szCommentFilename[128];

extern BOOL         No3D;

/*---------------------------------------------------------------------------
** Functions
*/

/*---------------------------------------------------------------------------
** Purpose:    Given the direct EXE index for a driver return the car number
**             based index used for the Driver name, Qualifying/Race positions.
**
** Returns:    Car number adjusted index from 0.
**
** Notes  :    Don't call this if IsDriverEnabled() returns FALSE.
*/
int
DriverIndex(
     unsigned short exe_index           /* In  Direct EXE index            */
);

/*---------------------------------------------------------------------------
** Purpose:    Given the direct EXE index for a driver whose car number is
**             zero return the n'th free entry.
**
** Returns:    Car number adjusted index from 0.
**
** Notes  :    Only call this if IsDriverEnabled() returns FALSE.
*/
int
DriverDisabledIndex(
     unsigned short exe_index           /* In  Direct EXE index            */
);

/*---------------------------------------------------------------------------
** Purpose:    To determine if a car index entry is currently enabled via
**             its car number.
**
** Returns:    True if enabled, False if not.
**
** Notes  :    
*/
BOOL
IsDriverEnabled(
     unsigned short exe_index           /* In  Direct EXE index            */
);

/*---------------------------------------------------------------------------
** Purpose:    To determine if a both car index entries are currently enabled
**             for a team via the car numbers.
**
** Returns:    True if enabled, False if not.
**
** Notes  :    
*/
BOOL
IsTeamEnabled(
     unsigned short exe_index           /* In  Direct EXE index            */
);

/*---------------------------------------------------------------------------
** Purpose:    To obtain the specified string.
**
** Returns:    A pointer to the recovered string (allocated in single static)
**
** Notes  :    An error string is inserted if the IDT can't be found.
*/
char *
GetIdt(
     unsigned short idt                 /* In  String to recover           */
);

/*---------------------------------------------------------------------------
** Purpose:    Convert a filter specification which uses '|' to represet
**             nulls ('\0').
**
** Returns:    Nothing.
**
** Notes  :    
*/
void
ConvertFilterSpec(
     char *filter                       /* I/O  Filter spec to convert     */
);

/*---------------------------------------------------------------------------
** Purpose:    Obtains the language specific dialog resource template name.
**
** Returns:    Point to language specific dialog template.
**
** Notes  :    
*/
char *
GetDlgTemplateName(
     const char     *template_name      /* In  Dialog resource name        */
);

/*---------------------------------------------------------------------------
** Purpose:    Get application name adjusted for current language.
**
** Returns:    Application title.
**
** Notes  :    
*/
char *
GetAppName(
     void
);

/*---------------------------------------------------------------------------
** Purpose:    To display a fatal error message and terminate the application.
**
** Returns:    Nothing.
**
** Notes  :    Attempts to minimise use of memory as it may have run out!
*/
void
FatalError(
     unsigned short idt                 /* In  Error IDT string            */
);

/*---------------------------------------------------------------------------
** Purpose:    Attempt to load CTL3D.DLL.
**
** Returns:    True if OK, false if not.
**
** Notes  :    
*/
BOOL
UtlLoad3D(
     void
);

void
UtlUnload3D(
     void
);

void
UtlCtl3dRegister(
     HINSTANCE hInstance
);

void
UtlCtl3dAutoSubclass(
     HINSTANCE hInstance
);

void
UtlCtl3dUnregister(
     HINSTANCE hInstance
);

void
UtlCtl3dColorChange(
     void
);

#endif    /* GPeditor_hpp */
/*---------------------------------------------------------------------------
** End of File
*/





