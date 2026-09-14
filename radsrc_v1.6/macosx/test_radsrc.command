#!/bin/sh
#
# Shell script that provides "double-click" functionality equivalent to
# MacOSX application.
#
# Doug Wright

#....move to directory in which this command resides
cd "$(dirname "$0")"  >/dev/null 2>&1

#....execute test job
echo
echo Executing test job: bin/radsrc reference/input.txt
eval bin/radsrc reference/input.txt

#....compare output to reference
echo
echo Comparing results to reference with diff.
echo
diff output.lin reference/output.lin

echo
echo The only difference above should be the date.
echo

#....user may have set terminal to automatically close window
#    so keep shell from exiting until user provides input
#
#    (could use "defaults" command to check the terminal preference setting)
#    (settings is found here terminal:preferences:settings:shell)
#
echo Hit any key to quit.;read answer