#
# Create zip file with complete macosx bundle with
# radsrc executable and data
#
# Doug Wright

BASE = radsrc_v$(shell cat ../version)
TMP  = /tmp/$(BASE)
ZIP  = $(BASE)_macosx.zip
DIR = ..
SCRIPTS = *.command

default: $(ZIP)

#....create zip file from tmp directory
$(ZIP): $(TMP)
	ditto -ck --sequesterRsrc --keepParent $(TMP) $@
	/bin/rm -fR $(TMP) #....delete tmp directory

#....prepare a tmp directory containing what we want to zip
$(TMP): bin lib data doc reference
	rm -fR radsrc_v*    #....remove all existing zip files or unpacked directories

	#....make sure macos execute bit is set on scripts
	SetFile -a E *.command

	#....make $(TMP) directory
	[ -e $@ ] && rm -fR $@; cp -R . $@

    # ditto does not have a file exclude feature, 
	# so remove files that should not be in zip archive
	rm -f $(TMP)/Makefile* 
	rm -f $(TMP)/Notes

bin lib data doc:
	cp -R $(DIR)/$@ .

reference:
	if [ -e $@ ]; then rm -fR $@; fi
	cp -fR $(DIR)/test/test.du/reference .
	cp $(DIR)/test/test.du/input ./reference/input.txt

.PHONY: $(TMP) reference