find_package(doctest REQUIRED)
find_package(ApprovalTests REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenEXR REQUIRED)

set(sources
	"src/ComparatorTests.cpp"
	"src/ErrorTest.cpp"
	"src/ExrCodecTest.cpp"
	"src/ImageTest.cpp"
	"src/ImageWriterTests.cpp"
	"src/main.cpp"
	"src/PixelDecodeTests.cpp"
	"src/PixelPerfectCompareStrategyTests.cpp"
	"src/PngCodecTests.cpp"
	"src/ThresholdCompareStrategyTests.cpp"
)

if(ImageApprovals_ENABLE_QT5_TESTS)

	find_package(Qt5 COMPONENTS Gui REQUIRED)

	list(APPEND sources
		"src/QImageTests.cpp"
	)

endif()

configure_file(
	"TestsConfig.hpp.in"
	"include/TestsConfig.hpp"
)

add_executable(ImageApprovalsTests ${sources})

target_include_directories(ImageApprovalsTests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")

target_link_libraries(
	ImageApprovalsTests
	PRIVATE
		ImageApprovals::ImageApprovals
		doctest::doctest
)

if(ImageApprovals_ENABLE_QT5_TESTS)
	target_link_libraries(ImageApprovalsTests PRIVATE Qt5::Gui)
endif()

if(MSVC)
	target_compile_options(ImageApprovalsTests PRIVATE /FC)
	target_link_libraries(ImageApprovalsTests PRIVATE -IGNORE:4099)
endif()

doctest_discover_tests(ImageApprovalsTests TEST_PREFIX "ImageApprovals/")

add_executable(ImageApprovals_SingleHeaderTest "SingleHeader.cpp")

target_include_directories(ImageApprovals_SingleHeaderTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../single_header/")

target_link_libraries(
	ImageApprovals_SingleHeaderTest
	PRIVATE
		ApprovalTests::ApprovalTests
		doctest::doctest
		OpenEXR::OpenEXR
		ZLIB::ZLIB
		PNG::PNG
)

if(MSVC)
	target_compile_options(ImageApprovals_SingleHeaderTest PRIVATE /FC)
	target_link_libraries(ImageApprovals_SingleHeaderTest PRIVATE -IGNORE:4099)
endif()

doctest_discover_tests(ImageApprovals_SingleHeaderTest TEST_PREFIX "ImageApprovals/")
