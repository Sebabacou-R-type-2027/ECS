add_library(utils)

target_sources(utils PUBLIC
    FILE_SET CXX_MODULES
        FILES utils.cppm
            containers.cppm
            types.cppm
            utilities.cppm
)
