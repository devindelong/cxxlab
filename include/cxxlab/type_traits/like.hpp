/**
 * @copyright Copyright (c) 2025, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "cxxlab/type_traits/copy_cvref.hpp"

namespace cxxlab
{

/**
 * @brief Type trait that behaves like forward_like and is intended to be used with "deducing this".
 * T.
 *
 * @tparam Self The type to check for const, volatile, and references qualifiers.
 * @tparam T The type to copy const, volatile, and references qualifiers to.
 */
template <typename Self, typename T>
using like = copy_cvref<Self, T>;

/**
 * @brief Type trait that behaves like forward_like and is intended to be used with "deducing this".
 *
 * @tparam Self The type to check for const, volatile, and references qualifiers.
 * @tparam T The type to copy const, volatile, and references qualifiers to.
 */
template <typename Self, typename T>
using like_t = like<Self, T>::type;

template <typename Self, typename T>
struct like_pointer : std::type_identity<std::conditional_t<
                         std::is_const_v<std::remove_reference_t<Self>>,
                         std::add_pointer_t<std::add_const_t<T>>,
                         std::add_pointer_t<T>>>
{
};

template <typename Self, typename T>
using like_pointer_t = like_pointer<Self, T>::type;

} // namespace cxxlab
