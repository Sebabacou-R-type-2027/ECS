add_library(ecs)

target_sources(ecs PUBLIC
    FILE_SET CXX_MODULES
        FILES ecs.cppm
            core.cppm
)

target_link_libraries(ecs PRIVATE utils)

add_subdirectory(components)
