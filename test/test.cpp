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
            if (access(shm_path.str().c_str(), F_OK) != 0) {
                std::cerr << "Test failed: Shared memory does not exist" << std::endl;
                return EXIT_FAILURE;
            }

            // try to open again
            try {
                cxxshm::SharedMemory shm2(SHM_NAME, SHM_SIZE, false, true);
                std::cerr << "Test failed: Can open again in exclusive mode" << std::endl;
                return EXIT_FAILURE;
            } catch (std::system_error &) {}

            // connect to shared memory
            cxxshm::SharedMemory shm3(SHM_NAME);
        }

        // check if shm exists
        std::ostringstream shm_path;
        shm_path << "/dev/shm/" << SHM_NAME;
        if (access(shm_path.str().c_str(), F_OK) != -1) {
            std::cerr << "Test failed: Shared memory was not closed" << std::endl;
            return EXIT_FAILURE;
        }
    } catch (const std::exception &e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
