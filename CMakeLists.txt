cmake_minimum_required(VERSION 3.20)

project(spine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        12.1.0
    GIT_SHALLOW    ON
)

FetchContent_MakeAvailable(fmt)

add_library(magic_enum INTERFACE)
target_include_directories(
    magic_enum
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/magic_enum"
)

add_executable(spine
    src/main.cpp
    src/lexer/Lexer.cpp
    src/parser/Parser.cpp
)

target_compile_features(spine PRIVATE cxx_std_20)

target_link_libraries(spine 
    PRIVATE 
    fmt::fmt
    magic_enum  
)