# Microsoft Developer Studio Generated NMAKE File, Based on libradsrc.dsp
!IF "$(CFG)" == ""
CFG=libradsrc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libradsrc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libradsrc - Win32 Release" && "$(CFG)" != "libradsrc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libradsrc.mak" CFG="libradsrc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libradsrc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libradsrc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "libradsrc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\libradsrc.lib"


CLEAN :
	-@erase "$(INTDIR)\batemansolution.obj"
	-@erase "$(INTDIR)\cpp_api.obj"
	-@erase "$(INTDIR)\dbmanager.obj"
	-@erase "$(INTDIR)\decaycomputer.obj"
	-@erase "$(INTDIR)\decaymode.obj"
	-@erase "$(INTDIR)\elements.obj"
	-@erase "$(INTDIR)\fortran.obj"
	-@erase "$(INTDIR)\isotope.obj"
	-@erase "$(INTDIR)\isotopedb.obj"
	-@erase "$(INTDIR)\isotopemix.obj"
	-@erase "$(INTDIR)\legacydata.obj"
	-@erase "$(INTDIR)\legacyfiles.obj"
	-@erase "$(INTDIR)\legacyglue.obj"
	-@erase "$(INTDIR)\photoncomputer.obj"
	-@erase "$(INTDIR)\radsource.obj"
	-@erase "$(INTDIR)\spline.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libradsrc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\libradsrc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /TP /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libradsrc.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libradsrc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\batemansolution.obj" \
	"$(INTDIR)\cpp_api.obj" \
	"$(INTDIR)\dbmanager.obj" \
	"$(INTDIR)\decaycomputer.obj" \
	"$(INTDIR)\decaymode.obj" \
	"$(INTDIR)\elements.obj" \
	"$(INTDIR)\fortran.obj" \
	"$(INTDIR)\isotope.obj" \
	"$(INTDIR)\isotopedb.obj" \
	"$(INTDIR)\isotopemix.obj" \
	"$(INTDIR)\legacydata.obj" \
	"$(INTDIR)\legacyfiles.obj" \
	"$(INTDIR)\legacyglue.obj" \
	"$(INTDIR)\photoncomputer.obj" \
	"$(INTDIR)\radsource.obj" \
	"$(INTDIR)\spline.obj"

"$(OUTDIR)\libradsrc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libradsrc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\libradsrc.lib"


CLEAN :
	-@erase "$(INTDIR)\batemansolution.obj"
	-@erase "$(INTDIR)\cpp_api.obj"
	-@erase "$(INTDIR)\dbmanager.obj"
	-@erase "$(INTDIR)\decaycomputer.obj"
	-@erase "$(INTDIR)\decaymode.obj"
	-@erase "$(INTDIR)\elements.obj"
	-@erase "$(INTDIR)\fortran.obj"
	-@erase "$(INTDIR)\isotope.obj"
	-@erase "$(INTDIR)\isotopedb.obj"
	-@erase "$(INTDIR)\isotopemix.obj"
	-@erase "$(INTDIR)\legacydata.obj"
	-@erase "$(INTDIR)\legacyfiles.obj"
	-@erase "$(INTDIR)\legacyglue.obj"
	-@erase "$(INTDIR)\photoncomputer.obj"
	-@erase "$(INTDIR)\radsource.obj"
	-@erase "$(INTDIR)\spline.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libradsrc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\libradsrc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /TP /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libradsrc.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libradsrc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\batemansolution.obj" \
	"$(INTDIR)\cpp_api.obj" \
	"$(INTDIR)\dbmanager.obj" \
	"$(INTDIR)\decaycomputer.obj" \
	"$(INTDIR)\decaymode.obj" \
	"$(INTDIR)\elements.obj" \
	"$(INTDIR)\fortran.obj" \
	"$(INTDIR)\isotope.obj" \
	"$(INTDIR)\isotopedb.obj" \
	"$(INTDIR)\isotopemix.obj" \
	"$(INTDIR)\legacydata.obj" \
	"$(INTDIR)\legacyfiles.obj" \
	"$(INTDIR)\legacyglue.obj" \
	"$(INTDIR)\photoncomputer.obj" \
	"$(INTDIR)\radsource.obj" \
	"$(INTDIR)\spline.obj"

"$(OUTDIR)\libradsrc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("libradsrc.dep")
!INCLUDE "libradsrc.dep"
!ELSE 
!MESSAGE Warning: cannot find "libradsrc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libradsrc - Win32 Release" || "$(CFG)" == "libradsrc - Win32 Debug"
SOURCE=..\..\libradsrc\batemansolution.cc

"$(INTDIR)\batemansolution.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\cpp_api.cc

"$(INTDIR)\cpp_api.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\dbmanager.cc

"$(INTDIR)\dbmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\decaycomputer.cc

"$(INTDIR)\decaycomputer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\decaymode.cc

"$(INTDIR)\decaymode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\elements.cc

"$(INTDIR)\elements.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\fortran.cc

"$(INTDIR)\fortran.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\isotope.cc

"$(INTDIR)\isotope.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\isotopedb.cc

"$(INTDIR)\isotopedb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\isotopemix.cc

"$(INTDIR)\isotopemix.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\legacydata.cc

"$(INTDIR)\legacydata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\legacyfiles.cc

"$(INTDIR)\legacyfiles.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\legacyglue.cc

"$(INTDIR)\legacyglue.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\photoncomputer.cc

"$(INTDIR)\photoncomputer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\radsource.cc

"$(INTDIR)\radsource.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\libradsrc\spline.cc

"$(INTDIR)\spline.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

