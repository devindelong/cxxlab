/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include <array>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

namespace cxxlab::spsc
{

template <typename Self, typename T>
struct like_pointer : std::type_identity<std::conditional_t<
                         std::is_const_v<std::remove_reference_t<Self>>,
                         std::add_pointer_t<std::add_const_t<T>>,
                         std::add_pointer_t<T>>>
{
};

template <typename Self, typename T>
using like_pointer_t = like_pointer<Self, T>::type;

namespace detail
{

template <typename T>
class slot
{
 public:
   using value_type = T;
   using pointer = T*;
   using const_pointer = T const*;
   using reference = T&;
   using const_reference = T const&;

   /**
    * @brief Gets a reference to the stored object.
    * @return Reference to the stored object.
    */
   template <typename Self>
   [[nodiscard]] auto get(this Self&& self) -> auto&&
   {
      return std::forward_like<Self>(*self.ptr());
   }

   /**
    * @brief Constructs T via placement new.
    * @param args Arguments for the constructor.
    */
   template <typename... Args>
   auto construct(Args&&... args) -> void
   {
      new (storage_.data()) T{std::forward<Args>(args)...};
   }

   /**
    * @brief Calls the destructor of T.
    * @details No-op for trivially destructible types.
    */
   auto destroy() -> void
   {
      // Only call the destructor for non-trivially-destructible types.
      if constexpr (not std::is_trivially_destructible_v<T>)
      {
         std::destroy_at(ptr());
      }
   }

 private:
   /**
    * @brief Gets a pointer to the stored object.
    * @return Pointer to the stored object.
    */
   template <typename Self>
   [[nodiscard]] auto ptr(this Self&& self) -> like_pointer_t<Self, T>
   {
      // std::launder saves us from having to store the pointer returned from placement new.
      return std::launder(reinterpret_cast<like_pointer_t<Self, T>>(self.storage_.data()));
   }

   alignas(T) std::array<std::byte, sizeof(T)> storage_;
};

} // namespace detail
} // namespace cxxlab::spsc
