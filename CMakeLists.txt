cmake_minimum_required(VERSION 4.0)

project(parameter_parser VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)


if(ADD_EXAMPLE)
    message(STATUS "building test example")
	add_subdirectory(example)
endif()

if(ENABLE_TESTING)
    message(STATUS "Building google tests")
	add_subdirectory(tests)
    include(CTest)
endif()

add_library(parameter_parser INTERFACE)
add_library(parameter_parser::parameter_parser ALIAS parameter_parser)


include(GNUInstallDirs)

target_include_directories(parameter_parser
                           INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS parameter_parser
        EXPORT parameter_parser-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_PREFIX})


install(EXPORT parameter_parser-targets
        FILE parameter_parser-targets.cmake
        NAMESPACE parameter_parser::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/parameter_parser
)

include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
                                "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
                                INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/parameter_parser)

write_basic_package_version_file("${CMAKE_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
                                VERSION ${PROJECT_VERSION}
                                COMPATIBILITY AnyNewerVersion)

install(FILES
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/parameter_parser)