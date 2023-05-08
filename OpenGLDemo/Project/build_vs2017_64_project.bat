@echo off
set CURRENT_DIR=%~dp0
set BIN_DIR=build

echo %CURRENT_DIR%%BIN_DIR%
if not exist %CURRENT_DIR%%BIN_DIR% (
    mkdir %CURRENT_DIR%%BIN_DIR%
)
cd %CURRENT_DIR%%BIN_DIR%

rem Generate VS2017 project.
cmake ../ -G "Visual Studio 15 2017 Win64"

move *.vcxproj ../
move *.sln ../

pause