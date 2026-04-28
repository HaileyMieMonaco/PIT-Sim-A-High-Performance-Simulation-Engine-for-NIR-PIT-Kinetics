cmake_minimum_required(VERSION 3.15)
project(PIT_Sim VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Force Homebrew LLVM for OpenMP Support ---
if(APPLE)
    set(LLVM_PREFIX "/opt/homebrew/opt/llvm")
    set(CMAKE_CXX_COMPILER "${LLVM_PREFIX}/bin/clang++")
    
    set(OpenMP_CXX_FLAGS "-fopenmp")
    set(OpenMP_omp_LIBRARY "${LLVM_PREFIX}/lib/libomp.dylib")
    
    add_library(OpenMP::OpenMP_CXX IMPORTED INTERFACE)
    set_target_properties(OpenMP::OpenMP_CXX PROPERTIES
        INTERFACE_COMPILE_OPTIONS "-fopenmp"
        INTERFACE_INCLUDE_DIRECTORIES "${LLVM_PREFIX}/include"
        INTERFACE_LINK_LIBRARIES "${OpenMP_omp_LIBRARY}"
    )
    
    # Path hint for Eigen and BLAS on Homebrew Apple Silicon
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
    include_directories("/opt/homebrew/include/eigen3") 
else()
    find_package(OpenMP REQUIRED)
endif()

# --- Dependencies ---
find_package(Eigen3 REQUIRED)
find_package(BLAS REQUIRED)

# --- Build Targets ---
add_library(pit_core STATIC
    src/core/LightTransport.cpp
    src/biology/ICDKinetics.cpp
)

# Use the specific Eigen3 target
target_include_directories(pit_core PUBLIC include ${EIGEN3_INCLUDE_DIRS})
target_link_libraries(pit_core PRIVATE OpenMP::OpenMP_CXX ${BLAS_LIBRARIES})

add_executable(pit_sim src/main.cpp)
target_link_libraries(pit_sim PRIVATE pit_core)