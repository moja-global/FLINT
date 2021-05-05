#include "moja/environment_win32u.h"

#include <Poco/Path.h>
#include <Poco/UnWindows.h>
#include <Poco/UnicodeConverter.h>

#include <cstring>
#include <iphlpapi.h>

namespace moja {

std::string EnvironmentImpl::startProcessFolderImpl() {

   std::wstring mod_path;
   mod_path.resize(MAX_PATH);
   mod_path.resize(::GetModuleFileNameW(NULL, &mod_path[0], mod_path.size()));
   std::string result;
   Poco::UnicodeConverter::toUTF8(mod_path, result);
   auto folder = Poco::Path(result).parent().absolute();
   return folder.toString();
}

}  // namespace moja
