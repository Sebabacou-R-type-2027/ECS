cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(ECS LANGUAGES CXX)

# `import std;` support
if(${CMAKE_CXX_STANDARD} IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
    set(CMAKE_CXX_MODULE_STD ON)
else()
    message(FATAL_ERROR "Compiler does not support `import std;`")
endif()

add_subdirectory(lib)
add_subdirectory(src)

install(EXPORT main DESTINATION lib/cmake/ecs
    NAMESPACE ecs::
    CXX_MODULES_DIRECTORY modules
    EXPORT_PACKAGE_DEPENDENCIES
)

include(CPack)
