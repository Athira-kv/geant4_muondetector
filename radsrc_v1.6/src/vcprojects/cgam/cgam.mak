# Microsoft Developer Studio Generated NMAKE File, Based on cgam.dsp
!IF "$(CFG)" == ""
CFG=cgam - Win32 Debug
!MESSAGE No configuration specified. Defaulting to cgam - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cgam - Win32 Release" && "$(CFG)" != "cgam - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cgam.mak" CFG="cgam - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cgam - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "cgam - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cgam - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cgam.exe"

!ELSE 

ALL : "libradsrc - Win32 Release" "$(OUTDIR)\cgam.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libradsrc - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cgam.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cgam.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\libradsrc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\cgam.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cgam.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\cgam.pdb" /machine:I386 /out:"$(OUTDIR)\cgam.exe" 
LINK32_OBJS= \
	"$(INTDIR)\cgam.obj" \
	"..\libradsrc\Release\libradsrc.lib"

"$(OUTDIR)\cgam.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cgam - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cgam.exe"

!ELSE 

ALL : "libradsrc - Win32 Debug" "$(OUTDIR)\cgam.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libradsrc - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cgam.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\cgam.exe"
	-@erase "$(OUTDIR)\cgam.ilk"
	-@erase "$(OUTDIR)\cgam.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\libradsrc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\cgam.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cgam.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\cgam.pdb" /debug /machine:I386 /out:"$(OUTDIR)\cgam.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\cgam.obj" \
	"..\libradsrc\Debug\libradsrc.lib"

"$(OUTDIR)\cgam.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("cgam.dep")
!INCLUDE "cgam.dep"
!ELSE 
!MESSAGE Warning: cannot find "cgam.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cgam - Win32 Release" || "$(CFG)" == "cgam - Win32 Debug"
SOURCE=..\..\misc\cgam.cc

"$(INTDIR)\cgam.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "cgam - Win32 Release"

"libradsrc - Win32 Release" : 
   cd "\Documents and Settings\hiller2\My Documents\cygwin\srcvc\tmp\dev\src\vcprojects\libradsrc"
   $(MAKE) /$(MAKEFLAGS) /F ".\libradsrc.mak" CFG="libradsrc - Win32 Release" 
   cd "..\cgam"

"libradsrc - Win32 ReleaseCLEAN" : 
   cd "\Documents and Settings\hiller2\My Documents\cygwin\srcvc\tmp\dev\src\vcprojects\libradsrc"
   $(MAKE) /$(MAKEFLAGS) /F ".\libradsrc.mak" CFG="libradsrc - Win32 Release" RECURSE=1 CLEAN 
   cd "..\cgam"

!ELSEIF  "$(CFG)" == "cgam - Win32 Debug"

"libradsrc - Win32 Debug" : 
   cd "\Documents and Settings\hiller2\My Documents\cygwin\srcvc\tmp\dev\src\vcprojects\libradsrc"
   $(MAKE) /$(MAKEFLAGS) /F ".\libradsrc.mak" CFG="libradsrc - Win32 Debug" 
   cd "..\cgam"

"libradsrc - Win32 DebugCLEAN" : 
   cd "\Documents and Settings\hiller2\My Documents\cygwin\srcvc\tmp\dev\src\vcprojects\libradsrc"
   $(MAKE) /$(MAKEFLAGS) /F ".\libradsrc.mak" CFG="libradsrc - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\cgam"

!ENDIF 


!ENDIF 

