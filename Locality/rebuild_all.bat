@echo off
msbuild engine.sln /t:rebuild /p:Configuration=%1;Platform=%2
msbuild game.sln /t:rebuild /p:Configuration=%1;Platform=%2
msbuild tool.sln /t:rebuild /p:Configuration=%1;Platform=%2
