#include "moja/environment.h"

#if defined(MOJA_OS_FAMILY_UNIX)
#include "environment_unix.cpp"
#elif defined(MOJA_OS_FAMILY_WINDOWS) && defined(MOJA_WIN32_UTF8)
#include "environment_win32u.cpp"
#elif defined(MOJA_OS_FAMILY_WINDOWS)
#include "environment_win32.cpp"
#endif

namespace moja {

std::string Environment::startProcessFolder() {
	return EnvironmentImpl::startProcessFolderImpl();
}

} // namespace moja
