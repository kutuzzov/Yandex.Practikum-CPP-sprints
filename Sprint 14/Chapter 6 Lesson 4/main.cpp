#include "ppm_image.h"

#include <iostream>
#include <string_view>

using namespace std;

byte Negate(byte b) {
    return byte(255 - to_integer<int>(b));
}

img_lib::Color Negate(img_lib::Color c) {
    return {Negate(c.r), Negate(c.g), Negate(c.b), c.a};
}

void NegateInplace(img_lib::Image& image) {
    for (int y = 0; y < image.GetHeight(); ++y) {
        img_lib::Color* line = image.GetLine(y);
        for (int x = 0; x < image.GetWidth(); ++x) {
            line[x] = Negate(line[x]);
        }
    }
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

    NegateInplace(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
}
