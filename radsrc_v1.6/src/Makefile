#
# GNU compiler family is the default
# to select other compilers do one of these at the command line:
#
#    make intel
#    make portland
#    make yana
#    make mingw
#
export

CC=gcc
CXX=g++
FC=g77

FCLIBS = -lstdc++#                  # C libs from GNU C++ compiler
CFLAGS =-I../libradsrc              
override CXXFLAGS += -Wall -I../libradsrc

#....options for LLNL LC computers
ifdef LC_ALL
  FFLAGS += -fno-second-underscore # g77 specific, for a local warning
endif

# Installation destination
INSTALLROOT=..
BININSTALLDIR=$(INSTALLROOT)/bin
LIBINSTALLDIR=$(INSTALLROOT)/lib
DOCINSTALLDIR=$(INSTALLROOT)/doc

BININSTALLCMD=install -s
LIBINSTALLCMD=install 

# Doxygen
DOXYGEN=doxygen

# ---------- end of configurable settings --------------
.SUFFIXES: .c .cc .f .F
.PHONY: clean allclean radsrc libradsrc misc

default: libradsrc radsrc

#
#....Make PC version that does not require cygwin
#    (requires cygwin package: mingw-runtime to build this executable)
#
mingw:
	$(MAKE) CXXFLAGS=-mno-cygwin                                         $(filter-out $@,$(MAKECMDGOALS))

intel:
	$(MAKE) CC=icc CXX=icpc FC=ifort CXXFLAGS="-fpic"                    $(filter-out $@,$(MAKECMDGOALS))

portland: 
	$(MAKE) CC=pgc CXX=pgCC FC=pgf77 FCLIBS="-lC -lstd" CXXFLAGS="-fpic" $(filter-out $@,$(MAKECMDGOALS))

yana: 
	$(MAKE) CC=gcc CXX=g++ FC=mpiifort                                   $(filter-out $@,$(MAKECMDGOALS))

misc: libradsrc
	$(MAKE)  -C misc misc
	-$(BININSTALLCMD) misc/cgam    $(BININSTALLDIR)/cgam
	-$(BININSTALLCMD) misc/fgam    $(BININSTALLDIR)/fgam

radsrc: libradsrc
	$(MAKE) -C radsrc radsrc
	$(BININSTALLCMD) radsrc/radsrc       $(BININSTALLDIR)/radsrc
	cd ../test; ./run

libradsrc:
	$(MAKE) -C libradsrc libradsrc.a
	$(LIBINSTALLCMD) libradsrc/libradsrc.a $(LIBINSTALLDIR)/libradsrc.a

clean:
	$(MAKE) -C radsrc clean
	$(MAKE) -C libradsrc clean
	$(MAKE) -C misc clean
	rm -f */*.o core *.core

allclean: clean
	rm -f $(BININSTALLDIR)/{cgam,fgam,radsrc,mcitest,age}
	rm -f $(LIBINSTALLDIR)/libradsrc.a
	rm -f $(BININSTALLDIR)/{cgam,fgam,radsrc,mscitest,age}.exe
	rm -f vcprojects/{radsrc,libradsrc,cgam}/{Debug,Release}/*
	rm -f vcprojects/{radsrc,libradsrc,cgam}/*.plg vcprojects/*.ncb
	rm -f $(DOCINSTALLDIR)/doxygen/html/*

.cc.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

docs:
	(cd $(DOCINSTALLDIR)/doxygen; $(DOXYGEN))

include Makefile.copyright
