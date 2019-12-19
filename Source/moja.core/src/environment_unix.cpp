#include "moja/environment_unix.h"

#include <Poco/Path.h>

#include <cstring>
#include <errno.h>
#include <unistd.h>

namespace moja {

std::string EnvironmentImpl::startProcessFolderImpl() {
   static bool bHaveResult = false;
   static char path[1024] = "";

   if (!bHaveResult) {
      if (readlink("/proc/self/exe", path, sizeof(path) - 1) == -1) {
         int ErrNo = errno;
         // unreachable
         return "";
      }
      bHaveResult = true;
   }
   auto folder = Poco::Path(path).parent();
   return folder.toString();
   ;
}

}  // namespace moja
