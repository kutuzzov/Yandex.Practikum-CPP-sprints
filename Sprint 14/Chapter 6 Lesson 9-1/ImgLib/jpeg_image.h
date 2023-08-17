#pragma once
#include "img_lib.h"

#include <filesystem>

namespace img_lib {
using Path = std::filesystem::path;

bool SaveJPEG(const Path& file, const Image& image);
Image LoadJPEG(const Path& file);

} // of namespace img_lib