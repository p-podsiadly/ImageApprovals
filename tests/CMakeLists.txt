find_package(doctest REQUIRED)

set(sources
	"src/ComparatorTests.cpp"
	"src/ExrCodecTest.cpp"
	"src/ImageTest.cpp"
	"src/main.cpp"
	"src/PixelDecodeTests.cpp"
	"src/PngCodecTests.cpp"
	"src/ThresholdImageComparatorTests.cpp"
)

configure_file(
	"TestsConfig.hpp.in"
	"include/TestsConfig.hpp"
)

add_executable(ImageApprovalsTests ${sources})

target_include_directories(ImageApprovalsTests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")

target_link_libraries(
	ImageApprovalsTests
	PRIVATE
		ImageApprovals
		doctest::doctest
)

if(MSVC)
	target_compile_options(ImageApprovalsTests PRIVATE /FC)
	target_link_libraries(ImageApprovalsTests PRIVATE -IGNORE:4099)
endif()

doctest_discover_tests(ImageApprovalsTests TEST_PREFIX "ImageApprovals/")
