ifeq (0,$(MAKELEVEL))
$(error Invoke make from the src directory only)
endif

# ---------- end of configurable settings --------------
.SUFFIXES: .c .cc .f .F
.PHONY: clean 

LIBOBJS= legacydata.o legacyfiles.o isotopedb.o \
	elements.o legacyglue.o isotope.o isotopemix.o \
	radsource.o fortran.o spline.o cpp_api.o \
	decaycomputer.o photoncomputer.o dbmanager.o \
	batemansolution.o decaymode.o

libradsrc.a: $(LIBOBJS)
	ar -crs libradsrc.a $(LIBOBJS)

clean:
	rm -f libradsrc.a *.o

.cc.o:
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

# ---------- dependencies ------------

batemansolution.o: batemansolution.cc batemansolution.h
cpp_api.o: cpp_api.cc cpp_api.h porting.h radsource.h
dbmanager.o: dbmanager.cc dbmanager.h radsrcerr.h isotope.h isotopedb.h legacyglue.h
decaycomputer.o: decaycomputer.cc decaycomputer.h isotopedb.h porting.h
decaymode.o: decaymode.h
elements.o: elements.cc elements.h porting.h
fortran.o: fortran.cc cpp_api.h porting.h radsource.h
isotope.o: isotope.cc isotope.h elements.h porting.h
isotopedb.o: isotopedb.cc isotopedb.h porting.h
isotopemix.o: isotopemix.cc porting.h isotopemix.h
legacydata.o: legacydata.cc legacydata.h porting.h legacyfiles.h
legacyfiles.o: legacyfiles.cc legacyfiles.h radsrcerr.h porting.h
legacyglue.o: legacyglue.cc isotopedb.h legacyfiles.h elements.h porting.h dbmanager.h
photoncomputer.o: photoncomputer.cc porting.h photoncomputer.h spline.h isotopedb.h
radsource.o: radsource.cc porting.h radsource.h isotopedb.h
spline.o: spline.h spline.cc

batemansolution.h: porting.h isotope.h
cpp_api.h: porting.h
dbmanager.h: porting.h
decaycomputer.h: porting.h isotopedata.h isotopemix.h radsrcerr.h batemansolution.h
decaymode.h: porting.h dbmanager.h isotope.h
elements.h:
isotope.h: dbmanager.h
isotopedata.h: isotope.h decaymode.h
isotopedb.h: porting.h isotopedata.h dbmanager.h radsrcerr.h
isotopemix.h: porting.h isotope.h
legacydata.h: porting.h isotope.h
legacyfiles.h: legacydata.h dbmanager.h
legacyglue.h: dbmanager.h
oldradsource.h: porting.h isotope.h isotopedb.h
photoncomputer.h: porting.h decaycomputer.h isotope.h
radsource.h: porting.h decaycomputer.h photoncomputer.h
radsrcerr.h:
spline.h:
