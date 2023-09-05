@echo off

echo %cd%

REM Debug
if exist ..\ExeFile\Debug_64\ (
	xcopy "third_lib\Assimp\lib\assimp-vc141-mtd.dll" "..\ExeFile\Debug_64\" /Y /D
	xcopy "third_lib\FreeType\lib\freetyped.dll" "..\ExeFile\Debug_64\" /Y /D
	xcopy "skin" "..\ExeFile\%Debug_64\skin\" /Y /D /E
	xcopy "ShaderProgram" "..\ExeFile\Debug_64\ShaderProgram\" /Y /D /E
)

REM Release
if exist ..\ExeFile\Release_64\ (
	xcopy "third_lib\Assimp\lib\assimp-vc141-mtd.dll" "..\ExeFile\Release_64\" /Y /D
	xcopy "third_lib\FreeType\lib\freetyped.dll" "..\ExeFile\Release_64\" /Y /D
	xcopy "skin" "..\ExeFile\Release_64\skin\" /Y /D /E
	xcopy "ShaderProgram" "..\ExeFile\Release_64\ShaderProgram\" /Y /D /E
)

pause

