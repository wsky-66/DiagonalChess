cmake_minimum_required(VERSION 3.16)
project(DiagonalChess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "C:/msys64/ucrt64")

add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    src/Engine/Piece.cpp
    src/Engine/Core/ChessPlatform.cpp
    src/Engine/Core/GameManager.cpp
    src/Engine/Board/DiagonalChessBoard.cpp
    src/Engine/Rule/DiagonalChessRule.cpp
    src/Engine/AI/DiagonalChessAI.cpp
    src/Engine/Network/NetworkManager.cpp
    src/Engine/UI/UIManager.cpp
    src/Engine/UI/MainMenuScene.cpp
    src/Engine/Audio/AudioManager.cpp
    src/Engine/Effects/ParticleSystem.cpp
    src/Engine/Games/DiagonalChess/DiagonalChessGame.cpp
    src/Engine/Board/GomokuBoard.cpp
    src/Engine/Rule/GomokuRule.cpp
    src/Engine/Games/Gomoku/GomokuGame.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SFML_DIR}/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${SFML_DIR}/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    debug sfml-main-d     optimized sfml-main
    debug sfml-graphics-d optimized sfml-graphics
    debug sfml-window-d   optimized sfml-window
    debug sfml-system-d   optimized sfml-system
    debug sfml-audio-d    optimized sfml-audio
    debug sfml-network-d  optimized sfml-network
    opengl32
    winmm
    gdi32
    freetype
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_DIR}/bin/libsfml-graphics-2.dll"
        "${SFML_DIR}/bin/libsfml-window-2.dll"
        "${SFML_DIR}/bin/libsfml-system-2.dll"
        "${SFML_DIR}/bin/libsfml-audio-2.dll"
        "${SFML_DIR}/bin/libsfml-network-2.dll"
        "${SFML_DIR}/bin/libopenal-1.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
