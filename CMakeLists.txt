cmake_minimum_required(VERSION 3.16)

project(HookLeap)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SFML_DIR "C:/Libraries/SFML-3.0.2/lib/cmake/SFML")

find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics Audio Network)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(HookLeap ${SOURCES})

target_include_directories(HookLeap PRIVATE include)

target_link_libraries(HookLeap 
SFML::System
SFML::Window
SFML::Graphics
SFML::Audio
SFML::Network )