#
# make       = make unix version, run test jobs, compare to reference output
# make mingw = on PC this generates version that can run without cygwin
# make win   = create zip file with PC binary and data
# make mac   = create zip file with MacOSX binary and data
#
# By default generate the setup file and then pass all
# targets to the Makefile in the src directory
#

default: setup
	@$(MAKE) -C src

mac:
	$(MAKE) -C macosx

win:
	$(MAKE) -C win

setup:
	@./setup.create

#....must remove any targets above if calling submake, otherwise you get a warning
FILTER_OUT = setup mac win
MAKECMDGOALS := $(filter-out $(FILTER_OUT),$(MAKECMDGOALS))

$(MAKECMDGOALS): setup
	@$(MAKE) -C src $(MAKECMDGOALS)

.PHONY: test $(FILTER_OUT)

MAKEFLAGS += --no-print-directory
