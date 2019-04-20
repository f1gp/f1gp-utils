/*
** $Header:   L:/GPEDITOR/VCS/VERSION.IP_   1.10   24 Jun 1995 11:37:00   tk  $
**
** Change History:
**              
** V1.0 15/01/94    First cut.
** V2.0 24/04/94    Scan for colour table now only looks for 75 % match,
**                  due to F1ED splating the identifier byte for 19th team.
**                  Added private GPEDITOR.INI. Old WIN.INI deleted.
**                  Added DTA import/export (F1ED V1.2 and V1.3 supported).
**                  Driver #13 helmet may now be edited. 
**                  Linked packed helmet colour regions for drivers #13 & #15.
**                  GP.EXE and *.NAM files may now be specified as command 
**                  line parameters.
**                  Car Performance up to 999 BHP.
**                  Car Performance "Scroll All" and "Equal BHP" added.
**                  Pop-up About Box very first time application is run.
**                  Now knows about US V1.04/V1.05 GP.EXE.
**                  Windows 3.1 only. 3-D Control/dialogs added (CTL3D.DLL
**                  doesn't need to be present).
**                  Auto filename extension append (W3.1 common File dialog).
**                  Driving Aids dialog.
**                  Car number editor.
**                  All 40 cars and drivers now supported.
**                  Translations added for French and German.
**                  Load/Save F1SA.GPE - All patch information.
**                  Drag and Drop.
**                  Toolbar.
**                  AI cars grip.
**                  Names editor (allows all characters).
**                  Player's car performance.
** V3.0 Skipped
** V4.0 25/09/94    Player's BHP now included in GPE.
**                  Fixed memory leak for StaticText in About dialog.
**                  Fixed memory leak for GPE comment display.
**                  Added AIG & BHP toolbar buttons.
**                  Import GPE wasn't closing open file.
** V4.1 23-Jun-95   Added support for Italian V1.05.
**                  Fixed automatic language selection bug at startup.
**                  Initial credits show every time version number changes.
**                  Fixed bug where an export GPE operation wouldn't save the
**                  current colours unless an "Update Game EXE" had been
**                  performed first.
*/

#define VERSION          "V4.1 23rd June 1995"
#define VERSION_MAJOR    4
#define VERSION_MINOR    1

#define VERSION_WORD     ((VERSION_MAJOR * 100) + VERSION_MINOR)
