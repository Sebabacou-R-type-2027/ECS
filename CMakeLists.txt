cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(R-Type/ECS LANGUAGES CXX)

# Minimal versions
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14.0)
        message(FATAL_ERROR "GCC version must be at least 14")
    endif()
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 16.0)
        message(FATAL_ERROR "Clang version must be at least 16")
    endif()
endif()

# `import std;` support
if(${CMAKE_CXX_STANDARD} IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
    set(CMAKE_CXX_MODULE_STD ON)
    add_compile_definitions(__cpp_lib_modules=202207L)
else()
    message(WARNING "Compiler does not support `import std;`")
endif()

# Dependencies
find_package(SFML 2.6 COMPONENTS graphics window system CONFIG REQUIRED)

add_subdirectory(lib)
