@echo off
msbuild %1.sln /p:Configuration=%2;Platform=%3

