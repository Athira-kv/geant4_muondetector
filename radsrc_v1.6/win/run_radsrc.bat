@REM
@REM Execute radsrc, give user the chance to use an input file
@REM

@echo Running radsrc

@set /p filename="Enter input file name [or hit return for interactive mode] "

bin\radsrc.exe %filename%

@pause
