/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "cxxshm.hpp"

#include <iostream>
#include <sstream>
#include <unistd.h>

#ifndef OS_LINUX
#    error The test only works on linux !
#endif

#define test_fail(line, reason)                                                                                        \
    do {                                                                                                               \
        std::cerr << "Test failed: " << reason << " (" << line << ')' << std::endl;                                    \
        return EXIT_FAILURE;                                                                                           \
    } while (false)

int main() {
    std::cout << cxxshm::get_lib_version() << std::endl;
    std::cout << cxxshm::get_lib_info() << std::endl;
    std::cout << cxxshm::get_lib_date() << std::endl;

    constexpr std::size_t SHM_SIZE = 256;
#ifdef COMPILER_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdate-time"
#endif
    const std::string SHM_NAME = "cxxshm_test_" __TIME__;
#ifdef COMPILER_CLANG
#    pragma clang diagnostic pop
#endif

    try {
        {
            // create shared memory
            cxxshm::SharedMemory shm(SHM_NAME, SHM_SIZE, false, true);

            // check if shm exists
            std::ostringstream shm_path;
            shm_path << "/dev/shm/" << SHM_NAME;
            if (access(shm_path.str().c_str(), F_OK) != 0) { test_fail(__LINE__, "Shared memory does not exist"); }

            // try to open again
            try {
                cxxshm::SharedMemory shm2(SHM_NAME, SHM_SIZE, false, true);
                test_fail(__LINE__, "Can open again in exclusive mode");
            } catch (std::system_error &) {}

            // connect to shared memory
            cxxshm::SharedMemory shm3(SHM_NAME);

            // check size
            if (shm3.get_size() != SHM_SIZE) { test_fail(__LINE__, "wrong size"); }

            // rw test
            shm3.at<int>(5) = 0x42;
            if (shm.at<int>(5) != 0x42) { test_fail(__LINE__, "not the same value"); }

            // out of range
            try {
                auto x = shm3.at<int>(SHM_SIZE);
                static_cast<void>(x);
                test_fail(__LINE__, "oor access");
            } catch (const std::out_of_range &) {}

            // connect to const shared memory
            const cxxshm::SharedMemory shm4(SHM_NAME);

            // check size
            if (shm4.get_size() != SHM_SIZE) { test_fail(__LINE__, "wrong size"); }

            // read test
            if (shm4.at<int>(5) != 0x42) { test_fail(__LINE__, "not the same value"); }

            // out of range
            try {
                auto x = shm4.at<int>(SHM_SIZE);
                static_cast<void>(x);
                test_fail(__LINE__, "oor access");
            } catch (const std::out_of_range &) {}
        }

        // check if shm exists
        std::ostringstream shm_path;
        shm_path << "/dev/shm/" << SHM_NAME;
        if (access(shm_path.str().c_str(), F_OK) != -1) { test_fail(__LINE__, "Shared memory was not closed"); }
    } catch (const std::exception &e) { test_fail(__LINE__, e.what()); }

    return EXIT_SUCCESS;
}
