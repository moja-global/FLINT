#pragma once

#include "_core_exports.h"

#include <filesystem>

namespace moja::filesystem {

using namespace std::filesystem;

inline path CORE_API expand(const std::string& path);
inline path CORE_API expand(const path& path);

}  // namespace moja::filesystem
