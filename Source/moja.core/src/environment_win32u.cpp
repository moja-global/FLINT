#include "moja/environment_win32u.h"

#include <cstring>
#include <filesystem>
#include <iphlpapi.h>

namespace moja {

std::string EnvironmentImpl::startProcessFolderImpl() {
   namespace fs = std::filesystem;

   std::vector<wchar_t> charBuffer;
   auto size = MAX_PATH;
   do {
      size *= 2;
      charBuffer.resize(size);
   } while (GetModuleFileNameW(NULL, charBuffer.data(), size) == size);

   fs::path path(charBuffer.data());  // Contains the full path including .exe
   return path.remove_filename().string();
}

}  // namespace moja
