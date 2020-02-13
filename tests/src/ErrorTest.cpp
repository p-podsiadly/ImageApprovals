#include <doctest/doctest.h>
#include <ImageApprovals.hpp>
#include <TestsConfig.hpp>

using namespace ApprovalTests;
using namespace ImageApprovals;

TEST_CASE("ErrorExceptions")
{
    auto disposer = Comparator::registerForAllExtensions<ThresholdCompareStrategy>();

    const auto corrupted = TEST_FILE("png/xhdn0g08.png");
    const auto valid = TEST_FILE("png/basi4a08.png");

    REQUIRE_THROWS_AS(FileApprover::verify(corrupted, valid), ApprovalException);

    REQUIRE_THROWS_AS(FileApprover::verify(valid, corrupted), ApprovalException);
}
