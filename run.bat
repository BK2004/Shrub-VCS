@echo off
set OUT=".\build"
set BUILD_EXE="app.exe"

call ./build.bat
%OUT%\%BUILD_EXE%
./clean.bat