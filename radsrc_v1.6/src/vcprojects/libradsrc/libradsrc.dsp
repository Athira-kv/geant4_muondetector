# Microsoft Developer Studio Project File - Name="libradsrc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libradsrc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libradsrc.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libradsrc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /TP /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libradsrc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /TP /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libradsrc - Win32 Release"
# Name "libradsrc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;cc"
# Begin Source File

SOURCE=..\..\libradsrc\batemansolution.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\cpp_api.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\dbmanager.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\decaycomputer.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\decaymode.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\elements.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\fortran.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\isotope.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\isotopedb.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\isotopemix.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\legacydata.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\legacyfiles.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\legacyglue.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\photoncomputer.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\radsource.cc
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\spline.cc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\libradsrc\batemansolution.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\cpp_api.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\dbmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\decaycomputer.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\decaymode.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\elements.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\isotope.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\isotopedata.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\isotopedb.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\isotopemix.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\legacydata.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\legacyfiles.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\legacyglue.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\photoncomputer.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\porting.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\radsource.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\radsrcerr.h
# End Source File
# Begin Source File

SOURCE=..\..\libradsrc\spline.h
# End Source File
# End Group
# End Target
# End Project
