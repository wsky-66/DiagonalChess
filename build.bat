@echo off
set SFML_DIR=..\ForestIceFireMan\SFML-2.5.1
set SRC=src\main.cpp src\Game.cpp
set INC=-Iinclude -I%SFML_DIR%\include
set LIB=-L%SFML_DIR%\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lws2_32 -lopengl32 -lwinmm -lgdi32 -lfreetype

echo Compiling Diagonal Chess...
g++ -std=c++17 -O2 %INC% %SRC% %LIB% -o game.exe

if %errorlevel% equ 0 (
    echo Build successful!
    echo Copying DLLs...
    copy /Y %SFML_DIR%\bin\sfml-system-2.dll . >nul
    copy /Y %SFML_DIR%\bin\sfml-window-2.dll . >nul
    copy /Y %SFML_DIR%\bin\sfml-graphics-2.dll . >nul
    copy /Y %SFML_DIR%\bin\sfml-audio-2.dll . >nul
    copy /Y %SFML_DIR%\bin\sfml-network-2.dll . >nul
    echo Run game.exe to play!
) else (
    echo Build failed!
)
