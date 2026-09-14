#
# make a zip file for Windows XP distribution
#

BASE = radsrc_v$(shell cat ../version)
ZIP  = $(BASE)_win.zip
DIR  = ..

default: $(ZIP) push

#....prepare a directory containing what we want to zip
#    gets deleted after the zip
$(BASE): check
        #....remove all zip files
	rm -rf *.zip 
	[ -e $@ ] && rm -rf $@; mkdir $@ $@/bin
	cp $(DIR)/bin/radsrc.exe $@/bin    # executable
	cp Readme.txt test_radsrc.bat run_radsrc.bat $@       # test job        
	svn export $(DIR)/lib $@/lib	   # library (binning files)
	svn export $(DIR)/data $@/data     # decay data
	svn export $(DIR)/doc $@/doc       # documentation
        #....reference output files
	svn export $(DIR)/test/test.du/reference.mingw $@/reference  
        #....example input file
	cp $(DIR)/test/test.du/input $@/reference/input.txt

#....Make sure that the executable is the mingw version, 
#    stop the zip if it is not
check:
	@objdump -p $(DIR)/bin/radsrc.exe | grep -q msvcrt.dll || (echo This is not mingw version, no zip made; false)

#....zip up the directory (delete it if zip was succesful)
$(ZIP): $(BASE)
	zip -qr $@ $< && /bin/rm -rf $(BASE)

push: $(ZIP)
	@#scp $< oban.llnl.gov:web.me

clean:
	/bin/rm -r $(BASE)*

.PHONY: check clean push