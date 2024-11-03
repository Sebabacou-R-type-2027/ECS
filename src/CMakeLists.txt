add_library(ecs)

target_sources(ecs PRIVATE core.cpp
    PUBLIC FILE_SET CXX_MODULES
        FILES ecs.cppm
            core.cppm
)

add_subdirectory(abstractions)
add_subdirectory(components)
add_subdirectory(implementations)
add_subdirectory(systems)

target_link_libraries(ecs PUBLIC utils)

install(TARGETS ecs EXPORT main
    FILE_SET CXX_MODULES DESTINATION modules
)
