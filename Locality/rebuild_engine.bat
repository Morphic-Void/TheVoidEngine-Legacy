@echo off
msbuild engine.sln /t:rebuild /p:Configuration=%1;Platform=%2

