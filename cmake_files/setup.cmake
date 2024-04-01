#
# Copyright (C) 2024 Nikolas Koesling <nikolas@koesling.info>.
# This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
#

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Determine whether this is a standalone project or included by other projects
set(STANDALONE_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(STANDALONE_PROJECT ON)
endif ()

# ----------------------------------------------- clang-tidy -----------------------------------------------------------
# ======================================================================================================================
if(CLANG_TIDY)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if (${CLANG_TIDY_NO_ERRORS}) 
            set (CLANG_TIDY_CONFIG_FILE ${CMAKE_SOURCE_DIR}/.clang-tidy-noerrors)
        else()
            set (CLANG_TIDY_CONFIG_FILE ${CMAKE_SOURCE_DIR}/.clang-tidy)
        endif()

        set(CMAKE_CXX_CLANG_TIDY
                clang-tidy
                -config-file=${CLANG_TIDY_CONFIG_FILE})
        message(STATUS "clang-tidy enabled: ${CLANG_TIDY_CONFIG_FILE}")
    else()
        message(WARNING "clang-tidy requested, but only available if clang is selected as compiler")
    endif()
endif()

# ----------------------------------------------- library target -------------------------------------------------------
# ======================================================================================================================
if (STATIC_LIB)
    add_library(${Target} STATIC)
else ()
    add_library(${Target} SHARED)
endif ()

if (INSTAL_LIB)
    install(
        TARGETS ${Target}
        EXPORT ${Target}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(
        DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h*"
    )

    install(
        EXPORT ${Target}Targets
        FILE ${Target}Targets.cmake
        NAMESPACE "${Target}::"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${Target}
    )

    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake_files/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${Target}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${Target}
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${Target}ConfigVersion.cmake"
        VERSION "${version}"
        COMPATIBILITY AnyNewerVersion
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${Target}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${Target}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${Target}
    )
endif ()

# ----------------------------------------------- set source and include directory -------------------------------------
# ======================================================================================================================
add_subdirectory(src)
add_subdirectory(include)
target_include_directories(${Target} PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>  # <prefix>/include
)

# ----------------------------------------------- warnings, compiler definitions and otions ----------------------------
# ======================================================================================================================
include(cmake_files/warnings.cmake)
include(cmake_files/define.cmake)
include(cmake_files/compileropts.cmake)

# force C++ Standard and disable/enable compiler specific extensions
set_target_properties(${Target} PROPERTIES
        CXX_STANDARD ${STANDARD}
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ${COMPILER_EXTENSIONS}
)

# enable tests only for standalone project
if (ENABLE_TEST AND STANDALONE_PROJECT)
    add_subdirectory(test)
endif ()

# disable compiler warnings if project is not a standalone project
if (NOT STANDALONE_PROJECT)
    unset(COMPILER_WARNINGS)
endif ()

set_definitions(${Target})
if (ENABLE_MULTITHREADING AND OPENMP)
    set_options(${Target} ON)
else ()
    set_options(${Target} OFF)
endif ()

if (COMPILER_WARNINGS)
    enable_warnings(${Target})
    message(STATUS "Compiler warnings enabled.")
else ()
    disable_warnings(${Target})
    message(STATUS "Compiler warnings disabled.")
endif ()

if (ENABLE_MULTITHREADING)
    # required by threading lib (std::thread)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${Target} PRIVATE Threads::Threads)
endif ()

# ----------------------------------------------- doxygen documentation ------------------------------------------------
# ======================================================================================================================
if (BUILD_DOC AND NOT STANDALONE_PROJECT)
    # doxygen documentation (https://vicrucann.github.io/tutorials/quick-cmake-doxygen/)
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        if (EXISTS ${DOXYGEN_IN})
            # request to configure the file
            configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
            message(STATUS "Doxygen configured")

            # note the option ALL which allows to build the docs together with the application
            add_custom_target(doc_doxygen_${Target} ALL
                    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Generating API documentation with Doxygen"
                    VERBATIM)
            message(STATUS "Added target doc_doxygen_${Target}")

            if (TARGET doc_doxygen)
                add_dependencies(doc_doxygen doc_doxygen_${Target})
            else ()
                add_custom_target(doc_doxygen DEPENDS doc_doxygen_${Target})
            endif ()
        else ()
            message(WARNING "doxygen documentation requested, but file ${DOXYGEN_IN} does not exist.")
        endif ()
    else (DOXYGEN_FOUND)
        message(WARNING "Doxygen must be installed and accessible to generate the doxygen documentation.")
    endif (DOXYGEN_FOUND)
endif ()

# add clang format target
if (CLANG_FORMAT AND NOT STANDALONE_PROJECT)
    set(CLANG_FORMAT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/.clang-format)

    if (EXISTS ${CLANG_FORMAT_FILE})
        include(cmake_files/ClangFormat.cmake)
        target_clangformat_setup(${Target})
        message(STATUS "Added clang format target(s)")
    else ()
        message(WARNING "Clang format enabled, but file ${CLANG_FORMAT_FILE}  does not exist")
    endif ()
endif ()

# generate version_info.cpp
# output is not the acutal generated file --> command is always executed
add_custom_command(
    OUTPUT
        ${CMAKE_SOURCE_DIR}/src/generated/version_info_cpp

    COMMAND
        bash ${CMAKE_SOURCE_DIR}/scripts/gen_version_info_cpp.sh ${PROJECT_NAME}

    WORKING_DIRECTORY
        ${CMAKE_SOURCE_DIR}
)

execute_process(
    COMMAND bash "${CMAKE_SOURCE_DIR}/scripts/gen_version_info_cpp.sh" ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(${Target}_generated_version_info
    DEPENDS ${CMAKE_SOURCE_DIR}/src/generated/version_info_cpp
)

add_dependencies(${Target} ${Target}_generated_version_info)
