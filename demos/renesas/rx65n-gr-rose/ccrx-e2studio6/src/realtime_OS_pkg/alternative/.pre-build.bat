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
echo %qu%Checking source code tree...%qu%

set ef=0
rem Default and In case of the CS+
set FN=%CD%\..\..\..\..\lib\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.c
rem In case of the e2 studio
if %pj%==..\ set FN=%CD%\.\lib\aws\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.obj
rem echo Maximum Path is "%FN%" && rem # For debug
set FX=%FN:~259%
rem echo Over of Path is "%FX%" && rem # For debug
if not "%FX%"=="" (set ef=1) && echo Error: The following deep folder path will fail the build process. && echo %CD%

set ep=0
if not exist %pj%\src\realtime_OS_pkg\r_bsp_rtos\platform.h                   set ep=1
if not exist %pj%\src\realtime_OS_pkg\r_bsp_rtos\board\rskrx65n_2mb\r_bsp.h   set ep=1
if not exist %pj%\src\realtime_OS_pkg\r_bsp_rtos\board\rskrx65n_2mb\hwsetup.c set ep=1
if not exist %pj%\src\realtime_OS_pkg\r_cmt_rtos_rx\r_cmt_rx_if.h set ep=1
if %ep%==1 echo\ && echo Error: Missing source files of the RX65N Real-time OS Package. && echo Please read the src\realtime_OS_pkg\get!.txt and get them.

set eg=0
if not exist %pj%\src\smc_gen\r_bsp    if not exist %pj%\src\smc_gen\r_bsp__unused    set eg=1
if not exist %pj%\src\smc_gen\r_cmt_rx if not exist %pj%\src\smc_gen\r_cmt_rx__unused set eg=1
if not exist %pj%\src\smc_gen\r_ether_rx set eg=1
if not exist %pj%\src\smc_gen\r_flash_rx set eg=1
if %eg%==1 echo\ && echo Error: Missing source files of generated code by the Smart Configurator. && echo Please read the src\smc_gen\generate!.txt and generate them.

setlocal enabledelayedexpansion
rem Note: This makes using '!' in echo message impossible.

set er=0
(set s=src\realtime_OS_pkg\alternative\platform.h) && (set d=src\realtime_OS_pkg\r_bsp_rtos\platform.h)                   && for %%S in (%pj%!s!) do for %%D in (%pj%!d!) do if not "%%~tS"=="%%~tD" if exist "%%~dpD%%~nD__unused%%~xD" echo Error: Unable to rename !d! --^> %%~nD__unused%%~xD && set er=1
(set s=src\realtime_OS_pkg\alternative\r_bsp.h)    && (set d=src\realtime_OS_pkg\r_bsp_rtos\board\rskrx65n_2mb\r_bsp.h)   && for %%S in (%pj%!s!) do for %%D in (%pj%!d!) do if not "%%~tS"=="%%~tD" if exist "%%~dpD%%~nD__unused%%~xD" echo Error: Unable to rename !d! --^> %%~nD__unused%%~xD && set er=1
(set s=src\realtime_OS_pkg\alternative\hwsetup.c)  && (set d=src\realtime_OS_pkg\r_bsp_rtos\board\rskrx65n_2mb\hwsetup.c) && for %%S in (%pj%!s!) do for %%D in (%pj%!d!) do if not "%%~tS"=="%%~tD" if exist "%%~dpD%%~nD__unused%%~xD" echo Error: Unable to rename !d! --^> %%~nD__unused%%~xD && set er=1

set en=0
(set s=src\smc_gen\r_bsp)    && for %%S in (%pj%!s!) do if exist %%S if exist %%S__unused echo Error: Unable to rename !s! --^> %%~nS__unused && set en=1
(set s=src\smc_gen\r_cmt_rx) && for %%S in (%pj%!s!) do if exist %%S if exist %%S__unused echo Error: Unable to rename !s! --^> %%~nS__unused && set en=1

if %ef%==1 echo\ && goto ERROR
if %ep%==1 echo\ && goto ERROR
if %eg%==1 echo\ && goto ERROR
if %er%==1 echo\ && goto ERROR
if %en%==1 echo\ && goto ERROR

echo %qu%Use r_bsp_rtos and r_cmt_rtos_rx modules instead of r_bsp and r_cmt_rx modules%qu%

(set s=src\smc_gen\r_bsp)    && for %%S in (%pj%!s!) do if not exist %%S__unused echo renaming folder: !s! --^> %%~nS__unused && ren %pj%!s! %%~nS__unused
(set s=src\smc_gen\r_cmt_rx) && for %%S in (%pj%!s!) do if not exist %%S__unused echo renaming folder: !s! --^> %%~nS__unused && ren %pj%!s! %%~nS__unused

echo %qu%Use alternative source files of platform.h, r_bsp.h and hwsetup.c in r_bsp_rtos module%qu%

(set s=src\realtime_OS_pkg\r_bsp_rtos\platform.h)                   && for %%S in (%pj%!s!) do if not exist "%%~dpS%%~nS__unused%%~xS" echo renaming file: !s! --^> %%~nS__unused%%~xS && move /y %%S "%%~dpS%%~nS__unused%%~xS" > NUL
(set s=src\realtime_OS_pkg\r_bsp_rtos\board\rskrx65n_2mb\r_bsp.h)   && for %%S in (%pj%!s!) do if not exist "%%~dpS%%~nS__unused%%~xS" echo renaming file: !s! --^> %%~nS__unused%%~xS && move /y %%S "%%~dpS%%~nS__unused%%~xS" > NUL
(set s=src\realtime_OS_pkg\r_bsp_rtos\board\rskrx65n_2mb\hwsetup.c) && for %%S in (%pj%!s!) do if not exist "%%~dpS%%~nS__unused%%~xS" echo renaming file: !s! --^> %%~nS__unused%%~xS && move /y %%S "%%~dpS%%~nS__unused%%~xS" > NUL

(set s=src\realtime_OS_pkg\alternative\platform.h) && (set d=src\realtime_OS_pkg\r_bsp_rtos\platform.h)                   && for %%S in (%pj%!s!) do for %%D in (%pj%!d!) do if not exist %%D echo copying file: !s! --^> !d! && copy /y %%S %%D > NUL
(set s=src\realtime_OS_pkg\alternative\r_bsp.h)    && (set d=src\realtime_OS_pkg\r_bsp_rtos\board\rskrx65n_2mb\r_bsp.h)   && for %%S in (%pj%!s!) do for %%D in (%pj%!d!) do if not exist %%D echo copying file: !s! --^> !d! && copy /y %%S %%D > NUL
(set s=src\realtime_OS_pkg\alternative\hwsetup.c)  && (set d=src\realtime_OS_pkg\r_bsp_rtos\board\rskrx65n_2mb\hwsetup.c) && for %%S in (%pj%!s!) do for %%D in (%pj%!d!) do if not exist %%D echo copying file: !s! --^> !d! && copy /y %%S %%D > NUL

:NOERROR
endlocal
exit /b 0 && rem # Not terminate cmd.exe here

:ERROR
endlocal
rem exit /b 2 && rem # For debug
exit 2 && rem # Terminate cmd.exe here
