#
# Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
# This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
#

# warnings that are valid for gcc and clang
function(commonwarn target)
    target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic -pedantic-errors)

    # see https://gcc.gnu.org/onlinedocs/gcc-4.3.2/gcc/Warning-Options.html for more details

    target_compile_options(${target} PRIVATE -Wnull-dereference)
    target_compile_options(${target} PRIVATE -Wold-style-cast)
    target_compile_options(${target} PRIVATE -Wdouble-promotion)
    target_compile_options(${target} PRIVATE -Wformat=2)
    target_compile_options(${target} PRIVATE -Winit-self)
    target_compile_options(${target} PRIVATE -Wsequence-point)
    target_compile_options(${target} PRIVATE -Wswitch-default)
    target_compile_options(${target} PRIVATE -Wswitch-enum -Wno-error=switch-enum)
    target_compile_options(${target} PRIVATE -Wconversion)
    target_compile_options(${target} PRIVATE -Wcast-align)
    target_compile_options(${target} PRIVATE -Wfloat-equal)
    target_compile_options(${target} PRIVATE -Wundef)
    target_compile_options(${target} PRIVATE -Wcast-qual)
endfunction()

# gcc specific warnings
function(gccwarn target)
    # see https://gcc.gnu.org/onlinedocs/gcc-4.3.2/gcc/Warning-Options.html for more details

    target_compile_options(${target} PRIVATE -Wduplicated-cond)
    target_compile_options(${target} PRIVATE -Wduplicated-branches)
    target_compile_options(${target} PRIVATE -Wlogical-op)
    target_compile_options(${target} PRIVATE -Wrestrict)
    target_compile_options(${target} PRIVATE -Wuseless-cast -Wno-error=useless-cast)
    target_compile_options(${target} PRIVATE -Wshadow=local -Wno-error=shadow)

    target_compile_options(${target} PRIVATE -Wno-error=switch-default)
    target_compile_options(${target} PRIVATE -Wno-error=attributes)
endfunction()

# clang specific warnings
function(clangwarn target)
    # enable all
    target_compile_options(${target} PRIVATE -Weverything)

    # and remove "useless" ones
    target_compile_options(${target} PRIVATE -Wno-c++98-compat)
    target_compile_options(${target} PRIVATE -Wno-c++98-c++11-c++14-compat)
    target_compile_options(${target} PRIVATE -Wno-c++98-compat-pedantic)
    target_compile_options(${target} PRIVATE -Wno-error=covered-switch-default)
    target_compile_options(${target} PRIVATE -Wno-shadow-field-in-constructor)
    target_compile_options(${target} PRIVATE -Wno-padded)
    target_compile_options(${target} PRIVATE -Wno-shadow-field)
    target_compile_options(${target} PRIVATE -Wno-weak-vtables)
    target_compile_options(${target} PRIVATE -Wno-exit-time-destructors)
    target_compile_options(${target} PRIVATE -Wno-global-constructors)
    target_compile_options(${target} PRIVATE -Wno-error=unreachable-code-return)
    target_compile_options(${target} PRIVATE -Wno-error=unreachable-code)
    target_compile_options(${target} PRIVATE -Wno-error=documentation)
    target_compile_options(${target} PRIVATE -Wno-error=unused-exception-parameter)
    target_compile_options(${target} PRIVATE -Wno-nested-anon-types)
    target_compile_options(${target} PRIVATE -Wno-gnu-anonymous-struct)
    target_compile_options(${target} PRIVATE -Wno-source-uses-openmp)
    target_compile_options(${target} PUBLIC -Wno-unsafe-buffer-usage)

endfunction()

function(enable_warnings target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        commonwarn(${target})
        gccwarn(${target})
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        commonwarn(${target})
        clangwarn(${target})
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${target} PRIVATE /Wall /WX)
    endif()
endfunction()

function(disable_warnings target)
    target_compile_options(${target} PRIVATE -w)
endfunction()
