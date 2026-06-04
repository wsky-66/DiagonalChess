@echo off
REM ===================================================================
REM build_local.bat — MSYS2 环境下的本地构建脚本
REM 使用 MSYS2 UCRT64 的 g++ 编译器 + SFML 库
REM 分为三步：编译 → 复制 DLL → 完成
REM ===================================================================

REM MSYS2 安装路径（UCRT64 环境）
set MSYS2_DIR=C:\msys64\ucrt64

REM g++ 完整路径
set GPP=%MSYS2_DIR%\bin\g++.exe

REM 头文件搜索路径（MSYS2 系统头文件 + 项目头文件）
set INC=-I"%MSYS2_DIR%\include" -Iinclude

REM 链接库（SFML 各模块 + Windows 系统库 + FreeType 字体库）
set LIB=-L"%MSYS2_DIR%\lib" -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system -lws2_32 -lopengl32 -lwinmm -lgdi32 -lfreetype

REM 源文件列表（已过时，实际项目有更多源文件，建议用 CMake 编译）
set SRC=src\main.cpp src\Game.cpp

REM DLL 来源目录（MSYS2 的 bin 目录）
set DLL_SRC=%MSYS2_DIR%\bin

REM 需要复制的 DLL 文件列表
REM   - SFML 各模块（graphics/window/system/audio/network）
REM   - FreeType（字体渲染）
REM   - OpenAL（音频）
REM   - GCC 运行时（libgcc_s_seh-1.dll, libstdc++-6.dll）
REM   - POSIX 线程（libwinpthread-1.dll）
set DLL_LIST=libsfml-graphics-2.dll libsfml-window-2.dll libsfml-system-2.dll libsfml-audio-2.dll libsfml-network-2.dll libfreetype-6.dll libopenal-1.dll libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll

echo ========================================
echo   Diagonal Chess - Build Script
echo ========================================
echo.

echo [1/3] Compiling...
REM 编译：-std=c++17 使用 C++17 标准，-O2 二级优化
"%GPP%" -std=c++17 -O2 %INC% %SRC% %LIB% -o game.exe
if %errorlevel% neq 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)
echo   Compilation successful!

echo.
echo [2/3] Copying DLLs...
REM 逐个复制 DLL 文件到当前目录
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
