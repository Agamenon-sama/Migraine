cmake_minimum_required(VERSION 3.17)
project("gui" VERSION 0.1)

# Set debug mode
set(CMAKE_BUILD_TYPE Debug)

# Module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Add src files
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Add header files
file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_SOURCE_DIR}/src/*.h
    ${CMAKE_SOURCE_DIR}/src/*.hpp
)

# Define include path
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/include/imgui")
include_directories("/usr/include/SDL2")

# The executable
add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})

# External dependencies
find_package(SDL2 REQUIRED)
message(STATUS "Found SDL2")

# Imgui
file(GLOB_RECURSE IMGUI_SRC
    ${CMAKE_SOURCE_DIR}/thirdparty/imgui/*.cpp
)

add_library(Imgui SHARED ${IMGUI_SRC})
message(STATUS "Should have dealt with Imgui")
add_library(GLAD "thirdparty/glad.c")
message(STATUS "Glad should be good")

set(LIBS SDL2 Imgui GLAD)

# Linking
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl -lGL -pthread")
target_link_libraries(${PROJECT_NAME} ${LIBS})
