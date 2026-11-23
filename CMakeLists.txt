cmake_minimum_required(VERSION 3.16)
project(chrono VERSION 0.1.0 LANGUAGES C)

option(CHRONO_BUILD_EXAMPLES "Build examples" ON)
option(CHRONO_BUILD_TESTS "Build tests" ON)
option(CHRONO_BUILD_CLI "Build the persistent elapsed-time CLI" ON)

add_library(chrono STATIC src/chrono.c)
add_library(chrono::chrono ALIAS chrono)
target_compile_features(chrono PUBLIC c_std_99)
target_include_directories(chrono PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
set_target_properties(chrono PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES)

if(WIN32 OR UNIX)
    target_sources(chrono PRIVATE src/chrono_clock.c)
    if(NOT WIN32)
        set_source_files_properties(src/chrono_clock.c PROPERTIES COMPILE_DEFINITIONS _POSIX_C_SOURCE=200809L)
    endif()
endif()

if(CHRONO_BUILD_EXAMPLES)
    add_executable(chrono_example examples/main.c)
    target_link_libraries(chrono_example PRIVATE chrono::chrono)
endif()

if(CHRONO_BUILD_TESTS)
    enable_testing()
    add_executable(chrono_test tests/test_chrono.c)
    target_link_libraries(chrono_test PRIVATE chrono::chrono)
    add_test(NAME chrono_test COMMAND chrono_test)
endif()

# project() initializes the prefix, so only replace CMake's default.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if(WIN32 AND DEFINED ENV{LOCALAPPDATA} AND NOT "$ENV{LOCALAPPDATA}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{LOCALAPPDATA}/Programs/chrono" _default_install_prefix)
    elseif(UNIX AND DEFINED ENV{HOME} AND NOT "$ENV{HOME}" STREQUAL "")
        set(_default_install_prefix "$ENV{HOME}/.local")
    endif()
    if(DEFINED _default_install_prefix)
        set(CMAKE_INSTALL_PREFIX "${_default_install_prefix}" CACHE PATH
            "Install prefix for chrono" FORCE)
        unset(_default_install_prefix)
    endif()
endif()

include(GNUInstallDirs)
if(CHRONO_BUILD_CLI AND NOT CMAKE_CROSSCOMPILING AND (WIN32 OR UNIX))
    add_executable(chrono_cli src/chrono_cli.c)
    target_link_libraries(chrono_cli PRIVATE chrono::chrono)
    set_target_properties(chrono_cli PROPERTIES OUTPUT_NAME chrono)
    if(NOT WIN32)
        target_compile_definitions(chrono_cli PRIVATE _POSIX_C_SOURCE=200809L)
    endif()
    install(TARGETS chrono_cli RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
install(TARGETS chrono EXPORT chronoTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES include/chrono.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT chronoTargets NAMESPACE chrono:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chrono)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/chronoConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/chronoConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chrono)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/chronoConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/chronoConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/chronoConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chrono)
