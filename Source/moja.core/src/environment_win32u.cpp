#include "moja/environment_win32u.h"

#include <Poco/Path.h>
#include <Poco/UnWindows.h>
#include <Poco/UnicodeConverter.h>

#include <cstring>
#include <iphlpapi.h>

namespace moja {

std::string EnvironmentImpl::startProcessFolderImpl() {
   static wchar_t path[512] = L"";
   if (!path[0]) {
      // Get directory this executable was launched from.
      GetModuleFileNameW(NULL, path, sizeof(path) / 2 - 1);
   }

   std::string result = "";
   Poco::UnicodeConverter::toUTF8(path, result);
   auto folder = Poco::Path(result).parent();

   return folder.toString();
}

}  // namespace moja
