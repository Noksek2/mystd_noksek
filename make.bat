@echo off
REM Windows Batch File for Building mystd_test
REM Equivalent to the Makefile

setlocal enabledelayedexpansion

set CC=gcc
set CFLAGS=-O2 -D_DEBUG
set TARGET=bin\mystd_test.exe
set BINDIR=bin_win

set OBJS=%BINDIR%\mystd.o %BINDIR%\myallocator.o %BINDIR%\main.o

REM Check if argument is provided
if not exist "%BINDIR%" (
    mkdir "%BINDIR%"
    echo Created %BINDIR% directory
)

REM Compile main.c
echo Compiling main.c...
%CC% %CFLAGS% -c main.c -o %BINDIR%\main.o
if errorlevel 1 (
    echo Error compiling main.c
    goto end
)

REM Compile mystd.c
echo Compiling mystd.c...
%CC% %CFLAGS% -c mystd.c -o %BINDIR%\mystd.o
if errorlevel 1 (
    echo Error compiling mystd.c
    goto end
)

REM Compile myallocator.c
echo Compiling myallocator.c...
%CC% %CFLAGS% -c myallocator.c -o %BINDIR%\myallocator.o
if errorlevel 1 (
    echo Error compiling myallocator.c
    goto end
)

REM Link object files
echo Linking %TARGET%...
%CC% %CFLAGS% -o %TARGET% %OBJS%
if errorlevel 1 (
    echo Error linking %TARGET%
    goto end
)

echo Build completed successfully: %TARGET%
goto end

:clean
echo Cleaning up...
if exist "%BINDIR%" (
    rmdir /s /q "%BINDIR%"
    echo Cleaned %BINDIR% directory
)
echo Clean completed.
goto end

:end
endlocal
exit /b 0
