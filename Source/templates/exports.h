#ifndef MOJA_LIB_CONFIGURATION_INCLUDED
#define MOJA_LIB_CONFIGURATION_INCLUDED

//
// MOJA_JOIN
//
// The following piece of macro magic joins the two
// arguments together, even when one of the arguments is
// itself a macro (see 16.3.1 in C++ standard).  The key
// is that macro expansion of macro arguments does not
// occur in MOJA_DO_JOIN2 but does in MOJA_DO_JOIN.
//
#define MOJA_JOIN(X, Y) MOJA_DO_JOIN(X, Y)
#define MOJA_DO_JOIN(X, Y) MOJA_DO_JOIN2(X, Y)
#define MOJA_DO_JOIN2(X, Y) X##Y

//
// Ensure that MOJA_DLL is default unless MOJA_STATIC is defined
//
#if defined(_WIN32) && defined(_DLL)
#if !defined(MOJA_DLL) && !defined(MOJA_STATIC)
#define MOJA_DLL
#endif
#endif

#if defined(_MSC_VER)
#if defined(MOJA_DLL)
#if defined(_DEBUG)
#define MOJA_LIB_SUFFIX "d.lib"
#else
#define MOJA_LIB_SUFFIX ".lib"
#endif
#elif defined(_DLL)
#if defined(_DEBUG)
#define MOJA_LIB_SUFFIX "mdd.lib"
#else
#define MOJA_LIB_SUFFIX "md.lib"
#endif
#else
#if defined(_DEBUG)
#define MOJA_LIB_SUFFIX "mtd.lib"
#else
#define MOJA_LIB_SUFFIX "mt.lib"
#endif
#endif
#endif

#endif // MOJA_LIB_CONFIGURATION_INCLUDED


#ifndef ${LIBNAME_EXPORT}_EXPORTS_INCLUDED
#define ${LIBNAME_EXPORT}_EXPORTS_INCLUDED

//
// The following block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the ${LIBNAME_EXPORT}_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// ${LIBNAME_EXPORT}_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
//

#if defined(_WIN32) && defined(MOJA_DLL)
#if defined(${LIBNAME_EXPORT}_EXPORTS)
#define ${LIBNAME_EXPORT}_API __declspec(dllexport)
#else
#define ${LIBNAME_EXPORT}_API __declspec(dllimport)
#endif
#endif

#if !defined(${LIBNAME_EXPORT}_API)
#if !defined(${LIBNAME_EXPORT}_NO_GCC_API_ATTRIBUTE) && defined (__GNUC__) && (__GNUC__ >= 4)
#define ${LIBNAME_EXPORT}_API __attribute__ ((visibility ("default")))
#else
#define ${LIBNAME_EXPORT}_API
#endif
#endif

//
// Automatically link ${LIBNAME} library.
//
#if defined(_MSC_VER)
#if !defined(MOJA_NO_AUTOMATIC_LIBS) && !defined(${LIBNAME_EXPORT}_EXPORTS)
#pragma comment(lib, "${LIBNAME}" MOJA_LIB_SUFFIX)
#endif
#endif

#endif // ${LIBNAME_EXPORT}_EXPORTS_INCLUDED


#ifndef MOJA_GLOBAL_INCLUDES
#define MOJA_GLOBAL_INCLUDES

//
// Pull in basic definitions
//

#include <Poco/Foundation.h>

#include "moja/types.h"
#include <string>
#include <memory>

#endif // MOJA_GLOBAL_INCLUDES


#ifndef MOJA_PLATFORM_CONFIGURATION_INCLUDED
#define MOJA_PLATFORM_CONFIGURATION_INCLUDED

//
// Include platform-specific definitions
//
#include "moja/platform.h"
#if defined(_WIN32)
	#include "moja/platform_win32.h"
#elif defined(MOJA_OS_FAMILY_UNIX)
	#include "moja/platform_posix.h"
#endif

//
// Include alignment settings early
//
#include "moja/alignment.h"

//
// Cleanup inconsistencies
//
#ifdef MOJA_OS_FAMILY_WINDOWS
#if defined(MOJA_WIN32_UTF8) && defined(MOJA_NO_WSTRING)
#error MOJA_WIN32_UTF8 and MOJA_NO_WSTRING are mutually exclusive.
#endif
#else
#ifdef MOJA_WIN32_UTF8
#undef MOJA_WIN32_UTF8
#endif
#endif

#endif // MOJA_PLATFORM_CONFIGURATION_INCLUDED
