cmake_minimum_required(VERSION 3.10)

project(ImageApprovals VERSION 0.1.0)

option(ImageApprovals_ENABLE_TEST "Enable tests?" ON)

include("conan.cmake")
conan_cmake_run(
    REQUIRES "approvaltests.cpp/8.0.0"
             "doctest/2.3.6"
             "libpng/1.6.37"
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
)

add_subdirectory(ImageApprovals)

if(ImageApprovals_ENABLE_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()
