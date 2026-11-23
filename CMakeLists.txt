cmake_minimum_required(VERSION 3.16)
project(chrono VERSION 0.1.0 LANGUAGES C)

option(CHRONO_BUILD_EXAMPLES "Build examples" ON)
option(CHRONO_BUILD_TESTS "Build tests" ON)

add_library(chrono STATIC src/chrono.c)
add_library(chrono::chrono ALIAS chrono)
target_compile_features(chrono PUBLIC c_std_99)
target_include_directories(chrono PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
set_target_properties(chrono PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES)

if(CHRONO_BUILD_EXAMPLES)
    add_executable(chrono_example examples/main.c)
    target_link_libraries(chrono_example PRIVATE chrono::chrono)
    if(WIN32)
        # QueryPerformanceCounter is provided by the Windows SDK.
    elseif(UNIX)
        target_compile_definitions(chrono_example PRIVATE _POSIX_C_SOURCE=200809L)
    endif()
endif()

if(CHRONO_BUILD_TESTS)
    enable_testing()
    add_executable(chrono_test tests/test_chrono.c)
    target_link_libraries(chrono_test PRIVATE chrono::chrono)
    add_test(NAME chrono_test COMMAND chrono_test)
endif()

include(GNUInstallDirs)
install(TARGETS chrono EXPORT chronoTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES include/chrono.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT chronoTargets NAMESPACE chrono:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chrono)
