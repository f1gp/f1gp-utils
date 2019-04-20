/*
** File   : utils.cpp
** Author : TK 
** Date   : 26/02/94
**
** $Header:   L:/GPEDITOR/VCS/UTILS.CP_   1.3   01 Oct 1994 15:28:02   tk  $
**
** F1 car design editor (for Microprose Grand Prix / World Circuit).
** Utils.
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

/*---------------------------------------------------------------------------
** Defines and Macros
*/

/*---------------------------------------------------------------------------
** Typedefs
*/

typedef void (CALLBACK * FARPROC_HI)(HINSTANCE hInstance);

/*---------------------------------------------------------------------------
** Local function prototypes
*/

static void UtlCallDllHi(const char *routine_name, HINSTANCE hInstance);

/*---------------------------------------------------------------------------
** Data
*/

static HINSTANCE    hInstanceUtl3d;

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
) {
     ASSERT(exe_index < NUM_ELEMENTS(GP_EXE_car_numbers));
     ASSERT(IsDriverEnabled(exe_index));
     return (GP_EXE_car_numbers[exe_index].number - 1);
}

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
) {
     unsigned short i;
     unsigned short disabled_entries;
     unsigned char  entries[NUM_ELEMENTS(GP_EXE_car_numbers)];

     ASSERT(exe_index < NUM_ELEMENTS(GP_EXE_car_numbers));
     ASSERT(!IsDriverEnabled(exe_index));

     disabled_entries = 0;
     for (i = 0; i < NUM_ELEMENTS(GP_EXE_car_numbers); i++) {
          entries[disabled_entries++] = i;
     }

     for (i = 0; i < disabled_entries; i++) {
          if (entries[i] == exe_index) {
               return entries[i];
          }
     }

     #pragma warn -ccc
     ASSERT(FALSE);
     #pragma warn -ccc

     return exe_index;
}

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
) {
     ASSERT(exe_index < NUM_ELEMENTS(GP_EXE_car_numbers));
     return (GP_EXE_car_numbers[exe_index].number != 0);
}

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
) {
     ASSERT(exe_index < GP_EXE_NUM_TEAMS);
     return (  IsDriverEnabled((exe_index * 2) + 0) ||
               IsDriverEnabled((exe_index * 2) + 1)
          );
}

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
) {
     static char    str[256];

     switch (CurrentLanguage) {
     case LANG_NOT_SUPPORTED:
     case LANG_ENGLISH:
     default:
          idt += IDO_ENGLISH;
          break;

     case LANG_FRENCH:
          idt += IDO_FRENCH;
          break;

     case LANG_GERMAN:
          idt += IDO_GERMAN;
          break;
     }

     if (LoadString(hStringsInstance, idt, str, sizeof(str)) == 0) {
          sprintf(str, "<Unable to locate string #%u>", idt);
     }

     return str;
}

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
) {
     char *p = filter;

     for (; *p; p++) {
          if (*p == '|') {
               *p = '\0';
          }
     }
}

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
) {
     static char    buff[80];
     char           v;

     switch (CurrentLanguage) {
     case LANG_NOT_SUPPORTED:
     default:
     case LANG_ENGLISH:
          v = 'E';
          break;

     case LANG_FRENCH:
          v = 'F';
          break;

     case LANG_GERMAN:
          v = 'G';
          break;
     }
     sprintf(buff, "%c_%s", v, template_name);

     return buff;
}

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
) {
     static char    buff[80];

     strcpy(buff, GetIdt(IDT_APP_NAME));
     return buff;
}

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
) {
     static char    msg[256];
     static char    str[128];

     if (LoadString(hErrorInstance, idt, str, sizeof(str)) == 0) {
          strcpy(str, "<Unable to locate error string>");
     }

     sprintf(msg, "Error #%u: %s", idt, str);

     MessageBeep(MB_ICONHAND);
     if (MessageBox(NULL, msg, GetAppName(), MB_ICONSTOP | MB_OK) == 0) {
          if (MessageBox(NULL, msg, GetAppName(), MB_ICONSTOP | MB_OK | MB_SYSTEMMODAL) == 0) {
               MessageBeep(-1);
          }
     }

     PostQuitMessage(0);
}

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
) {
     OFSTRUCT  of;

     /*
     ** Check for DLLs existence before attempt to load it.
     */
     if (OpenFile("CTL3D.DLL", &of, OF_EXIST) == HFILE_ERROR) {
          hInstanceUtl3d = 0;
          return FALSE;
     }

     /*
     ** Load DLL.
     */
     hInstanceUtl3d = LoadLibrary("CTL3D.DLL");

     if (hInstanceUtl3d <= HINSTANCE_ERROR) {
          hInstanceUtl3d = 0;
          return FALSE;
     }

     return TRUE;
}

void
UtlUnload3D(
     void
) {
     if (hInstanceUtl3d) {
          FreeLibrary(hInstanceUtl3d);
          hInstanceUtl3d = 0;
     }
}

void
UtlCtl3dRegister(
     HINSTANCE hInstance
) {
     UtlCallDllHi("Ctl3dRegister", hInstance);
}

void
UtlCtl3dAutoSubclass(
     HINSTANCE hInstance
) {
     UtlCallDllHi("Ctl3dAutoSubclass", hInstance);
}

void
UtlCtl3dUnregister(
     HINSTANCE hInstance
) {
     UtlCallDllHi("Ctl3dUnregister", hInstance);
}

void
UtlCtl3dColorChange(
     void
) {
     if (hInstanceUtl3d) {
          FARPROC   pProcAddr = GetProcAddress(hInstanceUtl3d, "Ctl3dColorChange");

          ASSERT(pProcAddr != NULL);
          if (pProcAddr) {
               FARPROC   pProcInst = MakeProcInstance(pProcAddr, app_window->Instance());

               ASSERT(pProcInst != NULL);
               if (pProcInst) {
                    (*pProcInst)();
                    FreeProcInstance(pProcInst);
               }
          }
     }
}

static void
UtlCallDllHi(
     const char     *routine_name,
     HINSTANCE      hInstance
) {
     if (hInstanceUtl3d) {
          FARPROC   pProcAddr = GetProcAddress(hInstanceUtl3d, routine_name);

          ASSERT(pProcAddr != NULL);
          if (pProcAddr) {
               FARPROC_HI     pProcInst = (FARPROC_HI) MakeProcInstance(pProcAddr, hInstance);

               ASSERT(pProcInst != NULL);
               if (pProcInst) {
                    (*pProcInst)(hInstance);
                    FreeProcInstance((FARPROC) pProcInst);
               }
          }
     }
}

/*---------------------------------------------------------------------------
** End of File
*/

