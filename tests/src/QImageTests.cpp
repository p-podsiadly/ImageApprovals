#include <doctest/doctest.h>
#include <TestsConfig.hpp>

#define ImageApprovals_CONFIG_QT5
#include <ImageApprovals.hpp>

using namespace ApprovalTests;
using namespace ImageApprovals;

TEST_CASE("ImageView from QImage")
{
    SUBCASE("Format_RGB888")
    {
        const auto receivedPath = TEST_FILE("qimage/rgb888.received.png");
        const auto approvedPath = TEST_FILE("qimage/rgb888.approved.png");

        QImage image(10, 3, QImage::Format::Format_RGB888);

        for (int y = 0; y < image.height(); ++y)
        {
            const int r = static_cast<int>(y * 255.0 / image.height());

            for (int x = 0; x < image.width(); ++x)
            {
                const int g = static_cast<int>(x * 255.0 / image.width());

                image.setPixelColor(x, y, QColor(r, g, 127));
            }
        }

        ImageWriter writer(image);
        writer.write(receivedPath);

        FileApprover::verify(receivedPath, approvedPath);
    }
}