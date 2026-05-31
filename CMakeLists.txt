cmake_minimum_required(VERSION 3.15)
project(PIT_Sim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- OpenMP ---
# On Apple Silicon, Homebrew LLVM provides OpenMP since Apple Clang does not.
# On Linux, the system compiler typically supports OpenMP natively.
if(APPLE)
    # Allow override via environment; default to Homebrew arm64 path.
    if(NOT DEFINED LLVM_PREFIX)
        set(LLVM_PREFIX "$ENV{LLVM_PREFIX}")
        if(NOT LLVM_PREFIX)
            set(LLVM_PREFIX "/opt/homebrew/opt/llvm")
        endif()
    endif()

    set(CMAKE_CXX_COMPILER "${LLVM_PREFIX}/bin/clang++")

    add_library(OpenMP::OpenMP_CXX IMPORTED INTERFACE)
    set_target_properties(OpenMP::OpenMP_CXX PROPERTIES
        INTERFACE_COMPILE_OPTIONS  "-fopenmp"
        INTERFACE_INCLUDE_DIRECTORIES "${LLVM_PREFIX}/include"
        INTERFACE_LINK_LIBRARIES  "${LLVM_PREFIX}/lib/libomp.dylib"
    )

    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
    include_directories("/opt/homebrew/include/eigen3")
else()
    find_package(OpenMP REQUIRED)
endif()

# --- Eigen3 ---
find_package(Eigen3 QUIET)
if(NOT Eigen3_FOUND)
    # Fallback: header-only, so a bare include path is enough.
    message(STATUS "Eigen3 not found via find_package; trying pkg-config")
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(EIGEN3 QUIET eigen3)
    endif()
    if(NOT EIGEN3_FOUND AND NOT Eigen3_FOUND)
        message(FATAL_ERROR "Eigen3 not found. Install via 'brew install eigen' or 'apt install libeigen3-dev'.")
    endif()
endif()

# --- Optional BLAS (not strictly required by current code) ---
find_package(BLAS QUIET)

# --- Library ---
add_library(pit_core STATIC
    src/core/LightTransport.cpp
    src/biology/ICDKinetics.cpp
)

target_include_directories(pit_core PUBLIC
    include
    ${EIGEN3_INCLUDE_DIRS}
    $<TARGET_PROPERTY:Eigen3::Eigen,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(pit_core
    PUBLIC  OpenMP::OpenMP_CXX
    $<$<BOOL:${BLAS_FOUND}>:${BLAS_LIBRARIES}>
)

# --- Executable ---
add_executable(pit_sim src/main.cpp)
target_link_libraries(pit_sim PRIVATE pit_core)

# --- Compiler warnings (helps catch issues early) ---
if(MSVC)
    target_compile_options(pit_core PRIVATE /W4)
    target_compile_options(pit_sim  PRIVATE /W4)
else()
    target_compile_options(pit_core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(pit_sim  PRIVATE -Wall -Wextra -Wpedantic)
endif()
