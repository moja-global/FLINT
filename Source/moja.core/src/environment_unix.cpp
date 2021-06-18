#include "moja/environment_unix.h"

#include <cstring>
#include <errno.h>
#include <filesystem>
#include <unistd.h>

namespace moja {

std::string EnvironmentImpl::startProcessFolderImpl() {
   namespace fs = std::filesystem;

   static bool bHaveResult = false;
   static char charBuffer[1024] = "";

   if (!bHaveResult) {
      if (readlink("/proc/self/exe", charBuffer, sizeof(charBuffer) - 1) == -1) {
         int ErrNo = errno;
         // unreachable
         return "";
      }
      bHaveResult = true;
   }
   fs::path path(charBuffer);
   return path.remove_filename().string();
}

}  // namespace moja
