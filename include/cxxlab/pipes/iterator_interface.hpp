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
#include <iterator>
#include <type_traits>
#include <utility>

namespace cxxlab::pipes
{

template <typename ValueType>
struct output_iterator_types
{
   using iterator_category = std::output_iterator_tag;
   using value_type = ValueType;
   using difference_type = std::ptrdiff_t;
   using pointer = std::add_pointer_t<ValueType>;
   using reference = std::add_lvalue_reference<ValueType>;
};

// -----------------------------------------------------------------------------
// iterator_interface
// -----------------------------------------------------------------------------

template <typename Iterator>
class iterator_interface : public output_iterator_types<void>
{
   template <typename Self>
   struct assignment_proxy
   {
      assignment_proxy(Self& self) : self_ref_{self} {}

      template <typename Elem>
      constexpr auto operator=(Elem&& elem) const -> assignment_proxy const&
      {
         self_ref_(std::forward<Elem>(elem));
         return *this;
      }

    private:
      std::reference_wrapper<Self> self_ref_;
   };

 public:
   /**
    * @brief Defaulted constructor.
    */
   iterator_interface()
      requires std::default_initializable<Iterator>
   = default;

   /**
    * @brief Construct from an output iterator.
    * @param iter Output iterator.
    */
   constexpr iterator_interface(Iterator iter) : out_iter_{std::move(iter)} {}

   /**
    * @brief No-op, CRTP via "deducing this"
    * @return *this
    */
   template <typename Self>
   constexpr auto operator*(this Self&& self) noexcept
   {
      return assignment_proxy<std::remove_cvref_t<Self>>{self};
   }

   /**
    * @brief No-op, CRTP via "deducing this"
    * @return *this
    */
   template <typename Self>
   constexpr auto operator++(this Self& self) noexcept -> Self&
   {
      return self;
   }

   /**
    * @brief No-op, CRTP via "deducing this"
    * @return *this
    */
   template <typename Self>
   constexpr auto operator++(this Self& self, int) noexcept -> Self&
   {
      return self;
   }

   /**
    * @brief Gets the internal output iterator..
    * @return Returns a possibly-move-constructed copy of the internal iterator member.
    */
   template <typename Self>
   constexpr auto base(this Self&& self) -> auto
   {
      return std::forward_like<Self>(self.out_iter_);
   }

 protected:
   template <typename Self, typename Elem>
   constexpr auto write(this Self&& self, Elem&& elem) -> void
      requires std::output_iterator<Iterator, Elem>
   {
      *self.out_iter_ = std::forward<Elem>(elem);
      ++self.out_iter_;
   }

 private:
   [[no_unique_address]] Iterator out_iter_{};
};

} // namespace cxxlab::pipes
