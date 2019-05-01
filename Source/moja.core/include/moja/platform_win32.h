#ifndef InT_Core_Platform_WIN32_INCLUDED
#define InT_Core_Platform_WIN32_INCLUDED

#include "moja/_core_exports.h"

#include <Poco/UnWindows.h>
#include <Poco/Platform_WIN32.h>

#if defined(POCO_MSVS_VERSION) && !defined(MOJA_MSVS_VERSION)
	#define MOJA_MSVS_VERSION POCO_MSVS_VERSION
#endif

#if defined(POCO_MSVC_VERSION) && !defined(MOJA_MSVC_VERSION)
    #define MOJA_MSVC_VERSION POCO_MSVC_VERSION
#endif
    
// Unicode Support
#if defined(UNICODE) && !defined(MOJA_WIN32_UTF8)
	#define MOJA_WIN32_UTF8
#endif

// Current instance
extern "C" CORE_API HINSTANCE hInstance;

#endif // InT_Core_Platform_WIN32_INCLUDED
