set(component reader)

add_library(${component} INTERFACE)
add_library(${PROJECT_NAME}::${component} ALIAS ${component})

# use the either the include directories in this root or in the install folder, depending on the build.
target_include_directories(${component}
                          INTERFACE
                          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(reader INTERFACE ${PROJECT_NAME}::utilities)

include(GNUInstallDirs)

# export targets contain info as to where the importer can find the relevant files (.hpp headers, .a/.dll archives etc)
install(TARGETS ${component} EXPORT ${component}-targets
        COMPONENT ${component}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# write all these info into a "MyLibComponent-target.cmake" file, and use "MyLib" as namespace to include component
install(EXPORT ${component}-targets
        FILE "${PROJECT_NAME}-${component}-targets.cmake"
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        COMPONENT ${component}
)

# Directory variables like CMAKE_INSTALL_LIBDIR etc
include(GNUInstallDirs)


# Helper functions to create library-config.cmake from Config.cmake.in
include(CMakePackageConfigHelpers)

# Create the MyLibComponentConfig.cmake (or mylib-component-config.cmake) from Config.cmake.in
# [here /${PROJECT_NAME}${component}Config.cmake == CoolMathLibGeometryConfig.cmake but coolmathlib-geometry-config.cmake also works]

# These will help cmake find the package and contains the address of  MyLibComponentConfig-target.cmake (or mylib-component-target.cmake) file.
# It can also contain the path to other config.cmake file, for example, dependencies of MyLibComponent etc...
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
                             "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${component}-config.cmake"
                             INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# Version control file.
write_basic_package_version_file("${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${component}-config-version.cmake"
                                VERSION ${PROJECT_VERSION}
                                COMPATIBILITY AnyNewerVersion)

# install the Config files & version control file into our install folder (the lib file in unix, or where you put all your thirs party libraries)
install(FILES
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${component}-config.cmake"
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${component}-config-version.cmake"
        DESTINATION  ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        COMPONENT    ${component})

# install the header files.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        COMPONENT ${component}
        DESTINATION ${CMAKE_INSTALL_PREFIX})