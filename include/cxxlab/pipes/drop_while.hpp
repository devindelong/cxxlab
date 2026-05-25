/**
 * @copyright Copyright (c) 2026, Devin DeLong. All rights reserved.
 *
 * @license This code is distributed under the BSD 3-Clause License.
 *          See the LICENSE file for the full text of the license.
 *
 * @author Devin DeLong
 */

#pragma once

#include "cxxlab/pipes/adaptor.hpp"
#include "cxxlab/pipes/iterator_interface.hpp"

#include <iterator>
#include <utility>

namespace cxxlab::pipes
{

// -----------------------------------------------------------------------------
// drop_while_iterator
// -----------------------------------------------------------------------------

template <std::weakly_incrementable Iterator, typename Predicate>
class drop_while_iterator : public iterator_interface<Iterator>
{
 public:
   constexpr drop_while_iterator()
      requires std::default_initializable<Predicate>
   = default;

   constexpr drop_while_iterator(Iterator iter, Predicate pred)
       : iterator_interface<Iterator>{std::move(iter)}, predicate_{std::move(pred)}
   {
   }

   template <typename Elem>
   constexpr auto operator()(Elem&& elem) -> void
      requires std::predicate<Predicate&, Elem&>
   {
      // Once should_drop_ is false, it should stay false.
      should_drop_ = should_drop_ ? std::invoke(predicate_, elem) : false;
      if (not should_drop_)
      {
         this->write(std::forward<Elem>(elem));
      }
   }

   template <typename Self>
   constexpr auto predicate(this Self&& self) -> auto
   {
      return std::forward_like<Self>(self.predicate_);
   }

 private:
   [[no_unique_address]] Predicate predicate_;
   bool should_drop_{true};
};

// -----------------------------------------------------------------------------
// drop_while_adaptor
// -----------------------------------------------------------------------------

template <typename Predicate>
using drop_while_adaptor = generic_adaptor<drop_while_iterator, Predicate>;

// -----------------------------------------------------------------------------
// drop_while
// -----------------------------------------------------------------------------

namespace detail
{
struct drop_while_fn
{
   template <typename Predicate>
   constexpr auto operator()(Predicate&& predicate) const
   {
      return drop_while_adaptor<std::remove_cvref_t<Predicate>>{std::forward<Predicate>(predicate)};
   }

   template <std::weakly_incrementable Iterator, typename Predicate>
   constexpr auto operator()(Iterator&& iter, Predicate&& predicate) const
   {
      return drop_while_iterator{std::forward<Iterator>(iter), std::forward<Predicate>(predicate)};
   }
};
} // namespace detail

inline constexpr auto drop_while = detail::drop_while_fn{};

} // namespace cxxlab::pipes
