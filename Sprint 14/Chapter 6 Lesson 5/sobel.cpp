#include "ppm_image.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string_view>

using namespace std;

int Sum(img_lib::Color c) {
    return to_integer<int>(c.r) + to_integer<int>(c.g) + to_integer<int>(c.b);
}

// реализуйте оператор Собеля
img_lib::Image Sobel(const img_lib::Image& image) {
    using namespace img_lib;
    int height = image.GetHeight();
    int width = image.GetWidth();
    Image result_image(width, height, Color::Black());

    //отступаем от краёв по 1 пикселю, чтобы применить крайние значения в формуле
    for (int y = 1; y < height - 1; ++y) {
        const Color* top_color = image.GetLine(y - 1);
        const Color* center_color = image.GetLine(y);
        const Color* bottom_color = image.GetLine(y + 1);

        for (int x = 1; x < width - 1; ++x) {
            const int tl = Sum(top_color[x - 1]);
            const int tc = Sum(top_color[x]);
            const int tr = Sum(top_color[x + 1]);
            const int cl = Sum(center_color[x - 1]);
            const int cr = Sum(center_color[x + 1]);
            const int bl = Sum(bottom_color[x - 1]);
            const int bc = Sum(bottom_color[x]);
            const int br = Sum(bottom_color[x + 1]);

            const int gx = -(tl + 2 * tc + tr) + bl + 2 * bc + br;
            const int gy = -(tl + 2 * cl + bl) + tr + 2 * cr + br;

            double sobel_value = std::clamp<double>(static_cast<double>(sqrt(gx * gx + gy * gy)), 0, 255);
            Color* color = &result_image.GetPixel(x, y);
            color->r = color->g = color->b = static_cast<byte>(sobel_value);
        }
    }
    return result_image;
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <input image> <output image>"sv << endl;
        return 1;
    }

    auto image = img_lib::LoadPPM(argv[1]);
    if (!image) {
        cerr << "Error loading image"sv << endl;
        return 2;
    }

    image = Sobel(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
}