#include "cxxshm.hpp"
#include "cxxshm_version_info.hpp"

#include <iostream>
#include <sstream>
#include <unistd.h>

#ifndef OS_LINUX
#    error The test only works on linux !
#endif

[[noreturn]] static void test_fail(std::size_t line, const char *reason) {
    std::cerr << "Test failed: " << (reason) << " (" << (line) << ')' << '\n';
    exit(EXIT_FAILURE);
}

int main() {
    const auto         shm_name = "cxxshm_test_1_" + cxxshm_version_info::GIT_HASH;
    std::ostringstream shm_path;
    shm_path << "/dev/shm/" << shm_name;
    {
        cxxshm::SharedMemory shm(shm_name, 0, false, true);

        // check if shm exists
        if (access(shm_path.str().c_str(), F_OK) == -1) test_fail(__LINE__, "Shared memory does not exist");
    }

    // check if shm exists
    if (access(shm_path.str().c_str(), F_OK) != -1) test_fail(__LINE__, "Shared memory was not closed");
}
