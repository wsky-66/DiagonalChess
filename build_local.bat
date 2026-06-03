@echo off
set MSYS2_DIR=C:\msys64\ucrt64
set GPP=%MSYS2_DIR%\bin\g++.exe
set INC=-I"%MSYS2_DIR%\include" -Iinclude
set LIB=-L"%MSYS2_DIR%\lib" -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system -lws2_32 -lopengl32 -lwinmm -lgdi32 -lfreetype
set SRC=src\main.cpp src\Game.cpp
set DLL_SRC=%MSYS2_DIR%\bin
set DLL_LIST=libsfml-graphics-2.dll libsfml-window-2.dll libsfml-system-2.dll libsfml-audio-2.dll libsfml-network-2.dll libfreetype-6.dll libopenal-1.dll libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll

echo ========================================
echo   Diagonal Chess - Build Script
echo ========================================
echo.

echo [1/3] Compiling...
"%GPP%" -std=c++17 -O2 %INC% %SRC% %LIB% -o game.exe
if %errorlevel% neq 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)
echo   Compilation successful!

echo.
echo [2/3] Copying DLLs...
for %%d in (%DLL_LIST%) do (
    copy /Y "%DLL_SRC%\%%d" . >nul 2>&1
)
echo   DLLs copied!

echo.
echo [3/3] Build complete!
echo.
echo ========================================
echo   Output: game.exe
echo   Location: %cd%
echo ========================================
echo.
echo Run game.exe to play!
