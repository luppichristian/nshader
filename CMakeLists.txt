cmake_minimum_required(VERSION 3.19)
project(nshader VERSION 1.0.0 LANGUAGES C)

#
# Configuration Options
#

option(NSHADER_BUILD_SHARED "Build nshader as a shared library" OFF)
option(NSHADER_BUILD_TESTS "Build nshader tests" ON)
option(NSHADER_BUILD_CLI "Build nshader command line interface" ON)
option(NSHADER_UNITY_FETCH "Fetch Unity if not found" ON)
option(NSHADER_SHADERCROSS_FETCH "Fetch SDL_shadercross if not found" ON)

#
# C Standard
#

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(FetchContent)

#
# Find or Fetch SDL3 (required by SDL_shadercross)
#

find_package(SDL3 3.1.3 CONFIG QUIET)

if(NOT SDL3_FOUND AND NSHADER_SHADERCROSS_FETCH)
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
# Find or Fetch SDL_shadercross
#

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
        GIT_TAG main
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL_shadercross)
endif()

#
# Library Sources
#

file(GLOB_RECURSE NSHADER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)
file(GLOB_RECURSE NSHADER_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

#
# Test Sources
#

file(GLOB_RECURSE NSHADER_TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c
)

#
# CLI Sources
#

file(GLOB_RECURSE NSHADER_CLI_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/cli/*.c
)

#
# Build Library (Static or Shared)
#

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

#
# Set Common Properties for the Active Target
#

if(NSHADER_BUILD_SHARED)
    set(NSHADER_TARGET nshader)
else()
    set(NSHADER_TARGET nshader-static)
endif()

target_include_directories(${NSHADER_TARGET}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

#
# Link SDL_shadercross
#

if(TARGET SDL3_shadercross::SDL3_shadercross)
    target_link_libraries(${NSHADER_TARGET}
        PRIVATE
            SDL3_shadercross::SDL3_shadercross
    )
elseif(TARGET SDL3_shadercross-static)
    target_link_libraries(${NSHADER_TARGET}
        PRIVATE
            SDL3_shadercross-static
    )
elseif(TARGET SDL_shadercross)
    target_link_libraries(${NSHADER_TARGET}
        PRIVATE
            SDL_shadercross
    )
endif()

#
# CLI Tool (Optional)
#

if(NSHADER_BUILD_CLI)
    add_executable(nshader-cli ${NSHADER_CLI_SOURCES})

    target_link_libraries(nshader-cli
        PRIVATE
            ${NSHADER_TARGET}
    )

    set_target_properties(nshader-cli PROPERTIES
        OUTPUT_NAME nshader
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )

    message(STATUS "Building CLI tool")
endif()

#
# Tests (Optional)
#

if(NSHADER_BUILD_TESTS)
    if(NSHADER_UNITY_FETCH)
        message(STATUS "Fetching Unity test framework...")

        FetchContent_Declare(
            unity
            GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
            GIT_TAG v2.6.0
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(unity)
    endif()

    enable_testing()

    add_executable(nshader-tests
        ${NSHADER_TEST_SOURCES}
    )

    target_link_libraries(nshader-tests
        PRIVATE
            ${NSHADER_TARGET}
            unity
    )

    add_test(NAME nshader-tests COMMAND nshader-tests)

    message(STATUS "Building tests with Unity")
endif()

#
# Installation
#

include(GNUInstallDirs)

install(TARGETS ${NSHADER_TARGET}
    EXPORT nshaderTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(NSHADER_BUILD_CLI)
    install(TARGETS nshader-cli
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

install(FILES ${NSHADER_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/nshader
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Only install export targets if dependencies were found via find_package
# (not FetchContent), to avoid exporting targets that don't have export sets
if(SDL3_shadercross_FOUND)
    install(EXPORT nshaderTargets
        FILE nshaderTargets.cmake
        NAMESPACE nshader::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nshader
    )

    message(STATUS "Export targets will be installed")
else()
    message(STATUS "Skipping export targets (using FetchContent dependencies)")
endif()
