set(component utilities)

add_library(${component} INTERFACE)
add_library(${PROJECT_NAME}::${component} ALIAS ${component})

target_include_directories(${component}
                          INTERFACE
                          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include(GNUInstallDirs)
install(TARGETS ${component} EXPORT ${component}-targets
        COMPONENT ${component}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

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

 
write_basic_package_version_file("${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${component}-config-version.cmake"
                                VERSION ${version}
                                COMPATIBILITY AnyNewerVersion)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
                             "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${component}-config.cmake"
                              INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


install(FILES
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${component}-config.cmake"
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${component}-config-version.cmake"
        DESTINATION  ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        COMPONENT    ${component})

# DESTINATION will be automatically prefixed by ${CMAKE_INSTALL_PREFIX}
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        COMPONENT ${component}
        DESTINATION ${CMAKE_INSTALL_PREFIX})