#pragma once
#include "img_lib.h"

#include <filesystem>

namespace img_lib {
using Path = std::filesystem::path;

bool SaveBMP(const Path& file, const Image& image);
Image LoadBMP(const Path& file);

} // of namespace img_lib
