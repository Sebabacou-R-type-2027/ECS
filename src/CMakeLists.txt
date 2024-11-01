add_library(ecs)

target_sources(ecs PRIVATE core.cpp
    PUBLIC FILE_SET CXX_MODULES
        FILES ecs.cppm
            core.cppm
)

target_link_libraries(ecs PRIVATE utils sfml-graphics sfml-window sfml-system)

add_subdirectory(abstractions)
add_subdirectory(components)
add_subdirectory(implementations)
add_subdirectory(systems)
