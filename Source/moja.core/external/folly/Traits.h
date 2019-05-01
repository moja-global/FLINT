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

// @author: Andrei Alexandrescu

#pragma once

#include <type_traits>

#include <folly/Portability.h>

namespace folly {
#if __cpp_lib_bool_constant || _MSC_VER

	using std::bool_constant;

#else

	//  mimic: std::bool_constant, C++17void_t
	template <bool B>
	using bool_constant = std::integral_constant<bool, B>;

#endif
}

/**
 * There is a bug in libstdc++, libc++, and MSVC's STL that causes it to
 * ignore unused template parameter arguments in template aliases and does not
 * cause substitution failures. This defect has been recorded here:
 * http://open-std.org/JTC1/SC22/WG21/docs/cwg_defects.html#1558.
 *
 * This causes the implementation of std::void_t to be buggy, as it is likely
 * defined as something like the following:
 *
 *  template <typename...>
 *  using void_t = void;
 *
 * This causes the compiler to ignore all the template arguments and does not
 * help when one wants to cause substitution failures.  Rather declarations
 * which have void_t in orthogonal specializations are treated as the same.
 * For example, assuming the possible `T` types are only allowed to have
 * either the alias `one` or `two` and never both or none:
 *
 *  template <typename T,
 *            typename std::void_t<std::decay_t<T>::one>* = nullptr>
 *  void foo(T&&) {}
 *  template <typename T,
 *            typename std::void_t<std::decay_t<T>::two>* = nullptr>
 *  void foo(T&&) {}
 *
 * The second foo() will be a redefinition because it conflicts with the first
 * one; void_t does not cause substitution failures - the template types are
 * just ignored.
 */

namespace traits_detail {
	template <class T, class...>
	struct type_t_ {
		using type = T;
	};
} // namespace traits_detail

template <class T, class... Ts>
using type_t = typename traits_detail::type_t_<T, Ts...>::type;
template <class... Ts>
using void_t = type_t<void, Ts...>;

template <bool... Bs>
struct Bools {
	using valid_type = bool;
	static constexpr std::size_t size() {
		return sizeof...(Bs);
	}
};

// Lighter-weight than Conjunction, but evaluates all sub-conditions eagerly.
template <class... Ts>
struct StrictConjunction
	: std::is_same<Bools<Ts::value...>, Bools<(Ts::value || true)...>> {};
