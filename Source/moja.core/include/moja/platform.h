#ifndef MOJA_CORE_PLATFORM_H_
#define MOJA_CORE_PLATFORM_H_

#include <Poco/Platform.h>

#if defined(POCO_OS_FAMILY_WINDOWS)
#define MOJA_OS_FAMILY_WINDOWS 1
#elif defined(POCO_OS_FAMILY_UNIX)
#define MOJA_OS_FAMILY_UNIX 1
#elif defined(POCO_OS_FAMILY_BSD)
#define MOJA_OS_FAMILY_BSD 1
#endif

#define MOJA_OS POCO_OS
#define MOJA_ARCH POCO_ARCH
#define MOJA_ARCH_LITTLE_ENDIAN POCO_ARCH_LITTLE_ENDIAN

#if defined(POCO_COMPILER_MSVC)
#define MOJA_COMPILER_MSVC
#if (_MSC_VER >= 1910)
#define MOJA_NOEXCEPT noexcept  // _NOEXCEPT
#else
#define MOJA_NOEXCEPT _NOEXCEPT
#endif
#elif defined(POCO_COMPILER_CLANG)
#define MOJA_COMPILER_CLANG
#define MOJA_NOEXCEPT noexcept
#elif defined(POCO_COMPILER_GCC)
#define MOJA_COMPILER_GCC
#define MOJA_NOEXCEPT noexcept
#elif defined(POCO_COMPILER_CBUILDER)
#define MOJA_COMPILER_CBUILDER
#endif

#define MOJA_DEFAULT_NEWLINE_CHARS POCO_DEFAULT_NEWLINE_CHARS

#endif  // MOJA_CORE_PLATFORM_H_
