@echo off
rem call "$(SolutionDir)..\common\deploy\$(ProjectName)\postbuild.bat" "$(SolutionDir)" "$(ProjectName)" "$(Configuration)" "$(Platform)" "$(TargetName)"
rem %1 is solution directory
rem %2 is project directory
rem %3 is configuration directory
rem %4 is platform directory
rem %5 is target file name
cd "%~dp0"
echo Deploying %~2: >log.txt
if not exist "%~1bin\%~4\game\logs" mkdir "%~1bin\%~4\game\logs"
if not exist "%~1bin\%~4\game\temp" mkdir "%~1bin\%~4\game\temp"
if not exist "%~1bin\%~4\game\libs" mkdir "%~1bin\%~4\game\libs"
if not exist "%~1bin\%~4\game\input" mkdir "%~1bin\%~4\game\input"
if not exist "%~1bin\%~4\game\output" mkdir "%~1bin\%~4\game\output"
if not exist "%~1bin\%~4\pipeline\cso" mkdir "%~1bin\%~4\pipeline\cso"
copy /B /Y "text\lorem_ipsum.txt" "%~1bin\%~4\game\input\text.txt" >>log.txt
copy /B /Y "%~1%~2\%~3\%~4\*.cso" "%~1bin\%~4\pipeline\cso\*.cso" >>log.txt
if exist "%~1%~2\%~3\%~4\%~5.exe" copy /B /Y "%~1%~2\%~3\%~4\%~5.exe" "%~1bin\%~4\game\engine.exe" >>log.txt
cd "%~1bin\%~4\pipeline"
if exist tool.exe (
	del "%~1bin\%~4\pipeline\logs\*.*" /Q
	tool -r "json\resources.json" "..\game\libs\rlib.rbl"
	tool -s "json\shaders.json" "..\game\libs\slib.rbl" )
exit 0


