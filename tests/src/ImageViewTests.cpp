#include <doctest/doctest.h>
#include <ImageApprovals.hpp>
#include <TestsConfig.hpp>

using namespace ImageApprovals;

TEST_CASE("ImageView::copy")
{
    BitwiseCompareStrategy cmpStrategy;

    Image img = ImageCodec::read(TEST_FILE("cornell.approved.png"));

    Image imgCopy = img.copy();

    REQUIRE(cmpStrategy.compare(img, imgCopy).passed);
}