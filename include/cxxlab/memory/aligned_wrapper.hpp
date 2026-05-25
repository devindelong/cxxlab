/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "cxxlab/memory/cache_line_size.hpp"

#include <concepts>
#include <cstddef>
#include <new>
#include <utility>

namespace cxxlab
{

/**
 * @brief Lightweight wrapper class to add an alignment to a given type.
 * @detail The class tries to be an "invisible" wrapper by allowing implicit conversions to and from
 * the stored type; Hoever, in contexts where the type is deduced, an explicit get() member function
 * is provided, since implicit conversions will not occur in type-deduced contexts.
 */
template <typename T, std::size_t Alignment>
   requires(Alignment >= alignof(T))
class aligned_wrapper
{
 public:
   /**
    * @brief Gets the alignment.
    * @return The alignment.
    */
   static consteval auto alignment() noexcept -> std::size_t { return Alignment; }

   /**
    * @brief Constructs the stored value from variadic arguments.
    * @param args Variadic args to pass to the confstructor of the stored type.
    */
   template <typename... Args>
      requires std::constructible_from<T, Args...>
   aligned_wrapper(Args&&... args) : value_{std::forward<Args>(args)...}
   {
   }

   /**
    * @brief Constructs the stored value from an initializer list and variadic arguments.
    * @param list Initializer list to pass to the constructor of the stored type.
    * @param args Variadic args to pass to the constructor of the stored type.
    */
   template <typename U, typename... Args>
      requires std::constructible_from<T, std::initializer_list<U>&, Args...>
   aligned_wrapper(std::initializer_list<U> list, Args&&... args)
       : value_{list, std::forward<Args>(args)...}
   {
   }

   /**
    * @brief Gets the stored value.
    * @details This is useful type deduction contexts where implicit conversion to the stored type
    * will not occur.
    * @return The stored value.
    */
   template <typename Self>
   constexpr auto get(this Self&& self) noexcept -> auto&&
   {
      return std::forward_like<Self>(self.value_);
   }

   /**
    * @brief Implicit conversion to l-value.
    * @return An l-value reference to the stored type.
    */
   constexpr operator T&() & noexcept { return value_; }

   /**
    * @brief Implicit conversion to const l-value.
    * @return A const l-value reference to the stored type.
    */
   constexpr operator const T&() const& noexcept { return value_; }

   /**
    * @brief Implicit conversion to r-value.
    * @return An r-value reference to the stored type.
    */
   constexpr operator T&&() && noexcept { return std::move(value_); }

   /**
    * @brief Implicit conversion to const r-value.
    * @return A const r-value reference to the stored type.
    */
   constexpr operator const T&&() const&& noexcept { return std::move(value_); }

 private:
   // Stored type.
   alignas(Alignment) T value_;
};

template <typename T>
using cache_aligned_wrapper = aligned_wrapper<T, constants::cache_line_size>;
} // namespace cxxlab
