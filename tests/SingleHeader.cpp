#define APPROVALS_DOCTEST
#include <ApprovalTests.hpp>

#define ImageApprovals_CONFIG_WITH_LIBPNG
#define ImageApprovals_CONFIG_WITH_OPENEXR
#define ImageApprovals_CONFIG_IMPLEMENT
#include <ImageApprovals.hpp>

#include <set>

TEST_CASE("SingleHeader")
{
    const auto orderedExtensions = ImageApprovals::ImageCodec::getRegisteredExtensions();

    std::set<std::string> extensions(orderedExtensions.begin(), orderedExtensions.end());

    REQUIRE_EQ(extensions.count(".png"), 1);
    REQUIRE_EQ(extensions.count(".exr"), 1);
}