#define APPROVALS_DOCTEST
#include <ApprovalTests.hpp>

#define ImageApprovals_IMPLEMENT
#include <ImageApprovals.hpp>

TEST_CASE("SingleHeader")
{
    const auto extensions = ImageApprovals::ImageCodec::getRegisteredExtensions();

    REQUIRE_EQ(extensions.size(), 2);
    REQUIRE_EQ(extensions[0], ".exr");
    REQUIRE_EQ(extensions[1], ".png");
}