cmake_minimum_required(VERSION 3.20)
project(cuda_image_filtering LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckLanguage)
check_language(CUDA)

set(FILTER_SOURCES main.cpp blur_cpu.cpp)
if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
    list(APPEND FILTER_SOURCES blur.cu)
else()
    list(APPEND FILTER_SOURCES blur_stub.cpp)
endif()

add_executable(image_filter ${FILTER_SOURCES})
target_include_directories(image_filter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if(CMAKE_CUDA_COMPILER)
    set_target_properties(image_filter PROPERTIES CUDA_STANDARD 17 CUDA_STANDARD_REQUIRED ON)
endif()

enable_testing()
add_executable(blur_cpu_test tests/test_blur_cpu.cpp blur_cpu.cpp)
target_include_directories(blur_cpu_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME blur_cpu_reference COMMAND blur_cpu_test)
