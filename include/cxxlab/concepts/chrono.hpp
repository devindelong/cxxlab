/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include <chrono>
#include <concepts>
#include <cstddef>
#include <type_traits>

namespace cxxlab
{
namespace detail
{

/**
 * @brief Concept to check if a type is std::chrono::time_point.
 * @detail Private implementation where this fails if T ios cvref qualified.
 */
template <typename T>
concept chrono_time_point_impl = requires {
   typename T::clock;
   typename T::duration;
   typename T::rep;
   typename T::period;
   requires std::same_as<T, std::chrono::time_point<typename T::clock, typename T::duration>>;
};

/**
 * @brief Concept to check if a type is std::chrono::duration.
 * @detail Private implementation where this fails if T ios cvref qualified.
 */
template <typename T>
concept chrono_duration_impl = requires {
   typename T::rep;
   typename T::period;
   requires std::same_as<T, std::chrono::duration<typename T::rep, typename T::period>>;
};
} // namespace detail

/**
 * @brief Concept to check if a type is std::chrono::time_point.
 * @tparam T Possibly cvref qualified type to check.
 */
template <typename T>
concept chrono_time_point = detail::chrono_time_point_impl<std::remove_cvref_t<T>>;

/**
 * @brief Concept to check if a type is std::chrono::duration.
 * @tparam T Possibly cvref qualified type to check.
 */
template <typename T>
concept chrono_duration = detail::chrono_duration_impl<std::remove_cvref_t<T>>;
} // namespace cxxlab
