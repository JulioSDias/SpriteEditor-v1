@echo off

REM clang SpriteEditor_Platform.c -luser32 -lgdi32
gcc SpriteEditor_Platform.c -lgdi32 -luser32 -lWinmm -msse4.1
"a.exe"