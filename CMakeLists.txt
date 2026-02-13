cmake_minimum_required(VERSION 3.19)
project(nshader VERSION 1.0.0 LANGUAGES C CXX)

#
# Configuration Options
#

option(NSHADER_BUILD_SHARED "Build nshader as a shared library" OFF)
option(NSHADER_BUILD_COMPILER "Build nshader-compiler library" ON)
option(NSHADER_BUILD_TESTS "Build nshader tests" ON)
option(NSHADER_BUILD_CLI "Build nshader command line interface" ON)
option(NSHADER_GTEST_FETCH "Fetch GoogleTest if not found" ON)
option(NSHADER_SDL3_FETCH "Fetch SDL3 if not found" ON)
option(NSHADER_SHADERCROSS_FETCH "Fetch SDL_shadercross if not found" ON)

#
# C Standard
#

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

#
# C++ Standard (for tests)
#

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

#
# Find or Fetch SDL3 (required by nshader_sdl3_gpu)
#

find_package(SDL3 3.1.3 CONFIG QUIET)

if(NOT SDL3_FOUND AND NSHADER_SDL3_FETCH)
    message(STATUS "SDL3 not found, fetching from source...")

    set(SDL_SHARED OFF CACHE BOOL "Build SDL3 as shared library" FORCE)
    set(SDL_STATIC ON CACHE BOOL "Build SDL3 as static library" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "Build SDL3 test library" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "Build SDL3 tests" FORCE)

    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG main
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL3)
endif()

#
# Find or Fetch SDL_shadercross (only when compiler is enabled)
#

if(NSHADER_BUILD_COMPILER)
    find_package(SDL3_shadercross CONFIG QUIET)

    if(SDL3_shadercross_FOUND)
        message(STATUS "Found SDL3_shadercross")
    elseif(NSHADER_SHADERCROSS_FETCH)
        message(STATUS "SDL_shadercross not found, fetching from source...")

        set(SDLSHADERCROSS_VENDORED ON CACHE BOOL "Use vendored dependencies" FORCE)
        set(SDLSHADERCROSS_TESTS OFF CACHE BOOL "Build tests for SDL_shadercross" FORCE)
        set(SDLSHADERCROSS_INSTALL OFF CACHE BOOL "Enable SDL_shadercross installation" FORCE)
        set(SDLSHADERCROSS_CLI OFF CACHE BOOL "Build SDL_shadercross CLI" FORCE)
        set(SDLSHADERCROSS_STATIC ON CACHE BOOL "Build static SDL_shadercross library" FORCE)

        FetchContent_Declare(
            SDL_shadercross
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_shadercross.git
            GIT_TAG 7b7365a86611b2a7b6462e521cf1c43a037d0970
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(SDL_shadercross)
    endif()
endif()

# ###########################################################################
# nshader (format) library
# ###########################################################################

file(GLOB_RECURSE NSHADER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/format/src/*.c
)
file(GLOB_RECURSE NSHADER_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/format/include/*.h
)

if(NSHADER_BUILD_SHARED)
    add_library(nshader SHARED ${NSHADER_SOURCES} ${NSHADER_HEADERS})
    target_compile_definitions(nshader PRIVATE NSHADER_EXPORTS)
    target_compile_definitions(nshader PUBLIC NSHADER_SHARED)
    set_target_properties(nshader PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
else()
    add_library(nshader-static STATIC ${NSHADER_SOURCES} ${NSHADER_HEADERS})
    add_library(nshader::static ALIAS nshader-static)
    set_target_properties(nshader-static PROPERTIES
        VERSION ${PROJECT_VERSION}
        OUTPUT_NAME nshader
    )
endif()

if(NSHADER_BUILD_SHARED)
    set(NSHADER_TARGET nshader)
else()
    set(NSHADER_TARGET nshader-static)
endif()

target_include_directories(${NSHADER_TARGET}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/format/include>
        $<INSTALL_INTERFACE:include>
)

# Link SDL3 (needed by nshader_sdl3_gpu)
if(TARGET SDL3::SDL3-static)
    target_link_libraries(${NSHADER_TARGET}
        PRIVATE
            SDL3::SDL3-static
    )
elseif(TARGET SDL3::SDL3)
    target_link_libraries(${NSHADER_TARGET}
        PRIVATE
            SDL3::SDL3
    )
endif()

# ###########################################################################
# nshader-compiler library (optional)
# ###########################################################################

if(NSHADER_BUILD_COMPILER)
    file(GLOB_RECURSE NSHADER_COMPILER_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/compiler/src/*.c
    )
    file(GLOB_RECURSE NSHADER_COMPILER_HEADERS CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/compiler/include/*.h
    )

    if(NSHADER_BUILD_SHARED)
        add_library(nshader-compiler SHARED ${NSHADER_COMPILER_SOURCES} ${NSHADER_COMPILER_HEADERS})
        target_compile_definitions(nshader-compiler PRIVATE NSHADER_EXPORTS)
        target_compile_definitions(nshader-compiler PUBLIC NSHADER_SHARED)
        set_target_properties(nshader-compiler PROPERTIES
            VERSION ${PROJECT_VERSION}
            SOVERSION ${PROJECT_VERSION_MAJOR}
        )
        set(NSHADER_COMPILER_TARGET nshader-compiler)
    else()
        add_library(nshader-compiler-static STATIC ${NSHADER_COMPILER_SOURCES} ${NSHADER_COMPILER_HEADERS})
        add_library(nshader::compiler-static ALIAS nshader-compiler-static)
        set_target_properties(nshader-compiler-static PROPERTIES
            VERSION ${PROJECT_VERSION}
            OUTPUT_NAME nshader-compiler
        )
        set(NSHADER_COMPILER_TARGET nshader-compiler-static)
    endif()

    target_include_directories(${NSHADER_COMPILER_TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/compiler/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/format/src
    )

    # nshader-compiler depends on nshader
    target_link_libraries(${NSHADER_COMPILER_TARGET}
        PUBLIC
            ${NSHADER_TARGET}
    )

    # Link SDL3
    if(TARGET SDL3::SDL3-static)
        target_link_libraries(${NSHADER_COMPILER_TARGET}
            PRIVATE
                SDL3::SDL3-static
        )
    elseif(TARGET SDL3::SDL3)
        target_link_libraries(${NSHADER_COMPILER_TARGET}
            PRIVATE
                SDL3::SDL3
        )
    endif()

    # Link SDL_shadercross
    if(TARGET SDL3_shadercross::SDL3_shadercross)
        target_link_libraries(${NSHADER_COMPILER_TARGET}
            PRIVATE
                SDL3_shadercross::SDL3_shadercross
        )
    elseif(TARGET SDL3_shadercross-static)
        target_link_libraries(${NSHADER_COMPILER_TARGET}
            PRIVATE
                SDL3_shadercross-static
        )
    elseif(TARGET SDL_shadercross)
        target_link_libraries(${NSHADER_COMPILER_TARGET}
            PRIVATE
                SDL_shadercross
        )
    endif()

    message(STATUS "Building nshader-compiler library")
endif()

# ###########################################################################
# CLI Tool (Optional, requires compiler)
# ###########################################################################

if(NSHADER_BUILD_CLI)
    if(NOT NSHADER_BUILD_COMPILER)
        message(WARNING "CLI requires NSHADER_BUILD_COMPILER=ON, disabling CLI")
    else()
        file(GLOB_RECURSE NSHADER_CLI_SOURCES CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/cli/*.c
        )

        add_executable(nshader-cli ${NSHADER_CLI_SOURCES})

        target_link_libraries(nshader-cli
            PRIVATE
                ${NSHADER_COMPILER_TARGET}
        )

        set_target_properties(nshader-cli PROPERTIES
            OUTPUT_NAME nshader
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )

        message(STATUS "Building CLI tool")
    endif()
endif()

# ###########################################################################
# Tests (Optional)
# ###########################################################################

if(NSHADER_BUILD_TESTS)
    if(NSHADER_GTEST_FETCH)
        message(STATUS "Fetching GoogleTest framework...")

        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.15.2
            GIT_SHALLOW TRUE
        )
        # For Windows: Use static runtime to match project settings
        set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    enable_testing()

    file(GLOB_RECURSE NSHADER_TEST_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
    )

    add_executable(nshader-tests
        ${NSHADER_TEST_SOURCES}
    )

    target_link_libraries(nshader-tests
        PRIVATE
            ${NSHADER_TARGET}
            GTest::gtest_main
    )

    if(NSHADER_BUILD_COMPILER)
        target_link_libraries(nshader-tests
            PRIVATE
                ${NSHADER_COMPILER_TARGET}
        )
    endif()

    include(GoogleTest)
    gtest_discover_tests(nshader-tests DISCOVERY_MODE PRE_TEST)

    message(STATUS "Building tests with GoogleTest")
endif()

# ###########################################################################
# Installation
# ###########################################################################

include(GNUInstallDirs)

install(TARGETS ${NSHADER_TARGET}
    EXPORT nshaderTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY format/include/nshader
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(FILES format/include/nshader.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(NSHADER_BUILD_COMPILER)
    install(TARGETS ${NSHADER_COMPILER_TARGET}
        EXPORT nshaderTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(DIRECTORY compiler/include/nshader
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )
endif()

if(NSHADER_BUILD_CLI AND NSHADER_BUILD_COMPILER)
    install(TARGETS nshader-cli
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Only install export targets if dependencies were found via find_package
# (not FetchContent), to avoid exporting targets that don't have export sets
if(NSHADER_BUILD_COMPILER AND SDL3_shadercross_FOUND)
    install(EXPORT nshaderTargets
        FILE nshaderTargets.cmake
        NAMESPACE nshader::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nshader
    )

    message(STATUS "Export targets will be installed")
else()
    message(STATUS "Skipping export targets (using FetchContent dependencies)")
endif()
