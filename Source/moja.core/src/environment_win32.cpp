#include "moja/environment_win32.h"

#include <cstring>
#include <iphlpapi.h>
#include <Poco/Path.h>
#include <Poco/UnWindows.h>

#pragma comment(lib, "psapi.lib")

namespace moja {

std::string EnvironmentImpl::startProcessFolderImpl() {
	static char path[512] = "";
	if (!path[0]) {
		// Get directory this executable was launched from.
		GetModuleFileNameA(NULL, path, sizeof(path) - 1);
	}
	auto folder = Poco::Path(path).parent();
	return folder.toString();
}

} // namespace moja
