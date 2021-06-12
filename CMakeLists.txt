cmake_minimum_required(VERSION 3.10.0)
project(TERRAIN)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")

# add_subdirectory(extern)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/stb)
file(GLOB SOURCES "src/*.cpp")  # "src/hittables/*.cpp")
add_executable(app ${SOURCES})
