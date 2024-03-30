#
# Copyright (C) 2022-2024 Nikolas Koesling <nikolas@koesling.info>.
# This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
#

cmake_minimum_required(VERSION 3.22.0 FATAL_ERROR)

# ----------------------------------------------- User settings --------------------------------------------------------
# ======================================================================================================================

# project
project(cxxshm LANGUAGES CXX VERSION 2.0.2)

set(Target cxxshm)
set(STANDARD 20)

# options
option(BUILD_DOC "Build documentation" ON)
option(COMPILER_WARNINGS "Enable compiler warnings" ON)
option(ENABLE_MULTITHREADING "Link the default multithreading library for the current target system" OFF)
option(MAKE_32_BIT_BINARY "Compile as 32 bit application. No effect on 32 bit Systems" OFF)
option(OPENMP "enable openmp" OFF)
option(OPTIMIZE_DEBUG "apply optimizations also in debug mode" ON)
option(CLANG_FORMAT "use clang-format" ON)
option(CLANG_TIDY "use clang-tidy" ON)
option(CLANG_TIDY_NO_ERRORS "do not treat clang-tidy warnings as errors" ON)
option(OPTIMIZE_FOR_ARCHITECTURE "enable optimizations for specified architecture" OFF)
option(ARCHITECTURE "CPU architecture to optimize for" "native")
option(COMPILER_EXTENSIONS "enable compiler specific C++ extensions" OFF)
option(ENABLE_TEST "enable test builds" ON)
option(STATIC_LIB "build static library" OFF)
option(INSTAL_LIB "add library to install target" ON)

# ======================================================================================================================
# ======================================================================================================================


# ----------------------------------------------- Do not change --------------------------------------------------------
# ======================================================================================================================
include(setup.cmake)
