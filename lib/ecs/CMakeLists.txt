add_library(ecs)

target_sources(ecs PUBLIC
    FILE_SET CXX_MODULES
        FILES ecs.cppm
)

target_link_libraries(ecs PRIVATE utils)
