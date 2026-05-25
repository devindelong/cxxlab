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
#include <memory>
#include <optional>
#include <type_traits>

namespace cxxlab
{

/**
 * @brief A cache which does not propagate its state.
 *
 * Lightweight utility class for temporarily caching a value of type T within an object,
 * where the cache does not propagate across copies or moves. It is designed to support
 * memoization or lazy computation in contexts where cached state is local to a particular
 * object instance.
 *
 * This class is particularly useful in range adaptors and views, where intermediate
 * results may be expensive to compute, but caching across copies would break the expected
 * behavior of view semantics.
 */
template <typename T>
   requires std::is_object_v<T>
class transient_cache
{
 public:
   /**
    * @brief Default constructor.
    */
   transient_cache() = default;

   /**
    * @brief Copy constructor resets destination, leaves source unchanged.
    * @param other Source object to copy from.
    */
   constexpr transient_cache([[maybe_unused]] transient_cache const& other) noexcept
       : value_{std::nullopt}
   {
   }

   /**
    * @brief Move constructor resets both source and destination.
    * @param other Source object to copy from.
    */
   constexpr transient_cache(transient_cache&& other) noexcept : value_{std::nullopt}
   {
      other.value_.reset();
   }

   /**
    * @brief Copy-assignment resets destination, leaves source unchanged.
    * @param other Source object to copy from.
    * @return Reference to this.
    */
   constexpr auto operator=(transient_cache const& other) noexcept -> transient_cache&
   {
      if (this != std::addressof(other))
      {
         value_.reset();
      }
      return *this;
   }

   /**
    * @brief Move-assignment resets both source and destination.
    * @param other Source object to copy from.
    * @return Reference to this.
    */
   constexpr auto operator=(transient_cache&& other) noexcept -> transient_cache&
   {
      value_.reset();
      other.value_.reset();
      return *this;
   }

   /**
    * @brief Gets a l-value reference to the cached value.
    * @return Reference to the cached value.
    */
   constexpr auto operator*(this auto& self) -> decltype(auto) { return *self.value_; }

   /**
    * @brief Gets a const l-value reference to the cached value.
    * @return Reference to the cached value.
    */
   constexpr auto value(this auto& self) -> decltype(auto) { return *self.value_; }

   /**
    * @brief Checks if there is a cached value.
    * @return True if a value is cached, false otherwise.
    */
   constexpr auto has_value() const -> bool { return value_.has_value(); }

   /**
    * @brief Conversion to bool.
    * @return True if a value is cached, false otherwise.
    */
   constexpr operator bool() const noexcept { return value_.has_value(); };

   /**
    * @brief Emplace from constructor arguments.
    * @param args Constructor arguments for T.
    * @return Reference to this.
    */
   template <class... Args>
   constexpr auto emplace(Args&&... args) -> T&
      requires std::constructible_from<T, Args...>
   {
      return value_.emplace(std::forward<Args>(args)...);
   }

 private:
   std::optional<T> value_{};
};

} // namespace cxxlab
