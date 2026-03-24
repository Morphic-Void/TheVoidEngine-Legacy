@echo off
msbuild %1.sln /t:rebuild /p:Configuration=%2;Platform=%3

