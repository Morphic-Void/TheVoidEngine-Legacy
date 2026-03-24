@echo off
msbuild tool.sln /t:rebuild /p:Configuration=%1;Platform=%2

