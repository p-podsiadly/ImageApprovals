cmake_minimum_required(VERSION 3.10)

project(ImageApprovals VERSION 0.1.0)

include(CMakeDependentOption)

option(ImageApprovals_RUN_CONAN "Run conan install?" ON)
option(ImageApprovals_ENABLE_TESTS "Enable tests?" ON)

cmake_dependent_option(
        ImageApprovals_ENABLE_QT5_TESTS
        "Enable tests of Qt5 integration?" ON
        ImageApprovals_ENABLE_TESTS OFF)

if(NOT CONAN_EXPORTED AND ImageApprovals_RUN_CONAN)

    set(conan_requires
        "approvaltests.cpp/8.4.0"
        "libpng/1.6.37"
        "openexr/2.4.0"
        "zlib/1.2.11"
    )

    if(ImageApprovals_ENABLE_TESTS)
        list(APPEND conan_requires "doctest/2.3.6")
    endif()

    include("cmake-conan/conan.cmake")
    
    conan_cmake_run(
        REQUIRES ${conan_requires}
        BASIC_SETUP
        GENERATORS cmake_find_package
        BUILD missing
    )

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")

endif()

add_subdirectory(ImageApprovals)

if(ImageApprovals_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
