#
# Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
# This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
#

cmake_minimum_required(VERSION 3.13.4 FATAL_ERROR)

project(cxxshm LANGUAGES CXX VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)

set(Target "${CMAKE_PROJECT_NAME}")

add_library(${Target})
install(TARGETS ${Target})

add_subdirectory("src")
add_subdirectory("include")
target_include_directories(${Target} PUBLIC include)

include(warnings.cmake)
include(define.cmake)

set_target_properties(${Target} PROPERTIES
        CXX_STANDARD ${CMAKE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        )

# Determine whether this is a standalone project or included by other projects
set(STANDALONE_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(STANDALONE_PROJECT ON)
endif()

if(STANDALONE_PROJECT)
    enable_warnings(${Target})
else()
    disable_warnings(${Target})
endif()

set_definitions(${Target})


if(STANDALONE_PROJECT)
    # doxygen documentation (https://vicrucann.github.io/tutorials/quick-cmake-doxygen/)
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        if(EXISTS ${DOXYGEN_IN})
            # request to configure the file
            configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
            message(STATUS "Doxygen configured")

            # note the option ALL which allows to build the docs together with the application
            add_custom_target( doc_doxygen ALL
                    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Generating API documentation with Doxygen"
                    VERBATIM )
            message(STATUS "Added target doc_doxygen")
        else()
            message(WARNING "doxygen documentation requested, but file ${DOXYGEN_IN} does not exist.")
        endif()
    else (DOXYGEN_FOUND)
        message(WARNING "Doxygen need to be installed and accessible to generate the doxygen documentation.")
    endif (DOXYGEN_FOUND)

    # add clang format target
    set(CLANG_FORMAT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/.clang-format)

    if(EXISTS ${CLANG_FORMAT_FILE})
        set(CLANG_FORMAT_ENABLED ON)
        include(ClangFormat.cmake)
        target_clangformat_setup(${Target})
        message(STATUS "Added clang format target(s)")
    else()
        message(WARNING "Clang format enabled, but file ${CLANG_FORMAT_FILE}  does not exist")
    endif()

    # add test targets
    enable_testing()
    add_subdirectory(test)
endif()
