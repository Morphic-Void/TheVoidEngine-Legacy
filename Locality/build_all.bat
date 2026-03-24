@echo off
msbuild engine.sln /p:Configuration=%1;Platform=%2
msbuild game.sln /p:Configuration=%1;Platform=%2
msbuild tool.sln /p:Configuration=%1;Platform=%2
