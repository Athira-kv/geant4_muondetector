@REM
@REM Run test job
@REM

@echo Running test job
bin\radsrc.exe reference/input.txt

@echo off

@REM Now compare output to the reference file
@echo.
@echo.
@fc /a output.lin reference\output.lin

@echo.
@echo The only difference should be the date.

@pause
