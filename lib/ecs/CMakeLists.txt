add_library(ecs)

target_sources(ecs PUBLIC
    FILE_SET CXX_MODULES
        FILES ecs.cppm
)
