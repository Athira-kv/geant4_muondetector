ifeq (0,$(MAKELEVEL))
$(error Invoke make from the src directory only)
endif
 
.SUFFIXES: .c .cc .f .F
.PHONY: clean

RADSRCDIR=../libradsrc
CXXFLAGS+= -I$(RADSRCDIR) -g
misc: cgam fgam

cgam: cgam.o $(RADSRCDIR)/libradsrc.a
	$(CXX) -o cgam cgam.o -L$(RADSRCDIR) -lradsrc

fgam: fgam.o $(RADSRCDIR)/libradsrc.a
	$(FC) $(FFLAGSLOCAL) -o fgam fgam.o -L$(RADSRCDIR) -lradsrc $(FCLIBS)

clean:
	rm -f */*.o core *.core *.stackdump
	rm -f cgam fgam
	rm -f cgam.exe fgam.exe
.cc.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

cgam.o: cgam.cc
fgam.o: fgam.f
	$(FC) $(FFLAGS) -c -o $@ $<
$(RADSRCDIR)/libradsrc.a:
