add_library(ecs)

target_sources(ecs PUBLIC
    FILE_SET CXX_MODULES
        FILES ecs.cppm
            core.cppm
)

target_link_libraries(ecs PRIVATE utils sfml-graphics sfml-window sfml-system)

add_subdirectory(components)
add_subdirectory(systems)
