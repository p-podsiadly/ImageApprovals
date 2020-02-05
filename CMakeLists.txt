cmake_minimum_required(VERSION 3.10)

project(ImageApprovals)

option(ImageApprovals_ENABLE_TEST "Enable tests?" ON)

if(NOT CONAN_EXPORTED)
    include("conan.cmake")
    conan_cmake_run(
        REQUIRES "approvaltests.cpp/8.2.0"
                 "libpng/1.6.37"
                 "openexr/2.4.0"
                 "zlib/1.2.11"
                 "doctest/2.3.6"
        BASIC_SETUP
        GENERATORS cmake_find_package
        BUILD missing
    )
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(ImageApprovals)

if(ImageApprovals_ENABLE_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()
