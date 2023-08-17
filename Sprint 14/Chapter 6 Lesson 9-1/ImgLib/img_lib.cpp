#include "img_lib.h"

namespace img_lib {

Image::Image(int w, int h, Color fill)
    : width_(w)
    , height_(h)
    , step_(w)
    , pixels_(step_ * height_, fill) {
}

Color* Image::GetLine(int y) {
    assert(y >= 0 && y < height_);
    return pixels_.data() + step_ * y;
}

const Color* Image::GetLine(int y) const {
    return const_cast<Image*>(this)->GetLine(y);
}

int Image::GetWidth() const {
    return width_;
}

int Image::GetHeight() const {
    return height_;
}

// шаг задаёт смещение соседних строк изображения
// он обычно совпадает с width, но может быть больше
int Image::GetStep() const {
    return step_;
}

}  // namespace img_lib