/*
 * Copyright 2012-present Facebook, Inc.
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

#include <folly/functional/Invoke.h>

namespace folly {

//////////////////////////////////////////////////////////////////////

/**
 * Helper to generate an index sequence from a tuple like type
 */
template <typename Tuple>
using index_sequence_for_tuple =
	std::make_index_sequence<std::tuple_size<Tuple>::value>;

namespace detail {
namespace apply_tuple {
namespace adl {
	using std::get;

	struct ApplyInvoke {
		template <typename T>
		using seq = index_sequence_for_tuple<std::remove_reference_t<T>>;

		template <typename F, typename T, std::size_t... I>
		static constexpr auto
			invoke_(F&& f, T&& t, std::index_sequence<I...>) noexcept(
				is_nothrow_invocable<F&&, decltype(get<I>(std::declval<T>()))...>::value)
			-> invoke_result_t<F&&, decltype(get<I>(std::declval<T>()))...> {
			return invoke(static_cast<F&&>(f), get<I>(static_cast<T&&>(t))...);
		}
	};

	template <
		typename Tuple,
		std::size_t... Indices,
		typename ReturnTuple =
		std::tuple<decltype(get<Indices>(std::declval<Tuple>()))...>>
		auto forward_tuple(Tuple&& tuple, std::index_sequence<Indices...>)
		-> ReturnTuple {
		return ReturnTuple{ get<Indices>(std::forward<Tuple>(tuple))... };
	}
} // namespace adl
} // namespace apply_tuple
} // namespace detail

struct ApplyInvoke : private detail::apply_tuple::adl::ApplyInvoke {
public:
	template <typename F, typename T>
	constexpr auto operator()(F&& f, T&& t) const noexcept(
		noexcept(invoke_(static_cast<F&&>(f), static_cast<T&&>(t), seq<T>{})))
		-> decltype(invoke_(static_cast<F&&>(f), static_cast<T&&>(t), seq<T>{})) {
		return invoke_(static_cast<F&&>(f), static_cast<T&&>(t), seq<T>{});
	}
};

//////////////////////////////////////////////////////////////////////

//  libc++ v3.9 has std::apply
//  android ndk r15c libc++ claims to be v3.9 but is missing std::apply
#if __cpp_lib_apply >= 201603 ||                   \
    (((__ANDROID__ && _LIBCPP_VERSION > 3900) ||   \
      (!__ANDROID__ && _LIBCPP_VERSION > 3800)) && \
     _LIBCPP_STD_VER > 14) ||                      \
    (_MSC_VER && _HAS_CXX17)

	/* using override */ using std::apply;

#else // __cpp_lib_apply >= 201603

//  mimic: std::apply, C++17
	template <typename F, typename Tuple>
	constexpr decltype(auto) apply(F&& func, Tuple&& tuple) {
		return ApplyInvoke{}(static_cast<F&&>(func), static_cast<Tuple&&>(tuple));
	}

#endif // __cpp_lib_apply >= 201603
}
