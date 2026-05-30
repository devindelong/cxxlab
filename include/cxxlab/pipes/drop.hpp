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
// drop_iterator
// -----------------------------------------------------------------------------

template <std::weakly_incrementable Iterator, std::integral Integral>
class drop_iterator : public iterator_interface<Iterator>
{
 public:
   constexpr drop_iterator() = default;

   constexpr drop_iterator(Iterator&& iter, Integral count)
       : iterator_interface<Iterator>{std::forward<Iterator>(iter)}, count_{count}
   {
   }

   template <typename Elem>
   constexpr auto operator()(Elem&& elem) -> void
   {
      if (dropped_ < count_)
      {
         ++dropped_;
      }
      else
      {
         this->write(std::forward<Elem>(elem));
      }
   }

   constexpr auto count() const noexcept -> Integral { return count_; }
   constexpr auto dropped() const noexcept -> Integral { return dropped_; }

 private:
   Integral count_{0};
   Integral dropped_{0};
};

// -----------------------------------------------------------------------------
// drop_adaptor
// -----------------------------------------------------------------------------

template <std::integral Integral>
using drop_adaptor = generic_adaptor<drop_iterator, Integral>;

// -----------------------------------------------------------------------------
// drop
// -----------------------------------------------------------------------------

namespace detail
{
struct drop_fn
{
   template <std::integral Integral>
   constexpr auto operator()(Integral count) const
   {
      return drop_adaptor<Integral>{count};
   }

   template <std::weakly_incrementable Iterator, std::integral Integral>
   constexpr auto operator()(Iterator&& iter, Integral count) const
   {
      return drop_iterator{std::forward<Iterator>(iter), count};
   }
};
} // namespace detail

inline constexpr auto drop = detail::drop_fn{};
} // namespace cxxlab::pipes
