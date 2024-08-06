cmake_minimum_required(VERSION 3.29)
project(GameEngine_SDL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(
        GameEngine_SDL
  ${SOURCE_FILES}
)

find_package(SDL2 REQUIRED)

target_include_directories(GameEngine_SDL  
  PUBLIC src
  PUBLIC ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(GameEngine_SDL
  ${SDL2_LIBRARIES}
)
