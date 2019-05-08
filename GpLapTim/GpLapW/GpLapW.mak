# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=GpLapW - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to GpLapW - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "GpLapW - Win32 Debug" && "$(CFG)" != "GpLapW - Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "GpLapW.mak" CFG="GpLapW - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GpLapW - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "GpLapW - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GpLapW - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\GpLapW.exe"

CLEAN : 
	-@erase ".\WinDebug\GpLapW.exe"
	-@erase ".\WinDebug\stdafx.obj"
	-@erase ".\WinDebug\GpLapW.obj"
	-@erase ".\WinDebug\GpLapW.pch"
	-@erase ".\WinDebug\mainfrm.obj"
	-@erase ".\WinDebug\GpLapdoc.obj"
	-@erase ".\WinDebug\gplapvw.obj"
	-@erase ".\WinDebug\GpLapW.res"
	-@erase ".\WinDebug\gplapfrm.obj"
	-@erase ".\WinDebug\LapUtl.obj"
	-@erase ".\WinDebug\AboutBox.obj"
	-@erase ".\WinDebug\drivfilt.obj"
	-@erase ".\WinDebug\totallap.obj"
	-@erase ".\WinDebug\loglstbx.obj"
	-@erase ".\WinDebug\GpLapW.ilk"
	-@erase ".\WinDebug\GpLapW.pdb"
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\vc40.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/GpLapW.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/GpLapW.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GpLapW.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /profile
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/GpLapW.pdb" /debug /machine:I386 /out:"$(OUTDIR)/GpLapW.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/GpLapW.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/GpLapdoc.obj" \
	"$(INTDIR)/gplapvw.obj" \
	"$(INTDIR)/gplapfrm.obj" \
	"$(INTDIR)/LapUtl.obj" \
	"$(INTDIR)/AboutBox.obj" \
	"$(INTDIR)/drivfilt.obj" \
	"$(INTDIR)/totallap.obj" \
	"$(INTDIR)/loglstbx.obj" \
	"$(INTDIR)/GpLapW.res"

"$(OUTDIR)\GpLapW.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GpLapW - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\GpLapW.exe"

CLEAN : 
	-@erase ".\WinRel\GpLapW.exe"
	-@erase ".\WinRel\stdafx.obj"
	-@erase ".\WinRel\GpLapW.obj"
	-@erase ".\WinRel\GpLapW.pch"
	-@erase ".\WinRel\mainfrm.obj"
	-@erase ".\WinRel\GpLapdoc.obj"
	-@erase ".\WinRel\gplapvw.obj"
	-@erase ".\WinRel\GpLapW.res"
	-@erase ".\WinRel\gplapfrm.obj"
	-@erase ".\WinRel\LapUtl.obj"
	-@erase ".\WinRel\AboutBox.obj"
	-@erase ".\WinRel\drivfilt.obj"
	-@erase ".\WinRel\totallap.obj"
	-@erase ".\WinRel\loglstbx.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/GpLapW.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/GpLapW.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GpLapW.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /profile
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/GpLapW.pdb" /machine:I386 /out:"$(OUTDIR)/GpLapW.exe" 
LINK32_OBJS= \
	"$(INTDIR)/stdafx.obj" \
	"$(INTDIR)/GpLapW.obj" \
	"$(INTDIR)/mainfrm.obj" \
	"$(INTDIR)/GpLapdoc.obj" \
	"$(INTDIR)/gplapvw.obj" \
	"$(INTDIR)/gplapfrm.obj" \
	"$(INTDIR)/LapUtl.obj" \
	"$(INTDIR)/AboutBox.obj" \
	"$(INTDIR)/drivfilt.obj" \
	"$(INTDIR)/totallap.obj" \
	"$(INTDIR)/loglstbx.obj" \
	"$(INTDIR)/GpLapW.res"

"$(OUTDIR)\GpLapW.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

MTL_PROJ=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "GpLapW - Win32 Debug"
# Name "GpLapW - Win32 Release"

!IF  "$(CFG)" == "GpLapW - Win32 Debug"

!ELSEIF  "$(CFG)" == "GpLapW - Win32 Release"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_CPP_STDAF=\
	".\stdafx.h"\
	

!IF  "$(CFG)" == "GpLapW - Win32 Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W4 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/GpLapW.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GpLapW.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GpLapW - Win32 Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W4 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/GpLapW.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GpLapW.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GpLapW.cpp
DEP_CPP_GPLAP=\
	".\stdafx.h"\
	".\GPLAPW.H"\
	".\mainfrm.h"\
	".\gplapfrm.h"\
	".\gplapdoc.h"\
	".\gplapvw.h"\
	".\aboutbox.h"\
	".\LapUtl.h"\
	".\LOGLSTBX.H"\
	".\GPLAPTIM.H"\
	

"$(INTDIR)\GpLapW.obj" : $(SOURCE) $(DEP_CPP_GPLAP) "$(INTDIR)"\
 "$(INTDIR)\GpLapW.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_CPP_MAINF=\
	".\stdafx.h"\
	".\GPLAPW.H"\
	".\mainfrm.h"\
	".\aboutbox.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\GpLapW.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GpLapdoc.cpp
DEP_CPP_GPLAPD=\
	".\stdafx.h"\
	".\GPLAPW.H"\
	".\gplapdoc.h"\
	".\aboutbox.h"\
	".\LapUtl.h"\
	".\GPLAPTIM.H"\
	

"$(INTDIR)\GpLapdoc.obj" : $(SOURCE) $(DEP_CPP_GPLAPD) "$(INTDIR)"\
 "$(INTDIR)\GpLapW.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gplapvw.cpp
DEP_CPP_GPLAPV=\
	".\stdafx.h"\
	".\GPLAPW.H"\
	".\gplapdoc.h"\
	".\gplapvw.h"\
	".\LapUtl.h"\
	".\WaitCur.h"\
	".\drivfilt.h"\
	".\totallap.h"\
	".\aboutbox.h"\
	".\LOGLSTBX.H"\
	".\GPLAPTIM.H"\
	

"$(INTDIR)\gplapvw.obj" : $(SOURCE) $(DEP_CPP_GPLAPV) "$(INTDIR)"\
 "$(INTDIR)\GpLapW.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GpLapW.rc
DEP_RSC_GPLAPW=\
	".\res\GpLapW.ico"\
	".\res\GpLapdoc.ico"\
	".\res\toolbar.bmp"\
	".\res\SENNABW.bmp"\
	".\res\FL.BMP"\
	".\res\FLMASK.BMP"\
	".\res\Q1.BMP"\
	".\res\Q1MASK.BMP"\
	".\res\BL.BMP"\
	".\res\BLMASK.BMP"\
	".\res\P1FL.BMP"\
	".\res\P1MASK.BMP"\
	".\res\P1BL.BMP"\
	".\res\P1.BMP"\
	".\res\RW.BMP"\
	".\res\RWMASK.BMP"\
	".\res\AUTH.BMP"\
	".\res\AUTHMASK.BMP"\
	".\res\AUTHRACE.BMP"\
	".\res\GpLapW.rc2"\
	

"$(INTDIR)\GpLapW.res" : $(SOURCE) $(DEP_RSC_GPLAPW) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\gplapfrm.cpp
DEP_CPP_GPLAPF=\
	".\stdafx.h"\
	".\GPLAPW.H"\
	".\gplapfrm.h"\
	".\aboutbox.h"\
	

"$(INTDIR)\gplapfrm.obj" : $(SOURCE) $(DEP_CPP_GPLAPF) "$(INTDIR)"\
 "$(INTDIR)\GpLapW.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\LapUtl.cpp
DEP_CPP_LAPUT=\
	".\stdafx.h"\
	".\LapUtl.h"\
	".\GPLAPTIM.H"\
	

"$(INTDIR)\LapUtl.obj" : $(SOURCE) $(DEP_CPP_LAPUT) "$(INTDIR)"\
 "$(INTDIR)\GpLapW.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\AboutBox.cpp
DEP_CPP_ABOUT=\
	".\stdafx.h"\
	".\GPLAPW.H"\
	".\aboutbox.h"\
	

"$(INTDIR)\AboutBox.obj" : $(SOURCE) $(DEP_CPP_ABOUT) "$(INTDIR)"\
 "$(INTDIR)\GpLapW.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\drivfilt.cpp
DEP_CPP_DRIVF=\
	".\stdafx.h"\
	".\GPLAPW.H"\
	".\drivfilt.h"\
	".\aboutbox.h"\
	

"$(INTDIR)\drivfilt.obj" : $(SOURCE) $(DEP_CPP_DRIVF) "$(INTDIR)"\
 "$(INTDIR)\GpLapW.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\totallap.cpp
DEP_CPP_TOTAL=\
	".\stdafx.h"\
	".\GPLAPW.H"\
	".\totallap.h"\
	".\aboutbox.h"\
	

"$(INTDIR)\totallap.obj" : $(SOURCE) $(DEP_CPP_TOTAL) "$(INTDIR)"\
 "$(INTDIR)\GpLapW.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\loglstbx.cpp
DEP_CPP_LOGLS=\
	".\stdafx.h"\
	".\GPLAPW.H"\
	".\gplapvw.h"\
	".\LOGLSTBX.H"\
	".\LapUtl.h"\
	".\aboutbox.h"\
	".\gplapdoc.h"\
	".\GPLAPTIM.H"\
	

"$(INTDIR)\loglstbx.obj" : $(SOURCE) $(DEP_CPP_LOGLS) "$(INTDIR)"\
 "$(INTDIR)\GpLapW.pch"


# End Source File
# End Target
# End Project
################################################################################
