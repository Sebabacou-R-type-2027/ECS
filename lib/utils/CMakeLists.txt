add_library(utils)

target_sources(utils PUBLIC
    FILE_SET CXX_MODULES
        FILES utils.cppm
        types.cppm
)

install(TARGETS utils EXPORT ecs
    DESTINATION utils
    COMPONENT utils
    FILE_SET CXX_MODULES DESTINATION modules COMPONENT modules
)
