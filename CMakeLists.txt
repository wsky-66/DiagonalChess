# ===================================================================
# CMakeLists.txt — CMake 构建配置文件
# 用于配置编译选项、源文件、头文件路径和链接库
# ===================================================================
# 使用方式：
#   mkdir build && cd build
#   cmake .. -G "Ninja"                # 配置项目
#   ninja                               # 编译
# ===================================================================

# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目名称和语言（C++）
project(DiagonalChess LANGUAGES CXX)

# 设置 C++17 标准（使用了 std::make_unique, constexpr 等 C++17 特性）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML 库的安装路径（MSYS2 UCRT64 环境）
set(SFML_DIR "C:/msys64/ucrt64")

# 编译生成的可执行文件（WIN32 表示 Windows GUI 程序，不显示控制台窗口）
add_executable(${PROJECT_NAME} WIN32
    # —— 程序入口 ——
    src/main.cpp

    # —— 棋子 ——
    src/Engine/Piece/DiagonalChessPiece.cpp    # 对角象棋棋子
    src/Engine/Piece/GomokuPiece.cpp           # 五子棋棋子
    src/Engine/Core/ChessPlatform.cpp          # 游戏平台
    src/Engine/Core/GameManager.cpp            # 游戏管理器

    # —— 棋盘 ——
    src/Engine/Board/DiagonalChessBoard.cpp    # 对角象棋棋盘
    src/Engine/Board/GomokuBoard.cpp           # 五子棋棋盘

    # —— 规则 ——
    src/Engine/Rule/DiagonalChessRule.cpp      # 对角象棋规则
    src/Engine/Rule/GomokuRule.cpp             # 五子棋规则

    # —— AI ——
    src/Engine/AI/DiagonalChessAI.cpp          # AI 引擎（Minimax搜索）

    # —— 网络 ——
    src/Engine/Network/NetworkManager.cpp      # 局域网联机

    # —— UI ——
    src/Engine/UI/UIManager.cpp                # UI 管理器（棋盘绘制、按钮等）
    src/Engine/UI/MainMenuScene.cpp            # 主菜单场景

    # —— 音效/特效 ——
    src/Engine/Audio/AudioManager.cpp          # 音效生成与播放
    src/Engine/Effects/ParticleSystem.cpp      # 粒子特效

    # —— 游戏实例 ——
    src/Engine/Games/DiagonalChess/DiagonalChessGame.cpp  # 对角象棋游戏
    src/Engine/Games/Gomoku/GomokuGame.cpp               # 五子棋游戏
)

# 添加头文件搜索路径
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include        # 项目头文件目录
    ${SFML_DIR}/include                        # SFML 头文件目录
)

# 添加库文件搜索路径
target_link_directories(${PROJECT_NAME} PRIVATE
    ${SFML_DIR}/lib                            # SFML 库文件目录
)

# 链接依赖库
target_link_libraries(${PROJECT_NAME} PRIVATE
    # SFML 库（Debug 版用 -d 后缀，Release 版用优化版）
    debug sfml-main-d     optimized sfml-main       # SFML 主模块
    debug sfml-graphics-d optimized sfml-graphics   # 图形渲染
    debug sfml-window-d   optimized sfml-window     # 窗口管理
    debug sfml-system-d   optimized sfml-system     # 系统工具
    debug sfml-audio-d    optimized sfml-audio      # 音频播放
    debug sfml-network-d  optimized sfml-network    # 网络通信

    # Windows 系统库
    opengl32                                      # OpenGL 图形
    winmm                                         # Windows 多媒体
    gdi32                                         # GDI 图形设备接口
    freetype                                      # 字体渲染
)

# 编译后将 DLL 文件复制到可执行文件所在目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_DIR}/bin/libsfml-graphics-2.dll"   # SFML 图形库
        "${SFML_DIR}/bin/libsfml-window-2.dll"     # SFML 窗口库
        "${SFML_DIR}/bin/libsfml-system-2.dll"     # SFML 系统库
        "${SFML_DIR}/bin/libsfml-audio-2.dll"      # SFML 音频库
        "${SFML_DIR}/bin/libsfml-network-2.dll"    # SFML 网络库
        "${SFML_DIR}/bin/libopenal-1.dll"          # OpenAL 音频库
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"        # 目标目录（exe 所在目录）
)
