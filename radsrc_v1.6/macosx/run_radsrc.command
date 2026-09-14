#!/bin/sh
#
# Shell script that provides "double-click" functionality 
# to run RadSrc application
#
# Doug Wright

#....move to directory in which this command resides
cd "$(dirname "$0")"  >/dev/null 2>&1

#....prompt for possible input file
echo
echo "Enter input file name [or hit return for interactive mode] "
read file
echo

#....execute radsrc
eval bin/radsrc $file

#....user may have set terminal to automatically close window
#    so keep shell from exiting until user provides input
#
#    (could use "defaults" command to check the terminal preference setting)
#    (settings is found here terminal:preferences:settings:shell)
#
echo Hit any key to quit.;read answer
