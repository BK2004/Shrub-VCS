@echo off
set CPP_COMPILER=g++
set OUT=.\build
set DIRS=argparse commands libs
set MAIN_DIR=.\src
set BUILD_EXE=app.exe
set CLIBS=-lz -lcrypto -I ./src
set PRE_BUILD=argparse/gen_argparse.sh commands/gen_commands.sh

set OUT_ARGS=-o %OUT%\%BUILD_EXE% %MAIN_DIR%\main.cpp

FOR %%p in (%PRE_BUILD%) DO (
	bash %MAIN_DIR:\=/%/%%p
)

FOR %%d IN (%DIRS%) DO (
	call set "OUT_ARGS=%%OUT_ARGS%% -I %MAIN_DIR%\%%d"
	if exist %MAIN_DIR%\%%d\*.cpp call set "OUT_ARGS=%%OUT_ARGS%% %MAIN_DIR%\%%d\*.cpp"
)

if exist %OUT%\ (
	RD "%OUT%" /S /Q 
)

mkdir %OUT%

%CPP_COMPILER% %OUT_ARGS% %CLIBS%