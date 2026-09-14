# Microsoft Developer Studio Generated NMAKE File, Based on radsrc.dsp
!IF "$(CFG)" == ""
CFG=radsrc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to radsrc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "radsrc - Win32 Release" && "$(CFG)" != "radsrc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "radsrc.mak" CFG="radsrc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "radsrc - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "radsrc - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "radsrc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\radsrc.exe"

!ELSE 

ALL : "libradsrc - Win32 Release" "$(OUTDIR)\radsrc.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libradsrc - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\bins.obj"
	-@erase "$(INTDIR)\mcinput.obj"
	-@erase "$(INTDIR)\outputmci.obj"
	-@erase "$(INTDIR)\radsrc.obj"
	-@erase "$(INTDIR)\useriface.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\radsrc.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\libradsrc" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\radsrc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\radsrc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\radsrc.pdb" /machine:I386 /out:"$(OUTDIR)\radsrc.exe" 
LINK32_OBJS= \
	"$(INTDIR)\bins.obj" \
	"$(INTDIR)\mcinput.obj" \
	"$(INTDIR)\outputmci.obj" \
	"$(INTDIR)\radsrc.obj" \
	"$(INTDIR)\useriface.obj" \
	"..\libradsrc\Release\libradsrc.lib"

"$(OUTDIR)\radsrc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "radsrc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\radsrc.exe"

!ELSE 

ALL : "libradsrc - Win32 Debug" "$(OUTDIR)\radsrc.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libradsrc - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\bins.obj"
	-@erase "$(INTDIR)\mcinput.obj"
	-@erase "$(INTDIR)\outputmci.obj"
	-@erase "$(INTDIR)\radsrc.obj"
	-@erase "$(INTDIR)\useriface.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\radsrc.exe"
	-@erase "$(OUTDIR)\radsrc.ilk"
	-@erase "$(OUTDIR)\radsrc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\libradsrc" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\radsrc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /TP /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\radsrc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\radsrc.pdb" /debug /machine:I386 /out:"$(OUTDIR)\radsrc.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\bins.obj" \
	"$(INTDIR)\mcinput.obj" \
	"$(INTDIR)\outputmci.obj" \
	"$(INTDIR)\radsrc.obj" \
	"$(INTDIR)\useriface.obj" \
	"..\libradsrc\Debug\libradsrc.lib"

"$(OUTDIR)\radsrc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("radsrc.dep")
!INCLUDE "radsrc.dep"
!ELSE 
!MESSAGE Warning: cannot find "radsrc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "radsrc - Win32 Release" || "$(CFG)" == "radsrc - Win32 Debug"
SOURCE=..\..\radsrc\bins.cc

"$(INTDIR)\bins.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\radsrc\mcinput.cc

"$(INTDIR)\mcinput.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\radsrc\outputmci.cc

"$(INTDIR)\outputmci.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\radsrc\radsrc.cc

"$(INTDIR)\radsrc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\radsrc\useriface.cc

"$(INTDIR)\useriface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "radsrc - Win32 Release"

"libradsrc - Win32 Release" : 
   cd "\Documents and Settings\hiller2\My Documents\cygwin\srcvc\tmp\dev\src\vcprojects\libradsrc"
   $(MAKE) /$(MAKEFLAGS) /F ".\libradsrc.mak" CFG="libradsrc - Win32 Release" 
   cd "..\radsrc"

"libradsrc - Win32 ReleaseCLEAN" : 
   cd "\Documents and Settings\hiller2\My Documents\cygwin\srcvc\tmp\dev\src\vcprojects\libradsrc"
   $(MAKE) /$(MAKEFLAGS) /F ".\libradsrc.mak" CFG="libradsrc - Win32 Release" RECURSE=1 CLEAN 
   cd "..\radsrc"

!ELSEIF  "$(CFG)" == "radsrc - Win32 Debug"

"libradsrc - Win32 Debug" : 
   cd "\Documents and Settings\hiller2\My Documents\cygwin\srcvc\tmp\dev\src\vcprojects\libradsrc"
   $(MAKE) /$(MAKEFLAGS) /F ".\libradsrc.mak" CFG="libradsrc - Win32 Debug" 
   cd "..\radsrc"

"libradsrc - Win32 DebugCLEAN" : 
   cd "\Documents and Settings\hiller2\My Documents\cygwin\srcvc\tmp\dev\src\vcprojects\libradsrc"
   $(MAKE) /$(MAKEFLAGS) /F ".\libradsrc.mak" CFG="libradsrc - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\radsrc"

!ENDIF 


!ENDIF 

