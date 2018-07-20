@echo off
rem ################################################################################
rem # This batch file will be executed at the pre-build step
rem ################################################################################

setlocal

rem Default and In case of the CS+
set pj=.\
rem In case of the e2 studio
if exist ..\src\realtime_OS_pkg\alternative\%~nx0% set pj=..\
rem echo Project Folder is "%pj%" && rem # For debug

rem Default and In case of the CS+
set qu=
rem In case of the e2 studio
if exist ..\%~nx0% set qu='
echo %qu%Checking folder path depth...%qu%

rem Default and In case of the CS+
set FN=%CD%\..\..\..\..\lib\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.c
rem In case of the e2 studio
if %pj%==..\ set FN=%CD%\.\lib\aws\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.obj
rem echo Maximum Path is "%FN%" && rem # For debug
set FX=%FN:~259%
rem echo Over of Path is "%FX%" && rem # For debug
if not "%FX%"=="" (echo Error: The following deep folder path will fail the build process.) && (echo %CD%) && goto ERROR

echo OK

:NOERROR
endlocal
exit /b 0 && rem # Not terminate cmd.exe here

:ERROR
endlocal
rem exit /b 2 && rem # For debug
exit 2 && rem # Terminate cmd.exe here
