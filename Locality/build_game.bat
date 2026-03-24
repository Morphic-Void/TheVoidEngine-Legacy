@echo off
del "game\\%1\\%2\\game_*.*" /Q >NUL 2>NUL
msbuild game.sln /t:build /p:Configuration=%1;Platform=%2;TargetName="game_%date:~8,2%%date:~3,2%%date:~0,2%%time:~0,2%%time:~3,2%%time:~6,2%%time:~9,2%"
