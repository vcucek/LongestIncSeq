cmake_minimum_required(VERSION 3.8)

set(PROJECT_NAME_LIB "LongestIncSeqLib")
set(PROJECT_NAME "LongestIncSeq")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_COMPILER "/opt/cuda/bin/nvcc")
set(CUDA_TOOLKIT_ROOT_DIR "/opt/cuda")

project(${PROJECT_NAME}
  VERSION 0.1
  LANGUAGES CXX CUDA
)

find_package(CUDA 11 REQUIRED)

if(NOT DEFINED CMAKE_CUDA_STANDARD)
    set(CMAKE_CUDA_STANDARD 17)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
endif()

file(GLOB header_files "${CMAKE_SOURCE_DIR}/include/*.hpp")
file(GLOB cpu_source_files "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB gpu_source_files "${CMAKE_SOURCE_DIR}/src/*.cu")

include_directories(include/)

add_library(${PROJECT_NAME_LIB} STATIC ${header_files} ${cpu_source_files} ${gpu_source_files})
set_target_properties(${PROJECT_NAME_LIB} PROPERTIES 
    CUDA_ARCHITECTURES 50
    CUDA_SEPARABLE_COMPILATION ON)

target_include_directories(${PROJECT_NAME_LIB} PRIVATE ${CUDA_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME_LIB} PRIVATE
    ${CUDA_LIBRARIES}
    ${CUDA_DRIVER_LIBRARY}
)

add_executable(${PROJECT_NAME} apps/LongestIncSeq.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME_LIB}
) 
set_target_properties(${PROJECT_NAME} PROPERTIES 
    CUDA_ARCHITECTURES 50
    CUDA_SEPARABLE_COMPILATION ON)

include(CTest)

if(BUILD_TESTING)
    SET(CTEST_OUTPUT_ON_FAILURE ON)

    add_subdirectory(tests)

    # Application tests
    add_test(LongestIncSeq::file_argument ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/data/sequence.data)
    add_test(LongestIncSeq::file_argument_cpu ${PROJECT_NAME} cpu ${CMAKE_SOURCE_DIR}/data/sequence.data)
    add_test(LongestIncSeq::file_argument_cuda ${PROJECT_NAME} cuda ${CMAKE_SOURCE_DIR}/data/sequence.data)
    add_test(LongestIncSeq::file_argument_cpu_empty ${PROJECT_NAME} cpu ${CMAKE_SOURCE_DIR}/data/sequence_empty.data)
    add_test(LongestIncSeq::file_argument_cuda_empty ${PROJECT_NAME} cuda ${CMAKE_SOURCE_DIR}/data/sequence_empty.data)
    add_test(LongestIncSeq::file_argument_cpu_err ${PROJECT_NAME} cpu ${CMAKE_SOURCE_DIR}/data/sequence_error.data)
    set_property(TEST LongestIncSeq::file_argument_cpu_err PROPERTY WILL_FAIL TRUE)
    add_test(LongestIncSeq::file_argument_cuda_err ${PROJECT_NAME} cuda ${CMAKE_SOURCE_DIR}/data/sequence_error.data)
    set_property(TEST LongestIncSeq::file_argument_cuda_err PROPERTY WILL_FAIL TRUE)
endif()