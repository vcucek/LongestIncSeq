SET(CTEST_OUTPUT_ON_FAILURE ON)

file(GLOB unit_tests "*.cpp")
foreach(test_file ${unit_tests})
    cmake_path(GET test_file STEM test_name)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE ${PROJECT_NAME_LIB})
    set_target_properties(${test_name} PROPERTIES 
        CUDA_ARCHITECTURES 50
        CUDA_SEPARABLE_COMPILATION ON)
endforeach()

add_test(SequenceAnalyserCPU SequenceAnalyserCPUTest)
add_test(SequenceAnalyser::predefined SequenceAnalyserTest)
add_test(SequenceAnalyser::random_10_3 SequenceAnalyserTest random 10 3)
add_test(SequenceAnalyser::random_10000_7 SequenceAnalyserTest random 10000 7)
add_test(SequenceAnalyser::random_100000_1024 SequenceAnalyserTest random 10000o 1024)