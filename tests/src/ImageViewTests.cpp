#include <doctest/doctest.h>
#include <ImageApprovals.hpp>
#include <TestsConfig.hpp>

using namespace ImageApprovals;

TEST_CASE("ImageView::copy")
{
    BitwiseCompareStrategy cmpStrategy;

    const auto imgPath = TEST_FILE("cornell.approved.png");

    Image img = ImageCodec::getBestCodec(imgPath).read(imgPath);

    Image imgCopy = img.copy();

    REQUIRE(cmpStrategy.compare(img, imgCopy).passed);
}