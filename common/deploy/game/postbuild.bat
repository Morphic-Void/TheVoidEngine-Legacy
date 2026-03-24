@echo off
rem call "$(SolutionDir)..\common\deploy\$(ProjectName)\postbuild.bat" "$(SolutionDir)" "$(ProjectName)" "$(Configuration)" "$(Platform)" "$(TargetName)"
rem %1 is solution directory
rem %2 is project directory
rem %3 is configuration directory
rem %4 is platform directory
rem %5 is target file name
cd "%~dp0"
echo Deploying %~2: >log.txt
if not exist "%~1bin\%~4\game\libs" mkdir "%~1bin\%~4\game\libs"
if exist "%~1%~2\%~3\%~4\%~5.dll" (
	if exist "%~1bin\%~4\game\libs\game.dll" del "%~1bin\%~4\game\libs\game.dll" /Q
	copy /B /Y "%~1%~2\%~3\%~4\%~5.dll" "%~1bin\%~4\game\libs\%~5.dll" >>log.txt
	ren "%~1bin\%~4\game\libs\%~5.dll" "game.dll" >>log.txt )
exit 0


