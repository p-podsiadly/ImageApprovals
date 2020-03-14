find_package(ApprovalTests REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenEXR REQUIRED)
find_package(ZLIB REQUIRED)

if(ImageApprovals_ENABLE_QT5_INTEGRATION)
    find_package(Qt5 COMPONENTS Gui REQUIRED)
endif()

set(headers
    "include/ImageApprovals.hpp"

    "include/ImageApprovals/ColorSpace.hpp"
    "include/ImageApprovals/CompareStrategy.hpp"
    "include/ImageApprovals/Errors.hpp"
    "include/ImageApprovals/Image.hpp"
    "include/ImageApprovals/ImageCodec.hpp"
    "include/ImageApprovals/ImageComparator.hpp"
    "include/ImageApprovals/ImageView.hpp"
    "include/ImageApprovals/ImageWriter.hpp"
    "include/ImageApprovals/PixelFormat.hpp"
    "include/ImageApprovals/Units.hpp"
)

set(sources
    ${headers}

    "src/ColorSpace.cpp"
    "src/ColorSpaceUtils.cpp"
    "src/ColorSpaceUtils.hpp"
    "src/CompareStrategy.cpp"
    "src/ExrImageCodec.cpp"
    "src/ExrImageCodec.hpp"
    "src/Image.cpp"
    "src/ImageCodec.cpp"
    "src/ImageComparator.cpp"
    "src/ImageView.cpp"
    "src/PixelFormat.cpp"
    "src/PngImageCodec.cpp"
    "src/PngImageCodec.hpp"
    "src/Units.cpp"
)

configure_file(
    "Version.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ImageApprovals/Version.hpp"
)

add_library(ImageApprovals STATIC ${sources})
target_compile_features(ImageApprovals PUBLIC cxx_std_11)

target_compile_definitions(
    ImageApprovals
    PUBLIC
        -DImageApprovals_CONFIG_WITH_LIBPNG
        -DImageApprovals_CONFIG_WITH_OPENEXR
        -DImageApprovals_CONFIG_WITH_QT5
)

target_include_directories(
    ImageApprovals
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    ImageApprovals
    PUBLIC
        ApprovalTests::ApprovalTests
    PRIVATE
        PNG::PNG
        OpenEXR::OpenEXR
        ZLIB::ZLIB
)

if(ImageApprovals_ENABLE_QT5_INTEGRATION)
    target_link_libraries(ImageApprovals PUBLIC Qt5::Gui)
endif()

add_library(ImageApprovals::ImageApprovals ALIAS ImageApprovals)

install(
    DIRECTORY "include/"
    DESTINATION include
)

install(
    TARGETS ImageApprovals
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
