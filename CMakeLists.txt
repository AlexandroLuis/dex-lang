cmake_minimum_required(VERSION 3.15)
project(dexlang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src/lexer src/parser src/interpreter)

# Source files
set(SOURCES
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/interpreter/interpreter.cpp
    src/main.cpp
)

add_executable(dex ${SOURCES})

# Optional: Add warnings and debug flags
target_compile_options(dex PRIVATE -Wall -Wextra -g)
