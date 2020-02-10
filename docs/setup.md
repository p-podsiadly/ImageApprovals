# Setup

1. [Dependencies](#Dependencies)

2. [Integrating `ImageApprovals`](#Integrating-ImageApprovals)

    1. [Use case: projects using CMake](#Use-case-projects-using-CMake)

## Dependencies

`ImageApprovals` depends on the following libraries:

  * [`ApprovalTests.cpp`](https://github.com/approvals/ApprovalTests.cpp) (tested with version 8.2.0)

  * [`OpenEXR`](https://www.openexr.com/) (tested with version 2.4.0) 

  * [`libpng`](http://www.libpng.org/pub/png/libpng.html) (tested with version 1.6.37)

Additionally, tests of `ImageApprovals` itself use [`doctest`](https://github.com/onqtam/doctest).

All of the dependencies are available from [Conan package manager](https://conan.io/). 
By default, `ImageApprovals` itself uses Conan to obtain these libraries.

## Integrating `ImageApprovals`

### Use case: projects using CMake

For projects using CMake, the preferred way of integrate `ImageApprovals` is to include
the library directly into the source folder (for example, as a Git submodule).

In this case, it is enough to include `ImageApprovals` root directory using CMake's `add_subdirectory`
command and link against `ImageApprovals::ImageApprovals` CMake target:

    set(ImageApprovals_ENABLE_TESTS OFF)

    add_subdirectory(ImageApprovals)

    # ...

    add_executable(TestsUsingImageApprovals ${sources})
    
    target_link_library(TestsUsingImageApprovals PRIVATE ImageApprovals::ImageApprovals)

The following `CACHE` variables control what CMake build script does:

  * `ImageApprovals_ENABLE_TESTS` (default `ON`) controls whether tests of the library are built.

  * `ImageApprovals_RUN_CONAN` (default `ON`) controls whether CMake build script runs `conan install`
    command to automatically download dependencies.

In case of a project which already uses Conan for dependency management, it might be preferrable
to run `conan install` only once - in this case `ImageApprovals_RUN_CONAN` variable can be set to `OFF`,
before including `ImageApprovals` as a subdirectory.
