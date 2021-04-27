@echo off

REM clang Engine_Platform.c -luser32 -lgdi32
gcc Engine_Platform.c -lgdi32 -luser32 -lWinmm -msse4.1
"a.exe"