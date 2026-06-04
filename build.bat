@echo off
REM ===================================================================
REM build.bat — 旧版构建脚本
REM 使用指定的 SFML 2.5.1 路径编译（路径为相对路径 ..\ForestIceFireMan\SFML-2.5.1）
REM ===================================================================

REM SFML 安装路径
set SFML_DIR=..\ForestIceFireMan\SFML-2.5.1

REM 源文件列表
set SRC=src\main.cpp src\Game.cpp

REM 头文件搜索路径（-I 参数）
set INC=-Iinclude -I%SFML_DIR%\include

REM 链接库（-L 库路径，-l 库名，-lws2_32 是 Windows 网络库）
set LIB=-L%SFML_DIR%\lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network -lws2_32 -lopengl32 -lwinmm -lgdi32 -lfreetype

echo Compiling Diagonal Chess...
REM 使用 g++ 编译，C++17 标准，O2 优化
g++ -std=c++17 -O2 %INC% %SRC% %LIB% -o game.exe

REM 检查编译是否成功（%errorlevel% 是上一条命令的退出码，0 表示成功）
if %errorlevel% equ 0 (
    echo Build successful!
    echo Copying DLLs...
    REM 复制必要的 DLL 到当前目录（>nul 表示不显示复制过程）
    copy /Y %SFML_DIR%\bin\sfml-system-2.dll . >nul
    copy /Y %SFML_DIR%\bin\sfml-window-2.dll . >nul
    copy /Y %SFML_DIR%\bin\sfml-graphics-2.dll . >nul
    copy /Y %SFML_DIR%\bin\sfml-audio-2.dll . >nul
    copy /Y %SFML_DIR%\bin\sfml-network-2.dll . >nul
    echo Run game.exe to play!
) else (
    echo Build failed!
)
