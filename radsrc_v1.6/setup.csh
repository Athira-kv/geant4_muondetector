#
# Do not call this directly, use:
# 
#   exec ./setup
#

setenv RADSRC_HOME $1
setenv RADSRC_DATA $RADSRC_HOME/data/
setenv PATH $RADSRC_HOME/bin:$PATH

#csh needs a carriage return after the last command
