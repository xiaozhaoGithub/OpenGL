@echo off

echo %cd%
xcopy "third_lib\Assimp\lib\assimp-vc141-mtd.dll" "..\ExeFile\Debug_64" /S /Y

pause