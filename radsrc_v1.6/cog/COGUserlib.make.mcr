# This is the standard distribution COG usrdet/usrsor makefile.
# Makes the shared (runtime) library COGUserlib.so (.sl on HP),
# containing the user-written subroutine(s).
# The user-written subroutine can have any name less than 9
# characters long.

#-----------Modification Record------------------------------
# 6/18/02 R. Buck
# Added command-line argument, in=name1.F,name2.F,...
# where:
#  name1.F,name2.F,... are the names of usrdets/usrsors to be
#  compiled and saved in the runtime library COGUserlib.so.
# Added Perl script getinc, which gets the include file pathname
# and writes it to file ifile, which is subsequently read (included)
# in the make script.
# getinc uses COGwhere.ss to find the COG directory, so if the user
#  has not set a path to the COG directory, this will fail.

# 8/6/01 R. Buck
# Added platform PCLinux
# 1/31/01 R. Buck
# Eliminated unwanted usrdet.f file, if any found at make time.
# 12/8/98 R. Buck
# Added  -bexpall -berok -brtl /lib/crt0.o (for run-time linking of usrdets) 
#  to LD_OPTIONS for IBM AIX.
# 4/22/97 R. Buck
# Usrdets can now be named.
#-----------------------------------------------------------------

#---Making the runtime library COGUserlib.so------
# Type:
#  gmake -f COGUserlib.make in=usrdet1.F,usrdet2.F,usrsor1.F,...
# where: usrdet1.F,usrdet2.F,usrsor1.F... are the names of the usrdet or usrsor
#   routines you wish to have compiled and saved in the runtime library COGUserlib.so.
# Note that you may have an arbitrary number of different usrdets/usrsors in one
# COGUserlib.so runtime library. When COG is run, the USRDET (or USRSOR) lines in
# the input file selects the desired routine.
#--------------------------------------------------

#------Include File Note------
# The make file has a method for locating the usrdet/usrsor include
# files (and, if needed, the COG library files), which are stored in the
# directory /usrdet/include (/usrdet/lib), found in
# the COG Distribution directory. If you haven't set the shell PATH
# variable to include the path to the COG Distribution directory,
# this will fail.
#------------------------------

#------PC/LINUX Note------
# If the usrdet accesses any of the COG routines, then you must specify
# the COG binary library path in script variable LIBPATH.
# In this makefile, it is assumed that this path is:
#  /usr/gapps/cog/Linux/usrdet/libo
# If otherwise, change the " LIBPATH = ..." line accordingly in the
# PCLinux section of this script.
#------------------------------
	
# Run perl script "getinc" to get the include and lib pathnames, and write them
#  into file ifile.
	DUM := $(shell perl getinc > ifile)
# Read (by inclusion) the include pathname in ifile.
# Path is:
#  I = /usr/gapps/cog/usrdet/include  , for example.
include ifile

   DUM := $(shell echo "ifile = $I" >& 2)

ifndef I
	DUM := $(shell echo "Include files not found" >& 2)
	DUM := $(shell echo "You must set shell PATH variable to include" >& 2)
	DUM := $(shell echo " the path to the COG Distribution directory" >& 2)
	override in := NONE
endif	
ifndef in
	DUM := $(shell echo "make line is:" >& 2)
	DUM := $(shell echo "  gmake -f COGUserlib.make in=usrdet1.F,usrsor1.F,..." >& 2)
	DUM := $(shell echo "where usrdet1.F,usrsor1.F,... are the names of usrdets and" >& 2)
	DUM := $(shell echo "usrsors to be compiled and stored in COGUserlib.so" >& 2)
	DUM := $(shell echo "  " >& 2)
	in := NONE
endif
	DUM  := $(shell echo "Files to make are $(in)" >& 2)
	in1 := $(shell echo $(in)|sed s/,/\ /)
	OBJS := $(shell echo $(in1:.F=.o) )
	DUM  := $(shell echo "Objects are $(OBJS)" >& 2)
	DUM  := $(shell echo "Include file path is $I" >& 2)
	DUM  := $(shell echo "Library file path is $(LIBPATH)" >& 2)

SHELL = /bin/sh
curdir = $(shell pwd)
platform = $(shell uname)
nodename = $(shell uname -n)
ifneq (,$(findstring CYGWIN,$(platform)))
	platform = WINTELPC
endif
ifeq (Linux,$(platform))
 platform = IntelLinux
# I = /usr/local/apps/cog/usrdet/include
endif
ifneq (,$(findstring ilx,$(nodename)))
 platform = ILX
# I = /usr/gapps/cog/ILX/usrdet/include
endif
	PLAT := $(shell echo Platform is $(platform) >& 2)
	NODE := $(shell echo Nodename is $(nodename) >& 2)
	MACH := $(shell echo CPU is $(machine) >& 2)
host = $(shell uname -n)
override FFCPPFLAGS += -I.
cogroot = $(dir $(COGREL))
override FFCPPFLAGS += -DLANGUAGE_FORTRAN
CPP = cpp

ifeq (HP-UX, $(platform))
####insert HP specifics here####
# Set the f77 compiler flags for position-independent code (+z or +Z)
FFC = fort77
FFLAGS=  -I$I -g +z
TARGET = COGUserlib.sl
LDOPTS = -b  -t -o $(TARGET)
CPP = /lib/cpp
override CPPFLAGS += -DHPUX
#I = /usr/gapps/cog/HP/usrdet/include

else
ifeq (SunOS, $(platform))
####insert Sun specifics here###
# Set the f77 compiler flags for position-independent code (-pic)
# Change -O to -g below to use f77 debugger.
#
FFC = f77
FFLAGS=  -O -pic -I$I
TARGET = COGUserlib.so
LDOPTS = -dy -G -h $(TARGET) -z text -o $(TARGET)
CPP = /usr/lib/cpp
override CPPFLAGS += -DSunOS
# I = /usr/gapps/cog/SunOS/usrdet/include
else

####SGI#####################
ifeq (IRIX64, $(platform))
#insert SGI (hyper) specifics here
# "Make" machine is hyper.
FFC = f77
FC = f77
FFLAGS=  -O  -I$I -c
TARGET = COGUserlib.so
CPP := /usr/lib/cpp
override CPPFLAGS += -DIRIX64
LDOPTS := -call_shared -aoutkeep -U -o $(TARGET)

else
ifeq (OSF1, $(platform))
####insert Dec specifics here###
# Set the f77 compiler flags for position-independent code (-pic)
# Change -O to -g below to use f77 debugger.
#
FFC = f90
FC = f90
FFLAGS=  -O  -I$I
TARGET = COGUserlib.so
LDOPTS = -shared -o $(TARGET) # -all 
override CPPFLAGS += -DOSF1
# I = /usr/gapps/cog/Dec/usrdet/include
else

ifeq (AIX, $(platform))
###insert IBM specifics here###
# Set the f77 compiler flags for position-independent code (-pic)
# Change -O to -g below to use f77 debugger.
#
FFC = xlf
FC = xlf
CPP := /usr/ccs/lib/cpp
FFLAGS=  -I$I -O 
TARGET = COGUserlib.so
LDOPTS = -bexpall -berok -brtl /lib/crt0.o -lxlf90 -lxlf -lm -lc -o $(TARGET)
override CPPFLAGS += -DIBM  -P
#I = /usr/gapps/cog/Blue/usrdet/include

else

ifeq (WINTELPC, $(platform))
###insert WINTELPC specifics here###
	FFC = pgf90
	CPP = pgf90
    TARGET = COGUserlib.so
	override OUT += -o # Linux requires output destination for preprocessor
    override FFCPPFLAGS = 
	override FFCPPFLAGS :=  -g -v
	override FFLAGS += -fpic -tp px  -Kieee # compile for either p5 or p6 
	override CPPFLAGS +=  -F -DPCLinux -DSunOS #
#  Stop compiler after preprocessing step.
	COMP := $(FFC)
	COMPFLAGS := $(FFCFLAGS) -g77libs  # allows linking with files that
#    LIBPATH = "/Documents and Settings/buck/cog10/WINTELPC/lib"
    LDOPTS = -L$(LIBPATH) -lGeom -lDet -lMem -shared -o $(TARGET) # Load beep
else

ifeq (IntelLinux, $(platform))
###insert IntelLinux specifics here###
# Must use first: source /usr/local/pgi5/linux86/5.1/bin/startpgi.csh
	FFC = pgf90
	CPP = pgf90
    TARGET = COGUserlib.so
	override OUT += -o # Linux requires output destination for preprocessor
    override FFCPPFLAGS = 
	override FFCPPFLAGS :=  -g -v
	override FFLAGS += -fpic -tp px  -Kieee # compile for either p5 or p6 
	override CPPFLAGS +=  -F -DPCLinux -DSunOS #
#  Stop compiler after preprocessing step.
	COMP := $(FFC)
	COMPFLAGS := $(FFCFLAGS) -g77libs  # allows linking with files that
#    LIBPATH = /usr/local/apps/cog/usrdet/lib
    LDOPTS = -L$(LIBPATH)  -Bstatic -shared -lGeom -lDet -lMem \
     -lMPI -lRW -lLF -o $(TARGET) -L$(RADSRC_HOME)/lib -lradsrc -lC -lstd -lpgsse2
else

ifeq (ILX, $(platform))
###insert ILX specifics here###
# FOR MCR:
# MUST USE FIRST: source /usr/local/pgi/linux86/bin/startpgi.csh
# TO FIND PGF COMPILERS.
	FFC = pgf90
	CPP = pgf90
    TARGET = COGUserlib.so
	override OUT += -o # Linux requires output destination for preprocessor
    override FFCPPFLAGS = 
	override FFCPPFLAGS :=  -g -v
	override FFLAGS += -fpic -shared 
	override CPPFLAGS +=  -F -DPCLinux -DSunOS #
#  Stop compiler after preprocessing step.
	COMP := $(FFC)
	COMPFLAGS := $(FFCFLAGS) -g77libs  # allows linking with files that
#    LIBPATH = /usr/local/apps/cog/usrdet/lib
    LDOPTS = -L$(LIBPATH) -lGeom -lDet -lMem -shared -o $(TARGET) # Load beep
endif
endif
endif
endif
endif
endif
endif
endif

$(TARGET): $(OBJS)
	@echo "Platform is $(platform)"
ifeq (IntelLinux, $(platform))
	$(FFC) $(FFLAGS) $(OBJS) $(LIBPATH)/LUFx.o $(LDOPTS)
	rm $(OBJS)
else
ifeq (PCLinux, $(platform))
	$(FFC) $(FFLAGS) $(OBJS) $(LDOPTS)
else
ifeq (WINTELPC, $(platform))
	$(FFC) $(FFLAGS) $(OBJS) $(LDOPTS)
else
ifeq (SunOS, $(platform))
	$(FFC) $(FFLAGS) $(OBJS) $(LDOPTS)
else
ifeq (ILX, $(platform))
	$(FFC) $(FFLAGS) $(OBJS) $(LDOPTS)
else
	/usr/bin/ld $(LDOPTS) $(OBJS)
endif
endif
endif
endif
endif
	@echo ' '
	@echo '   Usrdet runtime library $(TARGET) written.'
	@echo '  Contains usrdet routines: $(OBJS).'

.F.o:
	@echo ">>> Compiling F to o for $<" #; if [ -f $(*F).f ]; then \
#	          rm $(*F).f; fi
	$(CPP) -I$I $(FFCPPFLAGS) $(CPPFLAGS) $< $(OUT) $(*F).f
	$(FFC) $(FFLAGS) -c $(*F).f -o $*.o
	rm $(*F).f
	-rm ifile

# This next section removes troublesome .f files, perhaps left from
#  a previous make or compilation.	
.f.o:
	-rm $< $@	
	@/bin/echo "\a\a"
	@echo '**Rerun makefile to compile usrdet.F file'
	@echo ' '
	rm ifile
	done

SUFFIXES: .F
