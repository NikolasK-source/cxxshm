/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "cxxshm.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <system_error>
#include <unistd.h>

#include <utility>

namespace cxxshm {

/**
 * @brief internal function: call mmap for a shared memory that was created/connected with shm_open
 *
 * @param size       shared memory size in bytes
 * @param fd         file handler
 * @param read_only  read only flag
 * @param name       shared memory name (for exception only)
 * @return           pointer to mapped shared memory
 *
 * @exception system_error thrown if the system call mmap failed
 */
static void *mmap_shm(std::size_t size, int fd, bool read_only, const std::string &name) {
    int prot = PROT_READ;
    if (!read_only) prot |= PROT_WRITE;

    auto addr = mmap(nullptr, size, prot, MAP_SHARED, fd, 0);

    if (addr == MAP_FAILED || addr == nullptr) {
        if (close(fd)) { perror("close"); }
        throw std::system_error(errno, std::generic_category(), "Failed to mmap shared memory '" + name + '\'');
    }

    return addr;
}

static void check_name(const std::string &name) {
    if (name.empty()) throw std::invalid_argument("name is empty");
}

SharedMemory::SharedMemory(std::string name, bool read_only) : NAME(std::move(name)), CREATED(false) {
    check_name(NAME);

    // open shared memory object
    fd = shm_open(NAME.c_str(), read_only ? O_RDONLY : O_RDWR, 0);
    if (fd < 0) {
        throw std::system_error(errno, std::generic_category(), "Failed to open shared memory '" + NAME + '\'');
    }

    // get size of shared memory object
    struct stat shm_stats {};
    if (fstat(fd, &shm_stats)) {
        if (close(fd)) { perror("close"); }
        fd = -1;
        throw std::system_error(
                errno, std::generic_category(), "Failed to read stats of shared memory '" + NAME + '\'');
    }
    size = static_cast<size_t>(shm_stats.st_size);

    // map shared memory
    addr = size ? mmap_shm(size, fd, read_only, NAME) : nullptr;
}

SharedMemory::SharedMemory(std::string name, std::size_t size, bool read_only, bool exclusive, mode_t mode)
    : NAME(std::move(name)), CREATED(true) {
    check_name(NAME);

    int flags = O_CREAT;
    flags |= read_only ? O_RDONLY : O_RDWR;
    if (exclusive) flags |= O_EXCL;

    // only the lowest 9 bit are relevant
    mode &= 0x1FF;

    // open shared memory object
    fd = shm_open(NAME.c_str(), flags, mode);
    if (fd < 0) {
        throw std::system_error(errno, std::generic_category(), "Failed to open shared memory '" + NAME + '\'');
    }

    if (size) {
        // set size
        if (ftruncate(fd, static_cast<__off_t>(size))) {
            throw std::system_error(errno, std::generic_category(), "Failed to resize shared memory '" + NAME + '\'');
        }
        this->size = size;
    } else {
        // get size
        struct stat shm_stats {};
        if (fstat(fd, &shm_stats)) {
            if (close(fd)) { perror("close"); }
            fd = -1;
            throw std::system_error(
                    errno, std::generic_category(), "Failed to read stats of shared memory '" + NAME + '\'');
        }
        this->size = static_cast<size_t>(shm_stats.st_size);
    }

    // map shared memory
    addr = size ? mmap_shm(size, fd, read_only, NAME) : nullptr;
}

SharedMemory::~SharedMemory() {
    if (addr) {
        if (munmap(addr, size)) { perror("munmap"); }
    }

    if (fd != -1) {
        if (close(fd)) { perror("close"); }

        // only unlink shared memories that were created before
        if (CREATED) {
            if (shm_unlink(NAME.c_str())) { perror("shm_unlink"); }
        }
    }
}

void SharedMemory::range_check(std::size_t index, std::size_t element_size) const {
    if ((index + 1) * element_size > size) { throw std::out_of_range("index out of range"); }
}

std::string get_lib_version() { return {PROJECT_VERSION}; }

std::string get_lib_info() { return {PROJECT_NAME " " PROJECT_VERSION " - " COMPILER_INFO " on " SYSTEM_INFO}; }

std::string get_lib_date() {
#ifdef COMPILER_CLANG
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdate-time"
#endif
    return {__DATE__ " " __TIME__};
#ifdef COMPILER_CLANG
#    pragma clang diagnostic pop
#endif
}

}  // namespace cxxshm
