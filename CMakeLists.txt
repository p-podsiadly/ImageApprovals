cmake_minimum_required(VERSION 3.10)

project(ImageApprovals)

include("conan.cmake")
conan_cmake_run(
    REQUIRES "approvaltests.cpp/8.0.0"
             "doctest/2.3.6"
             "libpng/1.6.37"
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing
)

add_subdirectory(ImageApprovals)

enable_testing()
add_subdirectory(Tests)
