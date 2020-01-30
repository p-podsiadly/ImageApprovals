set(sources
    "include/ImageApprovals.hpp"

    "include/ImageApprovals/ColorSpace.hpp"
    "include/ImageApprovals/Comparator.hpp"
    "include/ImageApprovals/Image.hpp"
    "include/ImageApprovals/ImageCodec.hpp"
    "include/ImageApprovals/ImageComparator.hpp"
    "include/ImageApprovals/ImageView.hpp"
    "include/ImageApprovals/PixelFormat.hpp"
    "include/ImageApprovals/StringUtils.hpp"
    "include/ImageApprovals/Units.hpp"

    "src/ColorSpace.cpp"
    "src/Comparator.cpp"
    "src/Image.cpp"
    "src/ImageCodec.cpp"
    "src/ImageComparator.cpp"
    "src/ImageView.cpp"
    "src/PixelFormat.cpp"
    "src/PngImageCodec.cpp"
    "src/Units.cpp"
)

add_library(ImageApprovals STATIC ${sources})
target_compile_features(ImageApprovals PUBLIC cxx_std_11)

target_include_directories(ImageApprovals PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(
    ImageApprovals
    PUBLIC
        CONAN_PKG::approvaltests.cpp
    PRIVATE
        CONAN_PKG::libpng
)
