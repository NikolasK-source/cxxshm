# CxxSHM

A C++ Library to handle POSIX shared memory

## Usage

### Initialize cxxshm::SharedMemory

#### Option A: Create Shared Memory

```c++
const std::string shm_name     = "example_shm";  // shared memory name
constexpr std::size_t shm_size = 4096;           // shared memory size
constexpr bool read_only       = false;          // allow read and write
constexpr bool exclusive       = true;           // fail if shared memory already exists
constexpr mode_t shm_mode      = 0660;           // read and write access for user and group

// create shared memory
cxxshm::SharedMemory shm(shm_name, shm_size, read_only, exclusive, shm_mode);
```

#### Option B: Connect to Existing Shared Memory

```c++
const std::string shm_name     = "example_shm";  // shared memory name
constexpr bool read_only       = false;          // allow read and write

// use shared memory
cxxshm::SharedMemory shm(shm_name, read_only);
```

### Access Shared Memory Data

#### Get Pointer to Shared Memory

```c++
auto shm_data = shm.get_addr<void *>();
auto shm_data_int = shm.get_addr<int *>();
```

#### Array Access

Access element of shared memory without range check (dangerous):
```c++
const auto value = shm.operator[]<int>(1);
```

Access element of shared memory with range check:
```c++
const auto value = shm.at<long>(42);
```

### Get Shared Memory Information

#### Get Name

```c++
const std::string& shm_name = shm.get_name();
```

#### Get Size

```c++
const std::size_t shm_size = shm.get_size();
```
