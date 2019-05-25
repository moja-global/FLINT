//
// Platform_POSIX.h
//

#ifndef InT_Core_Platform_POSIX_INCLUDED
#define InT_Core_Platform_POSIX_INCLUDED

//
// Thread-safety of local static initialization
//
#if __cplusplus >= 201103L || __GNUC__ >= 4 || defined(__clang__)
#ifndef INT_LOCAL_STATIC_INIT_IS_THREADSAFE
#define INT_LOCAL_STATIC_INIT_IS_THREADSAFE 1
#endif
#endif

#ifdef __GNUC__
#ifndef __THROW
#ifndef __GNUC_PREREQ
#define __GNUC_PREREQ(maj, min) (0)
#endif
#if defined __cplusplus && __GNUC_PREREQ(2, 8)
#define __THROW throw()
#else
#define __THROW
#endif
#endif

//
// GCC diagnostics enable/disable by Patrick Horgan, see
// http://dbp-consulting.com/tutorials/SuppressingGCCWarnings.html
// use example: MOJA_GCC_DIAG_OFF(unused-variable)
//
#if defined(INT_COMPILER_GCC) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 406)

#ifdef MOJA_GCC_DIAG_OFF
#undef MOJA_GCC_DIAG_OFF
#endif
#ifdef MOJA_GCC_DIAG_ON
#undef MOJA_GCC_DIAG_ON
#endif
#define MOJA_GCC_DIAG_STR(s) #s
#define MOJA_GCC_DIAG_JOINSTR(x, y) MOJA_GCC_DIAG_STR(x##y)
#define MOJA_GCC_DIAG_DO_PRAGMA(x) _Pragma(#x)
#define MOJA_GCC_DIAG_PRAGMA(x) MOJA_GCC_DIAG_DO_PRAGMA(GCC diagnostic x)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#define MOJA_GCC_DIAG_OFF(x)  \
   MOJA_GCC_DIAG_PRAGMA(push) \
   MOJA_GCC_DIAG_PRAGMA(ignored MOJA_GCC_DIAG_JOINSTR(-W, x))
#define MOJA_GCC_DIAG_ON(x) MOJA_GCC_DIAG_PRAGMA(pop)
#else
#define MOJA_GCC_DIAG_OFF(x) MOJA_GCC_DIAG_PRAGMA(ignored MOJA_GCC_DIAG_JOINSTR(-W, x))
#define MOJA_GCC_DIAG_ON(x) MOJA_GCC_DIAG_PRAGMA(warning MOJA_GCC_DIAG_JOINSTR(-W, x))
#endif
#else
#define MOJA_GCC_DIAG_OFF(x)
#define MOJA_GCC_DIAG_ON(x)
#endif

#endif  // __GNUC__

#endif  // InT_Core_Platform_POSIX_INCLUDED
