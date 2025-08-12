cmake_minimum_required(VERSION 3.10)
project(snakepp LANGUAGES CXX)

set(TARGET snake++)

set(SOURCEDIR "${CMAKE_SOURCE_DIR}/src")
file(GLOB SOURCES "${SOURCEDIR}/*.cc")

include_directories(${SOURCEDIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-g -Wall)

set(ASSETS_SRC "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_DEST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET}
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
    SDL2_image::SDL2_image
)

add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_SRC}" "${ASSETS_DEST}"
    COMMENT "Copying assets to build directory..."
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CMAKE_BINARY_DIR}/*"
)

