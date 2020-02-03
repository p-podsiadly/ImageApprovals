cmake_minimum_required(VERSION 2.8.12)

project(test_package)

include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup()

add_executable(test_package "main.cpp")
target_link_libraries(test_package PRIVATE ${CONAN_LIBS})
