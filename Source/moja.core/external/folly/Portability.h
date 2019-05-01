/*
 * Copyright 2011-present Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once


// MSCV 2017 __cplusplus definition by default does not track the C++ version.
// https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
#if !defined(_MSC_VER) || _MSC_VER >= 2000
static_assert(__cplusplus >= 201402L, "__cplusplus >= 201402L");
#endif

#if defined(__GNUC__) && !defined(__clang__)
static_assert(__GNUC__ >= 5, "__GNUC__ >= 5");
#endif

 // It turns out that GNU libstdc++ and LLVM libc++ differ on how they implement
 // the 'std' namespace; the latter uses inline namespaces. Wrap this decision
 // up in a macro to make forward-declarations easier.
#if FOLLY_USE_LIBCPP
#include <__config> // @manual
#define FOLLY_NAMESPACE_STD_BEGIN _LIBCPP_BEGIN_NAMESPACE_STD
#define FOLLY_NAMESPACE_STD_END _LIBCPP_END_NAMESPACE_STD
#else
#define FOLLY_NAMESPACE_STD_BEGIN namespace std {
#define FOLLY_NAMESPACE_STD_END }
#endif

// Unaligned loads and stores
namespace folly {
#if FOLLY_HAVE_UNALIGNED_ACCESS
	constexpr bool kHasUnalignedAccess = true;
#else
	constexpr bool kHasUnalignedAccess = false;
#endif
} // namespace folly

// Generalize warning push/pop.
#if defined(_MSC_VER)
#define FOLLY_PUSH_WARNING __pragma(warning(push))
#define FOLLY_POP_WARNING __pragma(warning(pop))
// Disable the GCC warnings.
#define FOLLY_GNU_DISABLE_WARNING(warningName)
#define FOLLY_GCC_DISABLE_WARNING(warningName)
#define FOLLY_CLANG_DISABLE_WARNING(warningName)
#define FOLLY_MSVC_DISABLE_WARNING(warningNumber) \
  __pragma(warning(disable : warningNumber))
#elif defined(__GNUC__)
// Clang & GCC
#define FOLLY_PUSH_WARNING _Pragma("GCC diagnostic push")
#define FOLLY_POP_WARNING _Pragma("GCC diagnostic pop")
#define FOLLY_GNU_DISABLE_WARNING_INTERNAL2(warningName) #warningName
#define FOLLY_GNU_DISABLE_WARNING(warningName) \
  _Pragma(                                     \
      FOLLY_GNU_DISABLE_WARNING_INTERNAL2(GCC diagnostic ignored warningName))
#ifdef __clang__
#define FOLLY_CLANG_DISABLE_WARNING(warningName) \
  FOLLY_GNU_DISABLE_WARNING(warningName)
#define FOLLY_GCC_DISABLE_WARNING(warningName)
#else
#define FOLLY_CLANG_DISABLE_WARNING(warningName)
#define FOLLY_GCC_DISABLE_WARNING(warningName) \
  FOLLY_GNU_DISABLE_WARNING(warningName)
#endif
#define FOLLY_MSVC_DISABLE_WARNING(warningNumber)
#else
#define FOLLY_PUSH_WARNING
#define FOLLY_POP_WARNING
#define FOLLY_GNU_DISABLE_WARNING(warningName)
#define FOLLY_GCC_DISABLE_WARNING(warningName)
#define FOLLY_CLANG_DISABLE_WARNING(warningName)
#define FOLLY_MSVC_DISABLE_WARNING(warningNumber)
#endif

#ifdef FOLLY_HAVE_SHADOW_LOCAL_WARNINGS
#define FOLLY_GCC_DISABLE_NEW_SHADOW_WARNINGS            \
  FOLLY_GNU_DISABLE_WARNING("-Wshadow-compatible-local") \
  FOLLY_GNU_DISABLE_WARNING("-Wshadow-local")            \
  FOLLY_GNU_DISABLE_WARNING("-Wshadow")
#else
#define FOLLY_GCC_DISABLE_NEW_SHADOW_WARNINGS /* empty */
#endif

// Globally disable -Wshadow for gcc < 5.
#if __GNUC__ == 4 && !__clang__
FOLLY_GCC_DISABLE_NEW_SHADOW_WARNINGS
#endif

