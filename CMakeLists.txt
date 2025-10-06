cmake_minimum_required(VERSION 4.0)

project(parameter_parser VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(utilities)
add_subdirectory(reader)

if(ADD_EXAMPLE)
    message(STATUS "building test example")
	add_subdirectory(example)
endif()

if(ENABLE_TESTING)
    message(STATUS "Building google tests")
	add_subdirectory(tests)
    include(CTest)
endif()

include(GNUInstallDirs)

include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
                                "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
                                INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

write_basic_package_version_file("${CMAKE_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
                                VERSION ${PROJECT_VERSION}
                                COMPATIBILITY AnyNewerVersion)

install(FILES
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})