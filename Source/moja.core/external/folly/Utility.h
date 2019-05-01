/*
 * Copyright 2016-present Facebook, Inc.
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

namespace folly {

template <typename T>
struct transparent : T {
	using is_transparent = void;
	using T::T;
};

template <typename T>
constexpr auto to_unsigned(T const& t) -> typename std::make_unsigned<T>::type {
	using U = typename std::make_unsigned<T>::type;
	return static_cast<U>(t);
}

template <class E>
constexpr std::underlying_type_t<E> to_underlying_type(E e) noexcept {
	static_assert(std::is_enum<E>::value, "not an enum type");
	return static_cast<std::underlying_type_t<E>>(e);
}

}

