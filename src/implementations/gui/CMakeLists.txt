target_sources(ecs PUBLIC
    FILE_SET CXX_MODULES
        FILES SFML.cppm
)

find_package(SFML 2.6 COMPONENTS graphics window system CONFIG REQUIRED)

target_link_libraries(ecs PRIVATE sfml-graphics sfml-window sfml-system)
