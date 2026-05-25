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

#include <concepts>
#include <iterator>
#include <utility>

namespace cxxlab::pipes
{

// -----------------------------------------------------------------------------
// take_iterator
// -----------------------------------------------------------------------------

template <std::weakly_incrementable Iterator, std::integral Integral>
class take_iterator : public iterator_interface<Iterator>
{
 public:
   constexpr take_iterator() = default;

   constexpr take_iterator(Iterator iter, Integral count)
       : iterator_interface<Iterator>{std::move(iter)}, count_{count}
   {
   }

   template <typename Elem>
   constexpr auto operator()(Elem&& elem) -> void
   {
      if (taken_ < count_)
      {
         this->write(std::forward<Elem>(elem));
         ++taken_;
      }
   }

   constexpr auto count() const noexcept -> auto { return count_; }
   constexpr auto taken() const noexcept -> auto { return taken_; }

 private:
   Integral count_{0};
   Integral taken_{0};
};

// -----------------------------------------------------------------------------
// take_adaptor
// -----------------------------------------------------------------------------

template <std::integral Integral>
using take_adaptor = generic_adaptor<take_iterator, Integral>;

// -----------------------------------------------------------------------------
// take
// -----------------------------------------------------------------------------

namespace detail
{
struct take_fn
{
   template <std::integral Integral>
   constexpr auto operator()(Integral count) const
   {
      return take_adaptor<Integral>{count};
   }

   template <std::weakly_incrementable Iterator, std::integral Integral>
   constexpr auto operator()(Iterator&& iter, Integral count) const
   {
      return take_iterator{std::forward<Iterator>(iter), count};
   }
};
} // namespace detail

inline constexpr auto take = detail::take_fn{};

} // namespace cxxlab::pipes
