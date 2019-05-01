#include "moja/environment_win32u.h"

#include <cstring>
#include <iphlpapi.h>
#include <Poco/Path.h>
#include <Poco/UnicodeConverter.h>
#include <Poco/UnWindows.h>

namespace moja {

static std::string EnvironmentImpl::startProcessFolderImpl() {
	static wchar_t path[512] = "";
	if (!path[0]) {
		// Get directory this executable was launched from.
		GetModuleFileNameW(NULL, path, sizeof(path) - 1);
	}
	Poco::UnicodeConverter::toUTF8(path, result);
	auto folder = Poco::Path(path).parent();
	return folder.toString();
}

} // namespace moja
