cmake_minimum_required(VERSION 3.15)
project(dex-lang)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/interpreter/interpreter.cpp
    src/interpreter/interpreter.h
    src/runtime/database.cpp
    src/runtime/sqlite_database.cpp
    src/runtime/mysql_database.cpp
    src/runtime/postgres_database.cpp
    src/runtime/dex_database_binding.cpp
    src/runtime/env_binding.cpp
    src/runtime/webserver.cpp
    src/runtime/fileio.cpp
    external/nlohmann/json_utils.cpp
    src/runtime/csv_utils.cpp
    src/runtime/fileio_binding.cpp
    # Add the dotenv-cpp source file here.
    # Assuming the main source file is named 'dotenv.cpp' inside external/dotenv-cpp.
    # If it's named something else (e.g., 'src/dotenv.cpp' within that folder), adjust the path.
    external/dotenv-cpp/dotenv.cpp # <--- ADDED THIS LINE FOR DOTENV
)

add_executable(dex ${SOURCES})

target_include_directories(dex PRIVATE external/dotenv-cpp)

# Add nlohmann json include directory (adjust path if needed)
target_include_directories(dex PRIVATE external/nlohmann)

# Database libraries
# Changed linking for SQLite3 to directly use 'sqlite3'
# This often works better for system-installed SQLite if find_package has issues.
find_package(SQLite3 REQUIRED)
target_include_directories(dex PRIVATE ${SQLITE3_INCLUDE_DIRS})
target_link_libraries(dex PRIVATE sqlite3) # <--- CHANGED THIS LINE FOR SQLITE3

find_path(MYSQL_INCLUDE_DIR mysql_driver.h PATHS /usr/include /usr/local/include)
find_library(MYSQL_LIB mysqlcppconn PATHS /usr/lib /usr/local/lib)
if(NOT MYSQL_INCLUDE_DIR OR NOT MYSQL_LIB)
    message(FATAL_ERROR "MySQL Connector/C++ not found")
endif()
target_include_directories(dex PRIVATE ${MYSQL_INCLUDE_DIR})
target_link_libraries(dex PRIVATE ${MYSQL_LIB})

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED libpqxx)
target_include_directories(dex PRIVATE ${LIBPQXX_INCLUDE_DIRS})
target_link_libraries(dex PRIVATE ${LIBPQXX_LIBRARIES})
target_compile_options(dex PRIVATE ${LIBPQXX_CFLAGS_OTHER})

if(UNIX)
    target_link_libraries(dex PRIVATE pthread)
endif()

# Optional compile options
target_compile_options(dex PRIVATE -Wall -Wextra -Wpedantic -O2)
