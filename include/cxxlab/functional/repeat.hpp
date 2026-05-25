/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <ranges>
#include <type_traits>

namespace cxxlab
{

/**
 * @brief Call a function or invocable object a number of times,
 * @tparam Callable An invocable type.
 * @param func Callable function or object to call.
 * @param num Number of times to call that function.
 * @not How to handle return values?
 */
template <typename Callable>
constexpr auto
repeat(Callable func, std::size_t num) noexcept(std::is_nothrow_invocable_v<Callable>) -> void
   requires std::invocable<Callable>
{
   for ([[maybe_unused]] auto _ : std::views::iota(std::size_t{0}, num))
   {
      std::invoke(func);
   }
}

} // namespace cxxlab
