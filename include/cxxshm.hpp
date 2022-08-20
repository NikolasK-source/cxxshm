/*
 * Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#pragma once

#include <string>

namespace cxxshm {

/**
 * @brief Wrapper for a named shared memory
 *
 * @details
 * Creates/Connects and maps the shared memory on construction and unmaps it on destruction.
 * Related man pages:
 *  - shm_open
 *  - shm_unlink
 *  - mmap
 *  - munmap
 *  - ftruncate
 *  - fstat
 *  - close
 */
class SharedMemory final {
private:
    const std::string NAME;            //* shared memory name
    int               fd   = -1;       //* shared memory file
    std::size_t       size = 0;        //* shared memory size
    void             *addr = nullptr;  //* shared memory map address
    const bool        CREATED;         //* indicates if the shared memory was created by the instance

public:
    /**
     * @brief connect to a existing named shared memory
     *
     * @details
     * connects to a posix shared memory by calling shm_open and mmap
     *
     * @param name       name of the shared memory
     * @param read_only  connect without write permissions
     *
     * @exception system_error thrown if one of the system calls shm_open, fstat or mmap failed
     * @exception invalid_argument name is empty
     */
    explicit SharedMemory(std::string name, bool read_only = false);

    /**
     * @brief create a named shared memory
     *
     * @param name       name of the shared memory
     * @param size       size to which the shared memory is resized in bytes.
     *                   a size 0 will not change the size
     * @param read_only  connect without write permissions
     * @param exclusive  fail if a shared memory with the same name already exists
     *
     * @exception system_error thrown if one of the system calls shm_open, fstat, ftruncate or mmap failed
     * @exception invalid_argument name is empty
     */
    explicit SharedMemory(std::string name, std::size_t size, bool read_only = false, bool exclusive = true);

    /**
     * @brief unmaps the shared memory
     *
     * @details
     * If the shared memory was created by this object, it is unlinked here.
     * Otherwise it is only unmaped.
     */
    ~SharedMemory();

    /**
     * @brief get size of the shared memory object
     *
     * @return size in bytes
     */
    [[maybe_unused, nodiscard]] size_t get_size() const { return size; }

    /**
     * @brief get address to the shared memory
     *
     * @details
     * caller is responsible for selecting the right element type!
     *
     * @tparam type element type
     * @return address
     */
    template <typename type = void *>
    [[nodiscard]] type get_addr() {
        static_assert(std::is_pointer<type>::value, "Expected a pointer type");
        return reinterpret_cast<type>(addr);
    }

    /**
     * @brief get address to the shared memory
     *
     * @details
     * caller is responsible for selecting the right element type!
     *
     * @tparam type element type
     * @return address
     */
    template <typename type = const void *>
    [[nodiscard]] type get_addr() const {
        static_assert(std::is_pointer<type>::value, "Expected a pointer type");

        // required to generate a compiler error if the template is called with a pointer to non const type.
        const void *const_addr = addr;

        return reinterpret_cast<type>(const_addr);
    }

    /**
     * @brief array access to shared memory (no bounds check)
     *
     * @tparam type data type
     * @param index array index
     * @return reference to array element
     */
    template <typename type>
    [[maybe_unused, nodiscard]] type &operator[](std::size_t index) {
        return get_addr<type *>()[index];
    }

    /**
     * @brief array access to const shared memory (no bounds check)
     *
     * @tparam type data type
     * @param index array index
     * @return const reference to array element
     */
    template <typename type>
    [[maybe_unused, nodiscard]] const type &operator[](std::size_t index) const {
        return get_addr<type *>()[index];
    }

    /**
     * @brief get name of the shared memory object
     * @return name
     */
    [[maybe_unused, nodiscard]] const std::string &get_name() const { return NAME; }

    /**
     * @brief "array access" with range check
     *
     * @tparam type data type
     * @param index array index
     * @return reference to array element
     */
    template <typename type>
    [[maybe_unused, nodiscard]] type &at(std::size_t index) {
        range_check(index, sizeof(type));
        return get_addr<type *>()[index];
    }

    /**
     * @brief const "array access" with range check
     *
     * @tparam type data type
     * @param index array index
     * @return const reference to array element
     */
    template <typename type>
    [[maybe_unused, nodiscard]] const type &at(std::size_t index) const {
        range_check(index, sizeof(type));
        return get_addr<type *>()[index];
    }

private:
    /**
     * @brief internal method: preform a range check
     * @param index access index
     * @param element_size element size (use sizeof)
     * @exception std::out_of_range index is out of range
     */
    void range_check(std::size_t index, std::size_t element_size) const;
};

/**
 * @brief get library version
 * @return library version as string
 */
[[maybe_unused, nodiscard]] std::string get_lib_version();

/**
 * @brief get full library info
 * @details contains name, version, compiler and system
 * @return library info as string
 */
[[maybe_unused, nodiscard]] std::string get_lib_info();

/**
 * @brief get library compilation date
 * @return compilation date as string
 */
[[maybe_unused, nodiscard]] std::string get_lib_date();

}  // namespace cxxshm

void example();
