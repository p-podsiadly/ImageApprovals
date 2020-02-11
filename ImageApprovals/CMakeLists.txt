find_package(ApprovalTests REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenEXR REQUIRED)
find_package(ZLIB REQUIRED)

set(headers
    "include/ImageApprovals.hpp"

    "include/ImageApprovals/ColorSpace.hpp"
    "include/ImageApprovals/Comparator.hpp"
    "include/ImageApprovals/Image.hpp"
    "include/ImageApprovals/ImageCodec.hpp"
    "include/ImageApprovals/CompareStrategy.hpp"
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
    "src/Comparator.cpp"
    "src/ExrImageCodec.cpp"
    "src/ExrImageCodec.hpp"
    "src/Image.cpp"
    "src/ImageCodec.cpp"
    "src/CompareStrategy.cpp"
    "src/ImageView.cpp"
    "src/PixelFormat.cpp"
    "src/PngImageCodec.cpp"
    "src/PngImageCodec.hpp"
    "src/Units.cpp"
)

add_library(ImageApprovals STATIC ${sources})
target_compile_features(ImageApprovals PUBLIC cxx_std_11)

target_include_directories(ImageApprovals PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(
    ImageApprovals
    PUBLIC
        ApprovalTests::ApprovalTests
    PRIVATE
        PNG::PNG
        OpenEXR::OpenEXR
        ZLIB::ZLIB
)

target_compile_definitions(ImageApprovals PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>")

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
