@echo off

echo === Setting up ===
Premake\Windows\premake5.exe --file=Build.lua vs2022

echo === Setup complete ===

pause