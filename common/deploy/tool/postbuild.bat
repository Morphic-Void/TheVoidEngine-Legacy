@echo off
rem call "$(SolutionDir)..\common\deploy\$(ProjectName)\postbuild.bat" "$(SolutionDir)" "$(ProjectName)" "$(Configuration)" "$(Platform)" "$(TargetName)"
rem %1 is solution directory
rem %2 is project directory
rem %3 is configuration directory
rem %4 is platform directory
rem %5 is target file name
cd "%~dp0"
echo Deploying %~2: >log.txt
if not exist "%~1bin\%~4\pipeline\logs" mkdir "%~1bin\%~4\pipeline\logs"
if not exist "%~1bin\%~4\pipeline\json" mkdir "%~1bin\%~4\pipeline\json"
copy /B /Y "bat\*.bat" "%~1bin\%~4\pipeline\*.bat" >>log.txt
copy /B /Y "dll\%~4\*.dll" "%~1bin\%~4\pipeline\*.dll" >>log.txt
copy /B /Y "..\..\shaders\json\*.json" "%~1bin\%~4\pipeline\json\*.json" >>log.txt
if exist "%~1%~2\%~3\%~4\%~5.exe" (
	del "%~1bin\%~4\pipeline\tool.exe" /Q >>log.txt
	copy /B /Y "%~1%~2\%~3\%~4\%~5.exe" "%~1bin\%~4\pipeline\temp.tmp" >>log.txt
	ren "%~1bin\%~4\pipeline\temp.tmp" "tool.exe" >>log.txt )
exit 0


