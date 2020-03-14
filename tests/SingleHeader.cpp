#define APPROVALS_DOCTEST
#include <ApprovalTests.hpp>

#define ImageApprovals_CONFIG_WITH_LIBPNG
#define ImageApprovals_IMPLEMENT
#include <ImageApprovals.hpp>

TEST_CASE("SingleHeader")
{
    const auto extensions = ImageApprovals::ImageCodec::getRegisteredExtensions();

    REQUIRE_EQ(extensions.size(), 2);
    REQUIRE_EQ(extensions[0], ".png");
    REQUIRE_EQ(extensions[1], ".exr");
}