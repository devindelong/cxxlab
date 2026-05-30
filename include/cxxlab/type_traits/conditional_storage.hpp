/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include <initializer_list>
#include <type_traits>

/**
 * @brief Creates a unique empty type.
 * @detail Relies on the fact that every lanbda is a unique closure type.
 */
#define CXXLAB_UNIQUE_EMPTY_TYPE ::cxxlab::empty_type<decltype([] {})>

namespace cxxlab
{
/**
 * @brief An empty type.
 * @tparam T Type parameter used to generate unique empty types.
 */
template <typename T = void>
struct empty_type
{
   /**
    * @brief Construct from variadic args.
    */
   constexpr empty_type(auto&&...) noexcept {}

   /**
    * @brief Construct from initializer list and variadic args.
    */
   template <typename U>
   constexpr empty_type(std::initializer_list<U>, auto&&...) noexcept
   {
   }
};

/**
 * @brief Conditionally keeps or "removes" a variable.
 * @detail Use this with [[no_unique_address]] to avoid storage space for empty types.
 * @tparam HasStorage Indicates if the type evaluates to T or an empty type.
 * @tparam T The desired type if Present is true.
 * @tparam E The empty type if Present is false.
 * @note Be careful when creating an alias to this type. If an alias is created, the empty
 * type will not be unique between instances of each alias and [[no_unique_address]] will
 * result in an extra added byte for each instance of the same type.
 */
template <bool HasStorage, typename T, typename E = CXXLAB_UNIQUE_EMPTY_TYPE>
using conditional_storage_t = std::conditional_t<HasStorage, T, E>;

} // namespace cxxlab
