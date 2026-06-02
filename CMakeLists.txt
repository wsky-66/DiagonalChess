cmake_minimum_required(VERSION 3.16)
project(DiagonalChess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../SFML-2.6.2")

add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    src/Game.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SFML_DIR}/include
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${SFML_DIR}/lib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    debug sfml-graphics-d optimized sfml-graphics
    debug sfml-window-d   optimized sfml-window
    debug sfml-system-d   optimized sfml-system
    opengl32
    winmm
    gdi32
    freetype
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<$<CONFIG:Debug>:${SFML_DIR}/bin/sfml-graphics-d-2.dll>$<$<CONFIG:Release>:${SFML_DIR}/bin/sfml-graphics-2.dll>"
        "$<$<CONFIG:Debug>:${SFML_DIR}/bin/sfml-window-d-2.dll>$<$<CONFIG:Release>:${SFML_DIR}/bin/sfml-window-2.dll>"
        "$<$<CONFIG:Debug>:${SFML_DIR}/bin/sfml-system-d-2.dll>$<$<CONFIG:Release>:${SFML_DIR}/bin/sfml-system-2.dll>"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
