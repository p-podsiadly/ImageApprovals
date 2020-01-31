cmake_minimum_required(VERSION 3.10)

project(ImageApprovals VERSION 0.1.0)

option(ImageApprovals_ENABLE_TEST "Enable tests?" ON)

if(NOT CONAN_EXPORTED)
    include("conan.cmake")
    conan_cmake_run(
        REQUIRES "doctest/2.3.6"
                 "libpng/1.6.37"
                 "openexr/2.4.0"
        BASIC_SETUP
        GENERATORS cmake_find_package
        BUILD missing
    )

    add_subdirectory(ApprovalTests.cpp)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(ImageApprovals)

if(ImageApprovals_ENABLE_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()
